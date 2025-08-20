#include "web_server.h"
#include "mongoose.h"
#include "ui_components.h"
#include "tls_config.h"
#include "web_control.h"
#include "video.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

// Global variables
static struct mg_mgr mgr;
static bool web_server_enabled = true;
static bool tls_enabled = false;
static int web_server_port = HTTP_PORT;
static int tls_port = TLS_PORT;
static char web_server_status[256] = "Initializing...";
static struct mg_tls_opts tls_opts = {0};

// UI state management
static char ui_state_json[8192];
static char current_tab[32] = "main";
static bool ui_connected = false;
static int active_connections = 0;

// Forward declarations
static void fn(struct mg_connection *c, int ev, void *ev_data);
static void handle_http_request(struct mg_connection *c, struct mg_http_message *hm);
static void handle_websocket_message(struct mg_connection *c, int ev, void *ev_data);

static const char* generate_ui_state_json(void);
static void send_json_response(struct mg_connection *c, const char* json);
static bool init_tls(void);
static void cleanup_tls(void);
static bool create_self_signed_certificates(void);
static void broadcast_ui_state(void);
static void handle_lvgl_command(const char* command, const char* value);

// HTML files directory path - use absolute path
static char html_dir[256];

void web_server_init(void) {
    if (!web_server_enabled) return;
    
    // Initialize HTML directory path
    snprintf(html_dir, sizeof(html_dir), "%s/html", getcwd(NULL, 0));
    printf("HTML directory: %s\n", html_dir);
    
    mg_mgr_init(&mgr);
    
    // Initialize TLS context
    mg_tls_ctx_init(&mgr);
    
    // Initialize TLS if enabled
    if (init_tls()) {
        tls_enabled = true;
        printf("TLS 1.3 enabled with certificates\n");
    } else {
        printf("TLS disabled - running HTTP only\n");
    }
    
    // Start HTTP server
    char listen_url[64];
    snprintf(listen_url, sizeof(listen_url), "http://0.0.0.0:%d", web_server_port);
    
    if (mg_http_listen(&mgr, listen_url, fn, NULL) != NULL) {
        snprintf(web_server_status, sizeof(web_server_status), 
                "Web server running on port %d", web_server_port);
        printf("Web server started on port %d\n", web_server_port);
        printf("Access the web interface at: http://localhost:%d\n", web_server_port);
    } else {
        snprintf(web_server_status, sizeof(web_server_status), 
                "Failed to start web server on port %d", web_server_port);
        printf("Failed to start web server on port %d\n", web_server_port);
    }
    
    // Start HTTPS server if TLS is enabled
    if (tls_enabled) {
        char tls_listen_url[64];
        snprintf(tls_listen_url, sizeof(tls_listen_url), "https://0.0.0.0:%d", tls_port);
        
        struct mg_connection *https_conn = mg_http_listen(&mgr, tls_listen_url, fn, NULL);
        if (https_conn != NULL) {
            printf("HTTPS server started on port %d\n", tls_port);
            printf("Access the secure web interface at: https://localhost:%d\n", tls_port);
        } else {
            printf("Failed to start HTTPS server on port %d\n", tls_port);
            printf("TLS configuration might be incompatible\n");
        }
    }
}

void web_server_poll(void) {
    if (!web_server_enabled) return;
    mg_mgr_poll(&mgr, 10); // 10ms timeout
}

void web_server_cleanup(void) {
    if (!web_server_enabled) return;
    cleanup_tls();
    mg_tls_ctx_free(&mgr);
    mg_mgr_free(&mgr);
    snprintf(web_server_status, sizeof(web_server_status), "Web server stopped");
}

const char* web_server_get_ui_state_json(void) {
    return generate_ui_state_json();
}

void web_server_set_ui_state_from_json(const char* json) {
    // Parse JSON and update UI state
    // This is a simplified implementation
    printf("Received UI state update: %s\n", json);
}

const char* web_server_get_status(void) {
    return web_server_status;
}

void web_server_set_enabled(bool enabled) {
    web_server_enabled = enabled;
}

bool web_server_is_enabled(void) {
    return web_server_enabled;
}

// TLS-related functions
bool web_server_is_tls_enabled(void) {
    return tls_enabled;
}

