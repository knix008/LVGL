#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Web server initialization
void web_server_init(void);

// Web server polling (call in main loop)
void web_server_poll(void);

// Web server cleanup
void web_server_cleanup(void);

// Get current UI state as JSON string
const char* web_server_get_ui_state_json(void);

// Set UI state from JSON string
void web_server_set_ui_state_from_json(const char* json);

// Get web server status
const char* web_server_get_status(void);

// Enable/disable web server
void web_server_set_enabled(bool enabled);
bool web_server_is_enabled(void);

// TLS-related functions
bool web_server_is_tls_enabled(void);
int web_server_get_tls_port(void);
const char* web_server_get_tls_status(void);

#ifdef __cplusplus
}
#endif

#endif // WEB_SERVER_H
