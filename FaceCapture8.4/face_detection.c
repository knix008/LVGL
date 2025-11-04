#include "face_detection.h"
#include <onnxruntime_c_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ONNX Runtime globals
static const OrtApi* ort_api = NULL;
static OrtEnv* ort_env = NULL;
static OrtSession* ort_session = NULL;
static OrtMemoryInfo* memory_info = NULL;
static bool initialized = false;

// Model input/output configuration for YOLOv8
#define INPUT_WIDTH 640
#define INPUT_HEIGHT 640
#define INPUT_CHANNELS 3
#define NUM_CLASSES 1  // YOLOv8-face has 1 class (face)

// Detection parameters
#define CONF_THRESHOLD 0.5f
#define NMS_THRESHOLD 0.45f

// Helper function to compute IoU (Intersection over Union)
static float compute_iou(const FaceBox *a, const FaceBox *b) {
    float x1_min = a->x;
    float y1_min = a->y;
    float x1_max = a->x + a->width;
    float y1_max = a->y + a->height;

    float x2_min = b->x;
    float y2_min = b->y;
    float x2_max = b->x + b->width;
    float y2_max = b->y + b->height;

    float inter_x_min = fmaxf(x1_min, x2_min);
    float inter_y_min = fmaxf(y1_min, y2_min);
    float inter_x_max = fminf(x1_max, x2_max);
    float inter_y_max = fminf(y1_max, y2_max);

    float inter_width = fmaxf(0.0f, inter_x_max - inter_x_min);
    float inter_height = fmaxf(0.0f, inter_y_max - inter_y_min);
    float inter_area = inter_width * inter_height;

    float area1 = a->width * a->height;
    float area2 = b->width * b->height;
    float union_area = area1 + area2 - inter_area;

    return union_area > 0 ? inter_area / union_area : 0.0f;
}

// Non-Maximum Suppression
static void nms(FaceBox *boxes, int *count, float nms_threshold) {
    for (int i = 0; i < *count; i++) {
        if (boxes[i].confidence == 0.0f) continue;

        for (int j = i + 1; j < *count; j++) {
            if (boxes[j].confidence == 0.0f) continue;

            float iou = compute_iou(&boxes[i], &boxes[j]);
            if (iou > nms_threshold) {
                // Suppress the box with lower confidence
                if (boxes[i].confidence > boxes[j].confidence) {
                    boxes[j].confidence = 0.0f;
                } else {
                    boxes[i].confidence = 0.0f;
                }
            }
        }
    }

    // Compact the array by removing suppressed boxes
    int write_idx = 0;
    for (int i = 0; i < *count; i++) {
        if (boxes[i].confidence > 0.0f) {
            if (write_idx != i) {
                boxes[write_idx] = boxes[i];
            }
            write_idx++;
        }
    }
    *count = write_idx;
}

// Preprocess image: resize and normalize
static float* preprocess_image(const uint8_t *rgb_data, int width, int height) {
    float *input_tensor = (float*)malloc(INPUT_WIDTH * INPUT_HEIGHT * INPUT_CHANNELS * sizeof(float));
    if (!input_tensor) {
        fprintf(stderr, "Failed to allocate input tensor\n");
        return NULL;
    }

    // Simple bilinear resize and normalize to [0, 1]
    for (int y = 0; y < INPUT_HEIGHT; y++) {
        for (int x = 0; x < INPUT_WIDTH; x++) {
            int src_x = (int)((float)x / INPUT_WIDTH * width);
            int src_y = (int)((float)y / INPUT_HEIGHT * height);

            if (src_x >= width) src_x = width - 1;
            if (src_y >= height) src_y = height - 1;

            int src_idx = (src_y * width + src_x) * 3;
            int dst_idx = y * INPUT_WIDTH + x;

            // YOLOv8 expects CHW format (channels first), normalized to [0, 1]
            input_tensor[dst_idx] = rgb_data[src_idx] / 255.0f;  // R
            input_tensor[INPUT_WIDTH * INPUT_HEIGHT + dst_idx] = rgb_data[src_idx + 1] / 255.0f;  // G
            input_tensor[INPUT_WIDTH * INPUT_HEIGHT * 2 + dst_idx] = rgb_data[src_idx + 2] / 255.0f;  // B
        }
    }

    return input_tensor;
}