int web_server_get_tls_port(void) {
    return tls_port;
}

const char* web_server_get_tls_status(void) {
    if (tls_enabled) {
        return "TLS 1.3 enabled";
    } else {
        return "TLS disabled";
    }
}

// Mongoose event handler
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_ACCEPT) {
        // Initialize TLS for HTTPS connections
        if (c->is_tls && tls_enabled) {
            struct mg_tls_opts opts = {
                .cert = tls_opts.cert,
                .key = tls_opts.key
            };
            mg_tls_init(c, &opts);
            printf("TLS initialized for connection\n");
        }
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        handle_http_request(c, hm);
    } else if (ev == MG_EV_WS_OPEN) {
        // WebSocket connection opened
        printf("WebSocket connection opened (TLS: %s)\n", c->is_tls ? "enabled" : "disabled");
        active_connections++;
        ui_connected = true;
        
        // Send initial UI state to new client
        const char* ui_state = generate_ui_state_json();
        mg_ws_send(c, ui_state, strlen(ui_state), WEBSOCKET_OP_TEXT);
        
    } else if (ev == MG_EV_WS_MSG) {
        // WebSocket message received
        handle_websocket_message(c, ev, ev_data);
    } else if (ev == MG_EV_CLOSE) {
        // Connection closed
        printf("Connection closed\n");
        if (c->is_websocket) {
            active_connections--;
            if (active_connections <= 0) {
                active_connections = 0;
                ui_connected = false;
            }
        }
    }
}

// Handle HTTP requests
static void handle_http_request(struct mg_connection *c, struct mg_http_message *hm) {
    if (mg_strcmp(hm->uri, mg_str("/")) == 0) {
        // Serve main HTML page from external file
        FILE *file = fopen("html/index.html", "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            char *content = malloc(file_size + 1);
            fread(content, 1, file_size, file);
            content[file_size] = '\0';
            fclose(file);
            
            mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", content);
            free(content);
        } else {
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "HTML file not found");
        }
    } else if (mg_strcmp(hm->uri, mg_str("/styles.css")) == 0) {
        // Serve CSS file
        FILE *file = fopen("html/styles.css", "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            char *content = malloc(file_size + 1);
            fread(content, 1, file_size, file);
            content[file_size] = '\0';
            fclose(file);
            
            mg_http_reply(c, 200, "Content-Type: text/css\r\n", "%s", content);
            free(content);
        } else {
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "CSS file not found");
        }
    } else if (mg_strcmp(hm->uri, mg_str("/script.js")) == 0) {
        // Serve JavaScript file
        FILE *file = fopen("html/script.js", "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            char *content = malloc(file_size + 1);
            fread(content, 1, file_size, file);
            content[file_size] = '\0';
            fclose(file);
            
            mg_http_reply(c, 200, "Content-Type: application/javascript\r\n", "%s", content);
            free(content);
        } else {
            mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "JS file not found");
        }
    } else if (mg_strcmp(hm->uri, mg_str("/api/status")) == 0) {
        // API endpoint for status with TLS information
        char json[512];
        snprintf(json, sizeof(json), 
                "{\"status\":\"running\",\"http_port\":%d,\"tls_enabled\":%s,\"tls_port\":%d,\"tls_status\":\"%s\"}",
                web_server_port, 
                tls_enabled ? "true" : "false",
                tls_port,
                web_server_get_tls_status());
        send_json_response(c, json);
    } else if (mg_strcmp(hm->uri, mg_str("/api/ui/state")) == 0) {
        // API endpoint for UI state
        const char* json = generate_ui_state_json();
        send_json_response(c, json);
    } else if (mg_strcmp(hm->uri, mg_str("/test-ws")) == 0) {
        // Serve the WebSocket test page
        struct mg_http_serve_opts opts = { .root_dir = ".", .mime_types = "text/html" };
        mg_http_serve_file(c, hm, "websocket_chrome_test.html", &opts);
    } else if (mg_strcmp(hm->uri, mg_str("/ws")) == 0) {
        // Upgrade to WebSocket
        mg_ws_upgrade(c, hm, NULL);
    } else if (mg_strcmp(hm->uri, mg_str("/assets")) == 0) {
        // Handle static files
        struct mg_http_serve_opts opts = { .root_dir = "assets" };
        mg_http_serve_dir(c, hm, &opts);
    } else {
        // 404 Not Found
        mg_http_reply(c, 404, "Content-Type: text/plain\r\n", "Not Found");
    }
}

