#include "gtk_app.h"
#include "logger.h"

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        GTKApp app;

        if (!app.init()) {
            LOG_ERROR("Failed to initialize GTK application");
            return 1;
        }

        LOG_INFO("GTK Webcam Viewer started successfully");
        app.run();
    } catch (const std::exception& e) {
        LOG_ERROR("Exception occurred: " << e.what());
        return 1;
    }

    return 0;
}
