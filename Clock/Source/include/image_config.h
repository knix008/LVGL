#ifndef IMAGE_CONFIG_H
#define IMAGE_CONFIG_H

// Image configuration settings
// You can modify these paths as needed for your deployment

// Image directory options (uncomment the one you want to use):
// #define IMAGE_DIR ""                           // Current directory (default)
#define IMAGE_DIR "A:../assets/"              // POSIX driver with absolute path
// #define IMAGE_DIR "../assets/"              // Relative to assets folder from source directory
// #define IMAGE_DIR "/usr/share/images/"      // System image directory
// #define IMAGE_DIR "./images/"               // Local images subdirectory
// #define IMAGE_DIR "/opt/korean-input/images/" // Custom installation directory

// Image file names
#define INTELLIVIX_LOGO "IntellvixSmall.png"

// Helper macro to construct full image path
#define IMAGE_PATH(filename) IMAGE_DIR filename

#endif // IMAGE_CONFIG_H 