// Handle WebSocket messages
static void handle_websocket_message(struct mg_connection *c, int ev, void *ev_data) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    
    // Parse JSON message
    char* json_str = malloc(wm->data.len + 1);
    memcpy(json_str, wm->data.buf, wm->data.len);
    json_str[wm->data.len] = '\0';
    
    printf("WebSocket message received: %s\n", json_str);
    
    // Parse command type and value
    char command_type[64] = "";
    char command_value[256] = "";
    
    // More robust JSON parsing for command
    char* type_ptr = strstr(json_str, "\"type\":\"");
    char* value_ptr = strstr(json_str, "\"value\":\"");
    
    if (type_ptr) {
        type_ptr += 8; // Skip "type":"
        char* end_ptr = strchr(type_ptr, '"');
        if (end_ptr) {
            int len = end_ptr - type_ptr;
            if (len < sizeof(command_type)) {
                strncpy(command_type, type_ptr, len);
                command_type[len] = '\0';
                printf("Parsed command type: '%s'\n", command_type);
            }
        }
    }
    
    if (value_ptr) {
        value_ptr += 9; // Skip "value":"
        char* end_ptr = strchr(value_ptr, '"');
        if (end_ptr) {
            int len = end_ptr - value_ptr;
            if (len < sizeof(command_value)) {
                strncpy(command_value, value_ptr, len);
                command_value[len] = '\0';
                printf("Parsed command value: '%s'\n", command_value);
            }
        }
    }
    
    // Handle the command
    if (strlen(command_type) > 0 && strlen(command_value) > 0) {
        printf("Executing command: type='%s', value='%s'\n", command_type, command_value);
        handle_lvgl_command(command_type, command_value);
        
        // Send response back to client
        char response[512];
        snprintf(response, sizeof(response), 
                "{\"type\":\"response\",\"command\":\"%s\",\"value\":\"%s\",\"status\":\"executed\",\"timestamp\":%ld}",
                command_type, command_value, (long)time(NULL));
        mg_ws_send(c, response, strlen(response), WEBSOCKET_OP_TEXT);
        printf("Sent response to client\n");
    } else {
        printf("Invalid command format - type: '%s', value: '%s'\n", command_type, command_value);
        
        // Send error response
        char error_response[512];
        snprintf(error_response, sizeof(error_response), 
                "{\"type\":\"error\",\"message\":\"Invalid command format\",\"received\":\"%s\"}",
                json_str);
        mg_ws_send(c, error_response, strlen(error_response), WEBSOCKET_OP_TEXT);
    }
    
    // Send current UI state back
    const char* ui_state = generate_ui_state_json();
    mg_ws_send(c, ui_state, strlen(ui_state), WEBSOCKET_OP_TEXT);
    
    free(json_str);
}

