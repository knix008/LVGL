#include "../include/socket_client_lib.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

SocketClientLib::SocketClientLib(const std::string& socket_path)
    : socket_path(socket_path) {}

SocketClientLib::Response SocketClientLib::send_command(
    const std::string& command,
    const std::map<std::string, std::string>& args) {

    std::string command_str = command;

    // Append arguments if provided
    if (!args.empty()) {
        for (const auto& [key, value] : args) {
            command_str += ":" + value;
        }
    }

    return execute(command_str);
}

SocketClientLib::Response SocketClientLib::send_raw(const std::string& command_str) {
    return execute(command_str);
}

SocketClientLib::Response SocketClientLib::camera_on() {
    return execute("camera_on");
}

SocketClientLib::Response SocketClientLib::camera_off() {
    return execute("camera_off");
}

SocketClientLib::Response SocketClientLib::capture(const std::string& initial,
                                                   const std::string& id) {
    std::string command = "capture:" + initial + ":" + id;
    return execute(command);
}

SocketClientLib::Response SocketClientLib::registering() {
    return execute("registering");
}

SocketClientLib::Response SocketClientLib::status() {
    return execute("status");
}

int SocketClientLib::stream_recognition() {
    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    // Connect to server
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    // Send stream_recognition command
    const char* command = "stream_recognition";
    if (write(sock, command, strlen(command)) < 0) {
        close(sock);
        return -1;
    }

    // Return socket for reading stream (caller is responsible for closing)
    return sock;
}

SocketClientLib::Response SocketClientLib::execute(const std::string& command_str) {
    Response response;
    response.success = false;
    response.message = "";

    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        response.message = "Failed to create socket";
        return response;
    }

    // Connect to server
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        response.message = "Server not running";
        close(sock);
        return response;
    }

    // Send command
    if (write(sock, command_str.c_str(), command_str.length()) < 0) {
        response.message = "Failed to send command";
        close(sock);
        return response;
    }

    // Read response
    char buffer[4096] = {0};
    ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    close(sock);

    if (bytes_read < 0) {
        response.message = "Failed to read response";
        return response;
    }

    std::string response_str(buffer, bytes_read);

    // Parse response
    if (response_str.substr(0, 3) == "OK:") {
        response.success = true;
        response.message = response_str.substr(3);
    } else if (response_str.substr(0, 6) == "ERROR:") {
        response.success = false;
        response.message = response_str.substr(6);
    } else {
        response.success = false;
        response.message = response_str;
    }

    return response;
}
