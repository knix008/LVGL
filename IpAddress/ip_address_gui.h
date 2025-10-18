/**
 * @file ip_address_gui.h
 * @brief IP Address Input GUI using LVGL
 * @details Supports both IPv4 and IPv6 input modes with validation
 */

#ifndef IP_ADDRESS_GUI_H
#define IP_ADDRESS_GUI_H

#include "lvgl.h"
#include "ip_address.h"

/* Window dimensions */
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640

/* UI Constants */
#define MARGIN 10
#define BUTTON_HEIGHT 40
#define TEXT_AREA_HEIGHT 50
#define LABEL_HEIGHT 30

/* IP Address Input Modes */
typedef enum {
    IP_MODE_IPV4 = 0,
    IP_MODE_IPV6 = 1
} ip_input_mode_t;

/* GUI State Structure */
typedef struct {
    lv_obj_t *main_screen;
    lv_obj_t *title_label;
    lv_obj_t *mode_switch;
    lv_obj_t *mode_label;
    lv_obj_t *ip_textarea;         /* Single text area for IP input */
    lv_obj_t *validate_btn;
    lv_obj_t *clear_btn;
    lv_obj_t *result_label;
    lv_obj_t *keyboard;
    
    ip_input_mode_t current_mode;
    bool is_valid;
} ip_gui_state_t;

/* Function Declarations */

/**
 * @brief Initialize the IP address GUI
 * @return Pointer to GUI state structure
 */
ip_gui_state_t* ip_gui_init(void);

/**
 * @brief Create the main UI components
 * @param state Pointer to GUI state
 */
void ip_gui_create_ui(ip_gui_state_t *state);

/**
 * @brief Switch between IPv4 and IPv6 input modes
 * @param state Pointer to GUI state
 * @param mode New input mode
 */
void ip_gui_switch_mode(ip_gui_state_t *state, ip_input_mode_t mode);

/**
 * @brief Validate IP address based on current mode
 * @param ip_str IP address string to validate
 * @param mode Input mode (IPv4 or IPv6)
 * @return true if valid, false otherwise
 */
bool ip_gui_validate_address(const char *ip_str, ip_input_mode_t mode);

/**
 * @brief Format IP address for display
 * @param ip_str Input IP string
 * @param mode Input mode
 * @param formatted Output buffer for formatted string
 * @param max_len Maximum length of output buffer
 */
void ip_gui_format_address(const char *ip_str, ip_input_mode_t mode, 
                          char *formatted, size_t max_len);

/**
 * @brief Clear the input field and result
 * @param state Pointer to GUI state
 */
void ip_gui_clear_input(ip_gui_state_t *state);

/**
 * @brief Handle keyboard events (Enter key for validation)
 * @param e LVGL event
 */
void ip_gui_keyboard_event_cb(lv_event_t *e);

/**
 * @brief Handle textarea focus events
 * @param e LVGL event
 */
void ip_gui_textarea_event_cb(lv_event_t *e);

/**
 * @brief Handle mode switch events
 * @param e LVGL event
 */
void ip_gui_mode_switch_event_cb(lv_event_t *e);

/**
 * @brief Handle validate button events
 * @param e LVGL event
 */
void ip_gui_validate_event_cb(lv_event_t *e);

/**
 * @brief Handle clear button events
 * @param e LVGL event
 */
void ip_gui_clear_event_cb(lv_event_t *e);

/**
 * @brief Update the result display
 * @param state Pointer to GUI state
 * @param is_valid Whether the IP is valid
 * @param message Result message to display
 */
void ip_gui_update_result(ip_gui_state_t *state, bool is_valid, const char *message);

/**
 * @brief Show validation result in a popup message box
 * @param state Pointer to GUI state
 * @param is_valid Whether the IP is valid
 * @param ip_address The IP address being validated
 */
void ip_gui_show_result_popup(ip_gui_state_t *state, bool is_valid, const char *ip_address);

/**
 * @brief Clean up GUI resources
 * @param state Pointer to GUI state
 */
void ip_gui_cleanup(ip_gui_state_t *state);

#endif /* IP_ADDRESS_GUI_H */