// Generate UI state as JSON
static const char* generate_ui_state_json(void) {
    // Get current tab from the actual LVGL application
    tab_index_t current_tab_index = get_current_tab();
    const char* current_tab_name = "db"; // Default
    
    // Map tab index to name
    switch (current_tab_index) {
        case TAB_DB: current_tab_name = "db"; break;
        case TAB_SETTINGS: current_tab_name = "settings"; break;
        case TAB_NUM: current_tab_name = "num"; break;
        case TAB_KOR: current_tab_name = "kor"; break;
        case TAB_CJI: current_tab_name = "cji"; break;
        case TAB_QWERTY: current_tab_name = "qwerty"; break;
        case TAB_CAL: current_tab_name = "cal"; break;
        case TAB_CLOCK: current_tab_name = "clock"; break;
        case TAB_AV: current_tab_name = "video"; break;
        case TAB_OPENCV: current_tab_name = "opencv"; break;
        case TAB_INFO: current_tab_name = "info"; break;
        default: current_tab_name = "db"; break;
    }
    
    // Enhanced UI state with more detailed information
    snprintf(ui_state_json, sizeof(ui_state_json),
        "{"
        "\"type\":\"ui_state\","
        "\"state\":{"
        "\"current_tab\":\"%s\","
        "\"current_tab_index\":%d,"
        "\"timestamp\":%ld,"
        "\"web_server_status\":\"%s\","
        "\"tls_enabled\":%s,"
        "\"tls_port\":%d,"
        "\"active_connections\":%d,"
        "\"ui_connected\":%s,"
        "\"features\":{"
        "\"database\":true,"
        "\"settings\":true,"
        "\"calendar\":true,"
        "\"clock\":true,"
        "\"video\":true,"
        "\"korean_input\":true,"
        "\"chunjiin_input\":true"
        "},"
        "\"system_info\":{"
        "\"uptime\":%ld,"
        "\"memory_usage\":\"normal\","
        "\"cpu_usage\":\"low\""
        "},"
        "\"available_commands\":{"
        "\"tab\":[\"db\",\"settings\",\"info\",\"calendar\",\"clock\",\"video\",\"opencv\",\"korean\",\"chunjiin\",\"num\",\"kor\",\"cji\",\"qwerty\"],"
        "\"video\":[\"play\",\"pause\",\"stop\",\"next\",\"prev\",\"volume\"]"
        "},"
        "\"video_status\":{"
        "\"status\":\"%s\","
        "\"current_file\":\"%s\""
        "}"
        "}"
        "}", 
        current_tab_name,
        current_tab_index,
        (long)time(NULL), 
        web_server_status,
        tls_enabled ? "true" : "false",
        tls_port,
        active_connections,
        ui_connected ? "true" : "false",
        (long)time(NULL), // uptime (simplified)
        video_get_status_string(),
        video_get_current_path()
    );
    
    return ui_state_json;
}

// Send JSON response
static void send_json_response(struct mg_connection *c, const char* json) {
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
}

// Handle LVGL commands from web interface
static void handle_lvgl_command(const char* command, const char* value) {
    printf("Executing LVGL command: %s = %s\n", command, value);
    
    if (strcmp(command, "tab") == 0) {
        // Handle tab switching
        strncpy(current_tab, value, sizeof(current_tab) - 1);
        current_tab[sizeof(current_tab) - 1] = '\0';
        
        // Use the tab control function to actually switch tabs
        switch_to_tab_by_name(value);
        
    } else if (strcmp(command, "system") == 0) {
        // Handle system commands
        if (strcmp(value, "refresh") == 0) {
            printf("Refreshing UI state\n");
            ui_connected = true;
        } else if (strcmp(value, "status") == 0) {
            printf("Getting system status\n");
        } else if (strcmp(value, "restart") == 0) {
            printf("Restart command received (not implemented)\n");
        } else if (strcmp(value, "shutdown") == 0) {
            printf("Shutdown command received (not implemented)\n");
        }
        
    } else if (strcmp(command, "database") == 0) {
        // Handle database commands
        if (strcmp(value, "query") == 0) {
            printf("Executing database query\n");
        } else if (strcmp(value, "backup") == 0) {
            printf("Creating database backup\n");
        } else if (strcmp(value, "optimize") == 0) {
            printf("Optimizing database\n");
        }
        
    } else if (strcmp(command, "video") == 0) {
        // First, switch to video tab automatically
        printf("Web command: Switching to video tab for video control\n");
        switch_to_tab_by_name("video");
        strncpy(current_tab, "video", sizeof(current_tab) - 1);
        current_tab[sizeof(current_tab) - 1] = '\0';
        
        // Then handle video commands
        if (strcmp(value, "play") == 0) {
            printf("Web command: Playing video\n");
            lv_result_t result = video_play();
            if (result == LV_RESULT_OK) {
                printf("Video play command executed successfully\n");
            } else {
                printf("Video play command failed\n");
            }
        } else if (strcmp(value, "pause") == 0) {
            printf("Web command: Pausing video\n");
            lv_result_t result = video_pause();
            if (result == LV_RESULT_OK) {
                printf("Video pause command executed successfully\n");
            } else {
                printf("Video pause command failed\n");
            }
        } else if (strcmp(value, "stop") == 0) {
            printf("Web command: Stopping video\n");
            lv_result_t result = video_stop();
            if (result == LV_RESULT_OK) {
                printf("Video stop command executed successfully\n");
            } else {
                printf("Video stop command failed\n");
            }
        } else if (strcmp(value, "next") == 0) {
            printf("Web command: Next video\n");
            lv_result_t result = video_next();
            if (result == LV_RESULT_OK) {
                printf("Video next command executed successfully\n");
            } else {
                printf("Video next command failed\n");
            }
        } else if (strcmp(value, "prev") == 0) {
            printf("Web command: Previous video (not implemented)\n");
        }
        
    } else if (strcmp(command, "custom") == 0) {
        // Handle custom commands
        printf("Custom command: %s\n", value);
    }
    
    // Broadcast updated UI state to all connected clients
    broadcast_ui_state();
}