bool face_detection_init(const char *model_path) {
    if (initialized) {
        fprintf(stderr, "Face detection already initialized\n");
        return true;
    }

    ort_api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!ort_api) {
        fprintf(stderr, "Failed to get ONNX Runtime API\n");
        return false;
    }

    // Create environment
    OrtStatus* status = ort_api->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "face_detection", &ort_env);
    if (status != NULL) {
        fprintf(stderr, "Failed to create ORT environment: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        return false;
    }

    // Create session options
    OrtSessionOptions* session_options = NULL;
    status = ort_api->CreateSessionOptions(&session_options);
    if (status != NULL) {
        fprintf(stderr, "Failed to create session options: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseEnv(ort_env);
        return false;
    }

    // Set number of threads
    ort_api->SetIntraOpNumThreads(session_options, 4);
    ort_api->SetSessionGraphOptimizationLevel(session_options, ORT_ENABLE_BASIC);

    // Create session
    status = ort_api->CreateSession(ort_env, model_path, session_options, &ort_session);
    if (status != NULL) {
        fprintf(stderr, "Failed to create session: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseSessionOptions(session_options);
        ort_api->ReleaseEnv(ort_env);
        return false;
    }

    ort_api->ReleaseSessionOptions(session_options);

    // Create memory info
    status = ort_api->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info);
    if (status != NULL) {
        fprintf(stderr, "Failed to create memory info: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseSession(ort_session);
        ort_api->ReleaseEnv(ort_env);
        return false;
    }

    initialized = true;
    printf("Face detection initialized successfully with model: %s\n", model_path);
    return true;
}

bool face_detection_detect(const uint8_t *rgb_data, int width, int height, FaceDetectionResult *result) {
    if (!initialized) {
        fprintf(stderr, "Face detection not initialized\n");
        return false;
    }

    if (!rgb_data || !result) {
        fprintf(stderr, "Invalid input parameters\n");
        return false;
    }

    result->count = 0;

    // Preprocess image
    float *input_tensor = preprocess_image(rgb_data, width, height);
    if (!input_tensor) {
        return false;
    }

    // Create input tensor
    int64_t input_shape[] = {1, INPUT_CHANNELS, INPUT_HEIGHT, INPUT_WIDTH};
    OrtValue* input_tensor_ort = NULL;
    OrtStatus* status = ort_api->CreateTensorWithDataAsOrtValue(
        memory_info,
        input_tensor,
        INPUT_WIDTH * INPUT_HEIGHT * INPUT_CHANNELS * sizeof(float),
        input_shape,
        4,
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        &input_tensor_ort
    );

    if (status != NULL) {
        fprintf(stderr, "Failed to create input tensor: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        free(input_tensor);
        return false;
    }

    // Run inference
    const char* input_names[] = {"images"};
    const char* output_names[] = {"output0"};
    OrtValue* output_tensor = NULL;

    status = ort_api->Run(ort_session, NULL, input_names, (const OrtValue* const*)&input_tensor_ort, 1,
                         output_names, 1, &output_tensor);

    if (status != NULL) {
        fprintf(stderr, "Failed to run inference: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseValue(input_tensor_ort);
        free(input_tensor);
        return false;
    }

    // Get output tensor data
    float* output_data = NULL;
    status = ort_api->GetTensorMutableData(output_tensor, (void**)&output_data);
    if (status != NULL) {
        fprintf(stderr, "Failed to get output data: %s\n", ort_api->GetErrorMessage(status));
        ort_api->ReleaseStatus(status);
        ort_api->ReleaseValue(output_tensor);
        ort_api->ReleaseValue(input_tensor_ort);
        free(input_tensor);
        return false;
    }

    // Get output shape
    OrtTensorTypeAndShapeInfo* output_info = NULL;
    ort_api->GetTensorTypeAndShape(output_tensor, &output_info);
    size_t output_count = 0;
    ort_api->GetTensorShapeElementCount(output_info, &output_count);

    // YOLOv8 output format: [1, 5, num_predictions]
    // 5 = [x, y, w, h, confidence]
    int num_predictions = output_count / 5;

    // Parse detections
    FaceBox temp_boxes[MAX_FACES * 2];  // Temporary buffer for NMS
    int temp_count = 0;

    for (int i = 0; i < num_predictions && temp_count < MAX_FACES * 2; i++) {
        float confidence = output_data[4 * num_predictions + i];

        if (confidence > CONF_THRESHOLD) {
            float cx = output_data[i];
            float cy = output_data[num_predictions + i];
            float w = output_data[2 * num_predictions + i];
            float h = output_data[3 * num_predictions + i];

            // Convert from center format to corner format and normalize
            temp_boxes[temp_count].x = (cx - w / 2.0f) / INPUT_WIDTH;
            temp_boxes[temp_count].y = (cy - h / 2.0f) / INPUT_HEIGHT;
            temp_boxes[temp_count].width = w / INPUT_WIDTH;
            temp_boxes[temp_count].height = h / INPUT_HEIGHT;
            temp_boxes[temp_count].confidence = confidence;

            // Clamp to [0, 1]
            if (temp_boxes[temp_count].x < 0) temp_boxes[temp_count].x = 0;
            if (temp_boxes[temp_count].y < 0) temp_boxes[temp_count].y = 0;
            if (temp_boxes[temp_count].x + temp_boxes[temp_count].width > 1)
                temp_boxes[temp_count].width = 1 - temp_boxes[temp_count].x;
            if (temp_boxes[temp_count].y + temp_boxes[temp_count].height > 1)
                temp_boxes[temp_count].height = 1 - temp_boxes[temp_count].y;

            temp_count++;
        }
    }

    // Apply NMS
    nms(temp_boxes, &temp_count, NMS_THRESHOLD);

    // Copy results to output (up to MAX_FACES)
    result->count = temp_count < MAX_FACES ? temp_count : MAX_FACES;
    for (int i = 0; i < result->count; i++) {
        result->boxes[i] = temp_boxes[i];
    }

    // Cleanup
    ort_api->ReleaseTensorTypeAndShapeInfo(output_info);
    ort_api->ReleaseValue(output_tensor);
    ort_api->ReleaseValue(input_tensor_ort);
    free(input_tensor);

    return true;
}

void face_detection_cleanup(void) {
    if (initialized) {
        if (memory_info) ort_api->ReleaseMemoryInfo(memory_info);
        if (ort_session) ort_api->ReleaseSession(ort_session);
        if (ort_env) ort_api->ReleaseEnv(ort_env);

        memory_info = NULL;
        ort_session = NULL;
        ort_env = NULL;
        initialized = false;

        printf("Face detection cleanup complete\n");
    }
}

bool face_detection_is_initialized(void) {
    return initialized;
}
