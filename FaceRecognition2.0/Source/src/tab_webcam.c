#include "tab_webcam.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <time.h> // Required for time and localtime

#define SOCKET_PATH "/tmp/opencv_gui_socket"
#define IPC_BUFFER_SIZE 1024
#define IPC_DISPLAY_BUFFER_SIZE 2048  // Larger buffer for display messages with prefixes

// IPC message structure
typedef struct {
    int type;
    char data[IPC_BUFFER_SIZE - sizeof(int)];
} IPCMessage;

// IPC message types
#define IPC_MSG_DETECTION 1
#define IPC_MSG_FRAME_PROCESSED 2
#define IPC_MSG_STATUS 3
#define IPC_MSG_ERROR 4

// Global variables
static lv_obj_t* g_webcam_tab = NULL;
static lv_obj_t* g_message_display = NULL;  // Simple message display area
static lv_timer_t* g_ipc_timer = NULL;
static int g_socket_fd = -1;

// Forward declarations
static void ipc_timer_callback(lv_timer_t* timer);
static int setup_ipc_socket(void);
static void cleanup_ipc_socket(void);
static void ipc_message_handler(const IPCMessage* msg);
static void add_message_to_display(const char* message, lv_color_t color);

void tab_webcam_init(lv_obj_t* parent) {
    if (g_webcam_tab) {
        // Already initialized
        return;
    }
    
    // Create tab
    g_webcam_tab = lv_obj_create(parent);
    lv_obj_set_size(g_webcam_tab, 800, 600);
    lv_obj_set_pos(g_webcam_tab, 0, 0);
    lv_obj_set_flex_flow(g_webcam_tab, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(g_webcam_tab, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    
    // Create title
    lv_obj_t* title = lv_label_create(g_webcam_tab);
    lv_label_set_text(title, "Webcam Messages");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create message display area (simple text area)
    g_message_display = lv_textarea_create(g_webcam_tab);
    lv_obj_set_size(g_message_display, 780, 520);
    lv_obj_set_pos(g_message_display, 10, 60);
    lv_obj_set_style_bg_color(g_message_display, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_border_width(g_message_display, 1, 0);
    lv_obj_set_style_border_color(g_message_display, lv_color_hex(0x444444), 0);
    lv_obj_set_style_text_color(g_message_display, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(g_message_display, &lv_font_montserrat_14, 0);
    lv_textarea_set_placeholder_text(g_message_display, "Waiting for webcam application messages...");
    lv_textarea_set_text(g_message_display, "");
    lv_textarea_set_cursor_pos(g_message_display, 0);
    lv_textarea_set_one_line(g_message_display, false);
    lv_textarea_set_max_length(g_message_display, 10000);  // Allow up to 10KB of text
    
    // Set up IPC socket
    if (setup_ipc_socket() != 0) {
        printf("Failed to set up IPC socket\n");
        add_message_to_display("ERROR: Failed to set up IPC socket", lv_color_hex(0xFF0000));
        return;
    }
    
    // Create LVGL timer to check for IPC messages periodically
    g_ipc_timer = lv_timer_create(ipc_timer_callback, 100, NULL); // Check every 100ms
    
    add_message_to_display("Webcam tab initialized successfully", lv_color_hex(0x00FF00));
    add_message_to_display("Waiting for webcam application messages...", lv_color_hex(0x00FFFF));
    
    printf("Webcam tab initialized successfully with IPC support\n");
}

void tab_webcam_cleanup(void) {
    // Stop IPC timer
    if (g_ipc_timer) {
        lv_timer_del(g_ipc_timer);
        g_ipc_timer = NULL;
    }
    
    cleanup_ipc_socket();
    
    // Clean up tab
    if (g_webcam_tab) {
        lv_obj_del(g_webcam_tab);
        g_webcam_tab = NULL;
        g_message_display = NULL;
    }
    
    printf("Webcam tab cleaned up\n");
}

lv_obj_t* tab_webcam_get_tab(void) {
    return g_webcam_tab;
}

void tab_webcam_update_status(const char* status_message) {
    if (status_message) {
        add_message_to_display(status_message, lv_color_hex(0x00FFFF));
    }
}

void tab_webcam_start_webcam(void) {
    add_message_to_display("INFO: Start webcam_ipc_app in another terminal", lv_color_hex(0xFFFF00));
    printf("Webcam start requested - webcam application should be started separately\n");
    printf("Run: ./webcam_ipc_app in another terminal\n");
}

void tab_webcam_stop_processing(void) {
    add_message_to_display("INFO: Stop webcam_ipc_app manually", lv_color_hex(0xFFFF00));
    printf("Webcam stop requested - webcam application should be stopped separately\n");
}

void tab_webcam_set_mode(int mode) {
    (void)mode;
}

// Helper function to add messages to the display
static void add_message_to_display(const char* message, lv_color_t color) {
    if (!g_message_display) return;
    
    // Get current text
    const char* current_text = lv_textarea_get_text(g_message_display);
    
    // Create timestamp
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", tm_info);
    
    // Create new message with timestamp
    char new_message[IPC_DISPLAY_BUFFER_SIZE];
    snprintf(new_message, sizeof(new_message), "[%s] %s\n", timestamp, message);
    
    // Combine with existing text
    char combined_text[IPC_DISPLAY_BUFFER_SIZE];
    if (strlen(current_text) > 0) {
        snprintf(combined_text, sizeof(combined_text), "%s%s", current_text, new_message);
    } else {
        strncpy(combined_text, new_message, sizeof(combined_text) - 1);
    }
    
    // Limit text length to prevent memory issues
    size_t text_len = strlen(combined_text);
    if (text_len > 8000) {  // Keep last 8KB
        const char* start = combined_text + (text_len - 8000);
        // Find the first newline to avoid cutting in the middle of a line
        const char* newline = strchr(start, '\n');
        if (newline) {
            start = newline + 1;
        }
        lv_textarea_set_text(g_message_display, start);
    } else {
        lv_textarea_set_text(g_message_display, combined_text);
    }
    
    // Scroll to bottom
    lv_textarea_set_cursor_pos(g_message_display, strlen(lv_textarea_get_text(g_message_display)));
}

// IPC functions
static int setup_ipc_socket(void) {
    struct sockaddr_un addr;
    
    // Create socket
    g_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (g_socket_fd == -1) {
        perror("Failed to create IPC socket");
        return -1;
    }
    
    // Set up address
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    // Remove existing socket file
    unlink(SOCKET_PATH);
    
    // Bind socket
    if (bind(g_socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Failed to bind IPC socket");
        close(g_socket_fd);
        return -1;
    }
    
    printf("IPC socket bound to %s\n", SOCKET_PATH);
    return 0;
}

static void cleanup_ipc_socket(void) {
    if (g_socket_fd != -1) {
        close(g_socket_fd);
        unlink(SOCKET_PATH);
        g_socket_fd = -1;
    }
}

static void ipc_message_handler(const IPCMessage* msg) {
    switch (msg->type) {
        case IPC_MSG_DETECTION:
            add_message_to_display(msg->data, lv_color_hex(0x00FF00));
            break;
            
        case IPC_MSG_FRAME_PROCESSED:
            {
                char frame_message[IPC_DISPLAY_BUFFER_SIZE];
                snprintf(frame_message, sizeof(frame_message), "Frame: %s", msg->data);
                add_message_to_display(frame_message, lv_color_hex(0x00FFFF));
            }
            break;
            
        case IPC_MSG_STATUS:
            add_message_to_display(msg->data, lv_color_hex(0x00FFFF));
            break;
            
        case IPC_MSG_ERROR:
            {
                char error_message[IPC_DISPLAY_BUFFER_SIZE];
                snprintf(error_message, sizeof(error_message), "ERROR: %s", msg->data);
                add_message_to_display(error_message, lv_color_hex(0xFF0000));
            }
            break;
            
        default:
            {
                char unknown_message[128];
                snprintf(unknown_message, sizeof(unknown_message), "Unknown message type: %d", msg->type);
                add_message_to_display(unknown_message, lv_color_hex(0xFFFF00));
            }
            break;
    }
}

// LVGL timer callback to check for IPC messages
static void ipc_timer_callback(lv_timer_t* timer) {
    (void)timer;
    
    if (g_socket_fd == -1) return;
    
    IPCMessage msg;
    ssize_t bytes_received;
    
    // Try to receive a message (non-blocking)
    bytes_received = recv(g_socket_fd, &msg, sizeof(msg), MSG_DONTWAIT);
    
    if (bytes_received > 0) {
        printf("Received IPC message type: %d\n", msg.type);
        ipc_message_handler(&msg);
    } else if (bytes_received == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
        printf("IPC receive error: %s\n", strerror(errno));
        // Don't break the timer, just log the error
    }
}