// Broadcast UI state to all connected WebSocket clients
static void broadcast_ui_state(void) {
    const char* ui_state = generate_ui_state_json();
    
    // Iterate through all connections and send to WebSocket clients
    for (struct mg_connection *c = mgr.conns; c != NULL; c = c->next) {
        if (c->is_websocket) {
            mg_ws_send(c, ui_state, strlen(ui_state), WEBSOCKET_OP_TEXT);
        }
    }
}

// Initialize TLS with certificates
static bool init_tls(void) {
    // Check if certificate files exist
    FILE *cert_file = fopen(TLS_CERT_FILE, "r");
    FILE *key_file = fopen(TLS_KEY_FILE, "r");
    
    if (!cert_file || !key_file) {
        if (cert_file) fclose(cert_file);
        if (key_file) fclose(key_file);
        printf("TLS certificates not found. Creating self-signed certificates...\n");
        return create_self_signed_certificates();
    }
    
    fclose(cert_file);
    fclose(key_file);
    
    // Load certificate and key
    tls_opts.cert = mg_file_read(&mg_fs_posix, TLS_CERT_FILE);
    tls_opts.key = mg_file_read(&mg_fs_posix, TLS_KEY_FILE);
    
    if (tls_opts.cert.len == 0 || tls_opts.key.len == 0) {
        printf("Failed to load TLS certificates\n");
        return false;
    }
    
    // Set additional TLS options for better compatibility
    tls_opts.ca = mg_str("");  // No CA required for self-signed
    
    printf("TLS certificates loaded successfully\n");
    return true;
}

// Create self-signed certificates for development
static bool create_self_signed_certificates(void) {
    printf("Creating self-signed RSA certificates for development...\n");
    
    // Create certs directory if it doesn't exist
    system("mkdir -p certs");
    
    // Generate RSA key and certificate for better compatibility with built-in TLS
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "openssl genrsa -out %s 2048 && "
        "openssl req -new -key %s -x509 -nodes -out %s -days 365 "
        "-subj '/C=US/ST=State/L=City/O=Organization/CN=localhost' "
        "-addext 'subjectAltName=DNS:localhost,IP:127.0.0.1' "
        "-addext 'keyUsage=digitalSignature,keyEncipherment' "
        "-addext 'extendedKeyUsage=serverAuth'",
        TLS_KEY_FILE, TLS_KEY_FILE, TLS_CERT_FILE);
    
    int result = system(cmd);
    if (result != 0) {
        printf("Failed to create self-signed RSA certificates\n");
        return false;
    }
    
    printf("Self-signed RSA certificates created successfully\n");
    
    // Load the newly created certificates
    tls_opts.cert = mg_file_read(&mg_fs_posix, TLS_CERT_FILE);
    tls_opts.key = mg_file_read(&mg_fs_posix, TLS_KEY_FILE);
    
    return (tls_opts.cert.len > 0 && tls_opts.key.len > 0);
}

// Cleanup TLS resources
static void cleanup_tls(void) {
    if (tls_opts.cert.len > 0) {
        free((void*)tls_opts.cert.buf);
        tls_opts.cert.buf = NULL;
        tls_opts.cert.len = 0;
    }
    if (tls_opts.key.len > 0) {
        free((void*)tls_opts.key.buf);
        tls_opts.key.buf = NULL;
        tls_opts.key.len = 0;
    }
    if (tls_opts.ca.len > 0) {
        free((void*)tls_opts.ca.buf);
        tls_opts.ca.buf = NULL;
        tls_opts.ca.len = 0;
    }
}


