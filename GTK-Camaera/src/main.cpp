#include "gtk_app.h"
#include <iostream>

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        GTKApp app;

        if (!app.init()) {
            std::cerr << "Failed to initialize GTK application" << std::endl;
            return 1;
        }

        std::cout << "GTK Webcam Viewer started successfully" << std::endl;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
