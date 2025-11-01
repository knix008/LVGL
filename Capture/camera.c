/**
 * @file camera.c
 * Camera capture logic implementation using V4L2
 */

#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <time.h>
#include <jpeglib.h>

// Buffer structure
struct buffer {
    void *start;
    size_t length;
};

// Static variables
static int camera_fd = -1;
static struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static pthread_t camera_thread;
static bool camera_running = false;
static uint8_t *camera_frame_data = NULL;
static pthread_mutex_t frame_mutex = PTHREAD_MUTEX_INITIALIZER;

// Forward declarations
static void *camera_thread_func(void *arg);

/**
 * Initialize camera device
 */
int camera_init(void)
{
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_capability cap;
    unsigned int i;

    // Allocate frame buffer
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

    // Open camera device
    camera_fd = open(VIDEO_DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (camera_fd == -1) {
        perror("Opening video device");
        fprintf(stderr, "WARNING: Running in test pattern mode (no camera access)\n");
        fprintf(stderr, "To fix: Add your user to 'video' group with: sudo usermod -a -G video $USER\n");
        // Don't return error - continue in test pattern mode
        return 0;
    }

    // Query capabilities
    if (ioctl(camera_fd, VIDIOC_QUERYCAP, &cap) == -1) {
        perror("Querying capabilities");
        close(camera_fd);
        camera_fd = -1;
        free(camera_frame_data);
        camera_frame_data = NULL;
        return -1;
    }

    printf("Camera: %s\n", cap.card);

    // Set format
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = CAMERA_WIDTH;
    fmt.fmt.pix.height = CAMERA_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(camera_fd, VIDIOC_S_FMT, &fmt) == -1) {
        perror("Setting pixel format");
        close(camera_fd);
        camera_fd = -1;
        free(camera_frame_data);
        camera_frame_data = NULL;
        return -1;
    }

    // Request buffers
    memset(&req, 0, sizeof(req));
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(camera_fd, VIDIOC_REQBUFS, &req) == -1) {
        perror("Requesting buffer");
        close(camera_fd);
        camera_fd = -1;
        free(camera_frame_data);
        camera_frame_data = NULL;
        return -1;
    }

    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        close(camera_fd);
        camera_fd = -1;
        free(camera_frame_data);
        camera_frame_data = NULL;
        return -1;
    }

    // Map buffers
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (ioctl(camera_fd, VIDIOC_QUERYBUF, &buf) == -1) {
            perror("Querying buffer");
            camera_cleanup();
            return -1;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length,
                                        PROT_READ | PROT_WRITE,
                                        MAP_SHARED,
                                        camera_fd, buf.m.offset);

        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("mmap");
            camera_cleanup();
            return -1;
        }
    }

    // Queue buffers
    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(camera_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("Queue buffer");
            camera_cleanup();
            return -1;
        }
    }

    // Start streaming
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera_fd, VIDIOC_STREAMON, &type) == -1) {
        perror("Start streaming");
        camera_cleanup();
        return -1;
    }

    printf("Camera initialized successfully\n");
    return 0;
}

/**
 * Cleanup camera resources
 */
void camera_cleanup(void)
{
    unsigned int i;
    enum v4l2_buf_type type;

    if (camera_fd != -1) {
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ioctl(camera_fd, VIDIOC_STREAMOFF, &type);

        if (buffers) {
            for (i = 0; i < n_buffers; ++i)
                munmap(buffers[i].start, buffers[i].length);
            free(buffers);
            buffers = NULL;
        }

        close(camera_fd);
        camera_fd = -1;
    }

    if (camera_frame_data) {
        free(camera_frame_data);
        camera_frame_data = NULL;
    }
}

/**
 * Camera thread - continuously captures frames
 */
static void *camera_thread_func(void *arg)
{
    (void)arg;

    while (camera_running) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(camera_fd, &fds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(camera_fd + 1, &fds, NULL, NULL, &tv);

        if (r == -1) {
            if (errno == EINTR)
                continue;
            perror("select");
            break;
        }

        if (r == 0) {
            fprintf(stderr, "select timeout\n");
            continue;
        }

        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(camera_fd, VIDIOC_DQBUF, &buf) == -1) {
            if (errno == EAGAIN)
                continue;
            perror("VIDIOC_DQBUF");
            break;
        }

        if (buf.index < n_buffers && camera_frame_data) {
            // Copy frame data with mutex protection
            pthread_mutex_lock(&frame_mutex);
            memcpy(camera_frame_data, buffers[buf.index].start,
                   CAMERA_WIDTH * CAMERA_HEIGHT * 3);
            pthread_mutex_unlock(&frame_mutex);
        }

        if (ioctl(camera_fd, VIDIOC_QBUF, &buf) == -1) {
            perror("VIDIOC_QBUF");
            break;
        }
    }

    return NULL;
}

/**
 * Start camera capture thread
 */
int camera_start(void)
{
    if (camera_fd == -1) {
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

    printf("Camera capture started\n");
    return 0;
}

/**
 * Stop camera capture thread
 */
void camera_stop(void)
{
    if (camera_running) {
        camera_running = false;
        pthread_join(camera_thread, NULL);
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
 * Save current frame to file in JPEG format
 */
int camera_save_photo(const char *filename)
{
    if (!camera_frame_data) {
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

    // Set image parameters
    cinfo.image_width = CAMERA_WIDTH;
    cinfo.image_height = CAMERA_HEIGHT;
    cinfo.input_components = 3;           // RGB
    cinfo.in_color_space = JCS_RGB;

    // Set default compression parameters
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);   // Quality: 90 (0-100)

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Write scanlines
    row_stride = CAMERA_WIDTH * 3;
    pthread_mutex_lock(&frame_mutex);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &camera_frame_data[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    pthread_mutex_unlock(&frame_mutex);

    // Finish compression
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    printf("Photo saved: %s\n", filename);
    return 0;
}
