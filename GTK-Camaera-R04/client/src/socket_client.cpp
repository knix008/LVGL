/**
 * @brief Simple Unix domain socket client for testing face recognition server
 *
 * Usage:
 *   ./build/socket_client camera_on
 *   ./build/socket_client camera_off
 *   ./build/socket_client capture:A:1
 *   ./build/socket_client registering
 *   ./build/socket_client status
 */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

const char* SOCKET_PATH = "/tmp/face_recognition.sock";
const int BUFFER_SIZE = 4096;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args]\n"
                  << "Commands:\n"
                  << "  camera_on           - Start camera\n"
                  << "  camera_off          - Stop camera\n"
                  << "  capture:<initial>:<id>  - Capture and register person (e.g., capture:A:1)\n"
                  << "  registering         - Train recognition model\n"
                  << "  status              - Get application status\n";
        return 1;
    }

    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error: Failed to create socket\n";
        return 1;
    }

    // Connect to server
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error: Failed to connect to socket at " << SOCKET_PATH << "\n";
        std::cerr << "Make sure the application is running.\n";
        close(sock);
        return 1;
    }

    // Build command string from arguments
    std::string command;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) command += ":";
        command += argv[i];
    }

    // Send command to server
    if (write(sock, command.c_str(), command.length()) < 0) {
        std::cerr << "Error: Failed to send command\n";
        close(sock);
        return 1;
    }

    // Read response from server
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);

    close(sock);

    if (bytes_read < 0) {
        std::cerr << "Error: Failed to read response\n";
        return 1;
    }

    // Display response
    std::string response(buffer, bytes_read);
    std::cout << "Response: " << response << "\n";

    // Return 0 for success (OK:), 1 for error (ERROR:)
    if (response.substr(0, 3) == "OK:") {
        std::cout << response.substr(3) << "\n";
        return 0;
    } else if (response.substr(0, 6) == "ERROR:") {
        std::cerr << response.substr(6) << "\n";
        return 1;
    }

    return 0;
}
