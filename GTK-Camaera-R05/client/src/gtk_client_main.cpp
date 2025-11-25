#include "../include/gtk_client.h"
#include <iostream>

int main(int /* argc */, char* /* argv */[]) {
    try {
        GTKClient client;

        if (!client.init()) {
            std::cerr << "Failed to initialize GTK client\n";
            return 1;
        }

        client.run();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}
