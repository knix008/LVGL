#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "ui_components.h"
#include "sqlite_test.h"
#include "lv_freetype.h"

// Function to find input device by type
const char* find_input_device(const char* type) {
    DIR *dir;
    struct dirent *ent;
    char path[256];
    char name[256];
    FILE *fp;
    
    dir = opendir("/dev/input");
    if (dir == NULL) {
        return NULL;
    }
    
    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, "event", 5) == 0) {
            snprintf(path, sizeof(path), "/dev/input/%s", ent->d_name);
            snprintf(name, sizeof(name), "/sys/class/input/%s/name", ent->d_name);
            
            fp = fopen(name, "r");
            if (fp) {
                if (fgets(name, sizeof(name), fp)) {
                    // Remove newline
                    name[strcspn(name, "\n")] = 0;
                    fclose(fp);
                    
                    // Check if this device matches the type we're looking for
                    if (strstr(name, type) != NULL) {
                        closedir(dir);
                        return strdup(path);
                    }
                } else {
                    fclose(fp);
                }
            }
        }
    }
    
    closedir(dir);
    return NULL;
}

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(1024, 768);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "LVGL SQLCipher Demo - DRM Mode");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // Initialize FreeType
    printf("Initializing FreeType...\n");
    lv_freetype_init(256);
    printf("FreeType initialized successfully.\n");

    // Run SQLCipher demonstration
    printf("Running SQLCipher demonstration...\n");
    sqlite_demo();

    // Initialize UI with tab menu
    lv_example_tab_menu();

    printf("Tab menu GUI created successfully. DRM display should appear now.\n");
    printf("Press Ctrl+C to exit.\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}