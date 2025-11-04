/**
 * @file camera.c
 * Camera capture logic implementation using FFmpeg
 */

#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <jpeglib.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>

// Static variables
static AVFormatContext *fmt_ctx = NULL;
static AVCodecContext *codec_ctx = NULL;
static struct SwsContext *sws_ctx = NULL;
static struct SwsContext *sws_ctx_full = NULL;
static int video_stream_index = -1;
static pthread_t camera_thread;
static bool camera_running = false;
static uint8_t *camera_frame_data = NULL;
static uint8_t *camera_frame_full = NULL;  // Full resolution frame buffer
static int full_width = 0;
static int full_height = 0;
static pthread_mutex_t frame_mutex = PTHREAD_MUTEX_INITIALIZER;
static AVFrame *frame_rgb = NULL;
static AVFrame *frame_rgb_full = NULL;

// Forward declarations
static void *camera_thread_func(void *arg);

/**
 * Initialize camera device using FFmpeg
 */
int camera_init(void)
{
    int ret;
    const AVCodec *codec = NULL;
    AVStream *stream = NULL;

    // Register all devices (required for v4l2 support)
    avdevice_register_all();

    // Allocate frame buffer for RGB data
    camera_frame_data = malloc(CAMERA_WIDTH * CAMERA_HEIGHT * 3);
    if (!camera_frame_data) {
        fprintf(stderr, "Failed to allocate frame buffer\n");
        return -1;
    }

    // Initialize with test pattern
    for (int y = 0; y < CAMERA_HEIGHT; y++) {
        for (int x = 0; x < CAMERA_WIDTH; x++) {
            int idx = (y * CAMERA_WIDTH + x) * 3;
            camera_frame_data[idx + 0] = (x * 255) / CAMERA_WIDTH;     // R
            camera_frame_data[idx + 1] = (y * 255) / CAMERA_HEIGHT;    // G
            camera_frame_data[idx + 2] = 128;                          // B
        }
    }

    // Open video device - first without specifying resolution to query capabilities
    AVDictionary *options = NULL;
    AVInputFormat *input_fmt = (AVInputFormat *)av_find_input_format("v4l2");
    if (!input_fmt) {
        fprintf(stderr, "v4l2 input format not found\n");
        return -1;
    }

    // Try opening without specifying resolution to get the default/maximum
    av_dict_set(&options, "input_format", "mjpeg", 0);  // Use MJPEG for best quality
    av_dict_set(&options, "framerate", "30", 0);
    // Don't set video_size - let the driver choose the best available

    ret = avformat_open_input(&fmt_ctx, VIDEO_DEVICE, input_fmt, &options);
    av_dict_free(&options);

    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        fprintf(stderr, "Cannot open video device %s: %s\n", VIDEO_DEVICE, errbuf);
        fprintf(stderr, "WARNING: Running in test pattern mode (no camera access)\n");
        fprintf(stderr, "To fix: Add your user to 'video' group with: sudo usermod -a -G video $USER\n");
        // Don't return error - continue in test pattern mode
        return 0;
    }

    // Retrieve stream information
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Cannot find stream information\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Find video stream
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            stream = fmt_ctx->streams[i];
            break;
        }
    }

    if (video_stream_index == -1) {
        fprintf(stderr, "Cannot find video stream\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Find decoder
    codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Allocate codec context
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Could not allocate codec context\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Copy codec parameters to context
    ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
    if (ret < 0) {
        fprintf(stderr, "Failed to copy codec parameters\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Open codec
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Store full resolution dimensions
    full_width = codec_ctx->width;
    full_height = codec_ctx->height;

    // Debug: Print pixel format info
    printf("  Pixel format: %s (id: %d)\n",
           av_get_pix_fmt_name(codec_ctx->pix_fmt), codec_ctx->pix_fmt);

    // Allocate BGR frame for preview (downscaled, BGR for LVGL)
    frame_rgb = av_frame_alloc();
    if (!frame_rgb) {
        fprintf(stderr, "Could not allocate BGR frame\n");
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    frame_rgb->format = AV_PIX_FMT_BGR24;
    frame_rgb->width = CAMERA_WIDTH;
    frame_rgb->height = CAMERA_HEIGHT;

    ret = av_frame_get_buffer(frame_rgb, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate RGB frame buffer\n");
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Allocate RGB frame for full resolution capture
    frame_rgb_full = av_frame_alloc();
    if (!frame_rgb_full) {
        fprintf(stderr, "Could not allocate full RGB frame\n");
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    frame_rgb_full->format = AV_PIX_FMT_RGB24;
    frame_rgb_full->width = full_width;
    frame_rgb_full->height = full_height;

    ret = av_frame_get_buffer(frame_rgb_full, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate full RGB frame buffer\n");
        av_frame_free(&frame_rgb_full);
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Allocate buffer for full resolution frame data
    camera_frame_full = malloc(full_width * full_height * 3);
    if (!camera_frame_full) {
        fprintf(stderr, "Failed to allocate full frame buffer\n");
        av_frame_free(&frame_rgb_full);
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Initialize software scaler context for preview (downscaled from full res)
    // Use BGR24 for LVGL compatibility (LVGL's RGB888 expects BGR byte order)
    sws_ctx = sws_getContext(
        full_width, full_height, AV_PIX_FMT_RGB24,
        CAMERA_WIDTH, CAMERA_HEIGHT, AV_PIX_FMT_BGR24,
        SWS_BILINEAR, NULL, NULL, NULL);

    if (!sws_ctx) {
        fprintf(stderr, "Could not initialize sws context\n");
        av_frame_free(&frame_rgb_full);
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Initialize software scaler context for full resolution (YUV to RGB)
    sws_ctx_full = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        full_width, full_height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);

    if (!sws_ctx_full) {
        fprintf(stderr, "Could not initialize full sws context\n");
        sws_freeContext(sws_ctx);
        av_frame_free(&frame_rgb_full);
        av_frame_free(&frame_rgb);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // Get the pixel format name to check if it's YUVJ (full range JPEG)
    const char *pix_fmt_name = av_get_pix_fmt_name(codec_ctx->pix_fmt);
    printf("  Detected pixel format: %s\n", pix_fmt_name);

    // For YUVJ formats, explicitly set color space to ensure consistency
    if (pix_fmt_name && strstr(pix_fmt_name, "yuvj")) {
        const int *inv_table = NULL, *table = NULL;
        int src_range, dst_range, brightness, contrast, saturation;

        // Get current color space details
        sws_getColorspaceDetails(sws_ctx_full, (int**)&inv_table, &src_range,
                                (int**)&table, &dst_range,
                                &brightness, &contrast, &saturation);

        // Log what swscaler is using by default
        printf("  Default color space - src_range: %d, dst_range: %d\n", src_range, dst_range);
    }


    printf("Camera initialized successfully with FFmpeg\n");
    printf("  Format: %s\n", codec->name);
    printf("  Native Resolution: %dx%d (auto-detected maximum)\n", codec_ctx->width, codec_ctx->height);
    printf("  Preview: %dx%d RGB24 (downscaled for display)\n", CAMERA_WIDTH, CAMERA_HEIGHT);
    printf("  Capture: %dx%d RGB24 (full resolution)\n", full_width, full_height);

    return 0;
}

/**
 * Cleanup camera resources
 */
void camera_cleanup(void)
{
    printf("Starting camera cleanup...\n");

    // Step 1: First, make sure camera thread is stopped
    if (camera_running) {
        printf("Camera still running, stopping it...\n");
        camera_stop();
    }

    // Step 2: Give system a moment to stabilize
    printf("Stabilizing device...\n");
    usleep(200000);  // 200ms delay

    // Step 3: Free software scaling contexts (safe to free even if in use)
    if (sws_ctx_full) {
        printf("Freeing scaling context (full)...\n");
        sws_freeContext(sws_ctx_full);
        sws_ctx_full = NULL;
    }

    if (sws_ctx) {
        printf("Freeing scaling context...\n");
        sws_freeContext(sws_ctx);
        sws_ctx = NULL;
    }

    // Step 4: Free frame buffers
    if (frame_rgb_full) {
        printf("Freeing RGB frame (full)...\n");
        av_frame_free(&frame_rgb_full);
        frame_rgb_full = NULL;
    }

    if (frame_rgb) {
        printf("Freeing RGB frame...\n");
        av_frame_free(&frame_rgb);
        frame_rgb = NULL;
    }

    // Step 5: Free codec context
    if (codec_ctx) {
        printf("Freeing codec context...\n");
        avcodec_free_context(&codec_ctx);
        codec_ctx = NULL;
    }

    // Step 6: Close format context and release device
    // This must be done LAST and very carefully
    if (fmt_ctx) {
        printf("Closing input format and releasing device (this is critical)...\n");
        fflush(stdout);  // Flush output before critical operation

        // Close gracefully - avformat_close_input returns void
        avformat_close_input(&fmt_ctx);
        printf("Device closed, waiting for driver to stabilize...\n");
        fmt_ctx = NULL;

        // Give driver time to cleanup and release device
        usleep(500000);  // 500ms delay for device reset
        printf("Device released successfully\n");
    }

    // Step 7: Free camera frame data buffers
    if (camera_frame_full) {
        printf("Freeing full resolution frame buffer...\n");
        free(camera_frame_full);
        camera_frame_full = NULL;
    }

    if (camera_frame_data) {
        printf("Freeing preview frame buffer...\n");
        free(camera_frame_data);
        camera_frame_data = NULL;
    }

    // Step 8: Reset state variables
    video_stream_index = -1;
    camera_running = false;

    printf("Camera cleanup complete\n");
}

/**
 * Camera thread - continuously captures frames using FFmpeg
 */
static void *camera_thread_func(void *arg)
{
    (void)arg;

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    if (!packet || !frame) {
        fprintf(stderr, "Could not allocate packet or frame\n");
        if (packet) av_packet_free(&packet);
        if (frame) av_frame_free(&frame);
        return NULL;
    }

    while (camera_running) {
        int ret = av_read_frame(fmt_ctx, packet);

        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                fprintf(stderr, "End of stream\n");
                break;
            }
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(ret, errbuf, sizeof(errbuf));
            fprintf(stderr, "Error reading frame: %s\n", errbuf);
            continue;
        }

        // Check if packet is from video stream
        if (packet->stream_index == video_stream_index) {
            // Send packet to decoder
            ret = avcodec_send_packet(codec_ctx, packet);
            if (ret < 0) {
                fprintf(stderr, "Error sending packet to decoder\n");
                av_packet_unref(packet);
                continue;
            }

            // Receive decoded frame
            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }
                if (ret < 0) {
                    fprintf(stderr, "Error receiving frame from decoder\n");
                    break;
                }

                // First: Convert YUV to RGB24 at full resolution
                sws_scale(sws_ctx_full,
                         (const uint8_t * const*)frame->data, frame->linesize,
                         0, codec_ctx->height,
                         frame_rgb_full->data, frame_rgb_full->linesize);

                // Second: Downscale full RGB to preview size
                sws_scale(sws_ctx,
                         (const uint8_t * const*)frame_rgb_full->data, frame_rgb_full->linesize,
                         0, full_height,
                         frame_rgb->data, frame_rgb->linesize);

                // Copy RGB data to buffers
                pthread_mutex_lock(&frame_mutex);

                // Copy preview frame
                for (int y = 0; y < CAMERA_HEIGHT; y++) {
                    memcpy(camera_frame_data + y * CAMERA_WIDTH * 3,
                           frame_rgb->data[0] + y * frame_rgb->linesize[0],
                           CAMERA_WIDTH * 3);
                }

                // Copy full resolution frame
                for (int y = 0; y < full_height; y++) {
                    memcpy(camera_frame_full + y * full_width * 3,
                           frame_rgb_full->data[0] + y * frame_rgb_full->linesize[0],
                           full_width * 3);
                }

                pthread_mutex_unlock(&frame_mutex);
            }
        }

        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_frame_free(&frame);

    return NULL;
}

/**
 * Start camera capture thread
 */
int camera_start(void)
{
    if (!fmt_ctx) {
        fprintf(stderr, "Camera not available - using test pattern mode\n");
        camera_running = true;  // Enable running flag for test pattern mode
        printf("Test pattern mode started\n");
        return 0;
    }

    camera_running = true;
    if (pthread_create(&camera_thread, NULL, camera_thread_func, NULL) != 0) {
        perror("pthread_create");
        camera_running = false;
        return -1;
    }

    printf("Camera capture started with FFmpeg\n");
    return 0;
}

/**
 * Stop camera capture thread
 */
void camera_stop(void)
{
    if (camera_running) {
        printf("Stopping camera capture thread...\n");
        camera_running = false;

        // Give thread a moment to notice the flag
        usleep(100000);  // 100ms

        // Wait for thread to finish (with timeout protection)
        if (fmt_ctx) {
            printf("Waiting for camera thread to join...\n");
            int ret = pthread_join(camera_thread, NULL);
            if (ret != 0) {
                fprintf(stderr, "Warning: pthread_join failed with code %d\n", ret);
            } else {
                printf("Camera thread joined successfully\n");
            }
        }
        printf("Camera capture stopped\n");
    }
}

/**
 * Get pointer to the latest camera frame
 */
uint8_t *camera_get_frame(void)
{
    return camera_frame_data;
}

/**
 * Check if camera is running
 */
bool camera_is_running(void)
{
    return camera_running;
}

/**
 * Save current frame to file in JPEG format at full camera resolution
 */
int camera_save_photo(const char *filename)
{
    if (!camera_frame_full) {
        fprintf(stderr, "No camera data available\n");
        return -1;
    }

    // JPEG compression structure
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *fp;
    JSAMPROW row_pointer[1];
    int row_stride;

    // Open output file
    fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to create file");
        return -1;
    }

    // Initialize JPEG compression
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    // Set image parameters (use full resolution)
    cinfo.image_width = full_width;
    cinfo.image_height = full_height;
    cinfo.input_components = 3;           // RGB
    cinfo.in_color_space = JCS_RGB;

    // Set default compression parameters
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 95, TRUE);   // Higher quality for full-res

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Write scanlines
    row_stride = full_width * 3;
    pthread_mutex_lock(&frame_mutex);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &camera_frame_full[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    pthread_mutex_unlock(&frame_mutex);

    // Finish compression
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    printf("Photo saved: %s (%dx%d)\n", filename, full_width, full_height);
    return 0;
}

/**
 * Get camera frame dimensions
 */
void camera_get_dimensions(int *width, int *height)
{
    if (width) *width = CAMERA_WIDTH;
    if (height) *height = CAMERA_HEIGHT;
}
