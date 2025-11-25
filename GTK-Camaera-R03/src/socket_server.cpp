#include "socket_server.h"
#include "logger.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <filesystem>
#include <sstream>
#include <algorithm>

SocketServer::SocketServer(const std::string& socket_path)
    : socket_path(socket_path), server_socket(-1), running(false) {}

SocketServer::~SocketServer() {
    stop();
    // Detach thread if it's still running to avoid terminate() exception
    if (server_thread && server_thread->joinable()) {
        server_thread->detach();
    }
}

bool SocketServer::start() {
    if (running) {
        LOG_WARN("Socket server already running");
        return false;
    }

    // Remove existing socket file if it exists
    if (std::filesystem::exists(socket_path)) {
        try {
            std::filesystem::remove(socket_path);
            LOG_INFO("Removed existing socket file: " << socket_path);
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to remove socket file: " << e.what());
            return false;
        }
    }

    // Create Unix domain socket
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket < 0) {
        LOG_ERROR("Failed to create socket");
        return false;
    }

    // Set socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR("Failed to bind socket: " << strerror(errno));
        close(server_socket);
        server_socket = -1;
        return false;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0) {
        LOG_ERROR("Failed to listen on socket");
        close(server_socket);
        server_socket = -1;
        return false;
    }

    running = true;
    server_thread = std::make_unique<std::thread>(&SocketServer::server_loop, this);
    LOG_INFO("Socket server started on: " << socket_path);
    return true;
}

void SocketServer::stop() {
    if (!running) {
        return;
    }

    running = false;
    LOG_INFO("Socket server stopping...");

    // Close socket first to unblock accept() call
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }

    // Try to wait for server thread to finish with a timeout
    if (server_thread && server_thread->joinable()) {
        // Give the thread 1 second to finish gracefully
        // In most cases, closing the socket will wake up accept() immediately
        for (int i = 0; i < 10; ++i) {
            if (!server_thread->joinable()) {
                break;  // Thread finished
            }
            usleep(100000);  // 100ms
        }

        if (server_thread->joinable()) {
            // Thread is still running, but we can't wait forever
            // The destructor will attempt to join again
            LOG_WARN("Socket server thread did not finish promptly - continuing anyway");
        } else {
            server_thread->join();
        }
    }

    // Clean up socket file immediately
    try {
        if (std::filesystem::exists(socket_path)) {
            std::filesystem::remove(socket_path);
            LOG_INFO("Socket file removed: " << socket_path);
        }
    } catch (const std::exception& e) {
        LOG_WARN("Failed to clean up socket file: " << e.what());
    }

    LOG_INFO("Socket server stopped");
}

void SocketServer::register_command(const std::string& command, CommandCallback callback) {
    command_handlers[command] = callback;
    LOG_INFO("Registered command: " << command);
}

void SocketServer::server_loop() {
    LOG_INFO("Socket server loop started");

    while (running) {
        struct sockaddr_un addr;
        socklen_t addr_len = sizeof(addr);

        // Set socket to non-blocking mode with timeout
        // This allows us to check running flag periodically
        int client_fd = accept(server_socket, (struct sockaddr*)&addr, &addr_len);
        if (client_fd < 0) {
            if (running) {
                LOG_ERROR("Accept failed: " << strerror(errno));
            } else {
                LOG_INFO("Accept failed due to shutdown (exiting loop)");
                break;  // Exit loop explicitly when shutting down
            }
            continue;
        }

        // Handle client in separate scope to ensure cleanup
        handle_client(client_fd);
    }

    LOG_INFO("Socket server loop ended");
}

void SocketServer::handle_client(int client_fd) {
    try {
        // Read command from client
        char buffer[1024] = {0};
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

        if (bytes_read < 0) {
            LOG_ERROR("Failed to read from client");
            close(client_fd);
            return;
        }

        if (bytes_read == 0) {
            close(client_fd);
            return;
        }

        std::string command_str(buffer, bytes_read);
        LOG_INFO("Received command: " << command_str);

        // Execute command
        std::string response = execute_command(command_str);

        // Send response back to client
        if (write(client_fd, response.c_str(), response.length()) < 0) {
            LOG_ERROR("Failed to write response to client");
        }

        close(client_fd);

    } catch (const std::exception& e) {
        LOG_ERROR("Exception handling client: " << e.what());
        close(client_fd);
    }
}

std::string SocketServer::execute_command(const std::string& command_str) {
    // Parse command: "command_name:arg1:arg2:..."
    std::istringstream iss(command_str);
    std::string command;
    std::getline(iss, command, ':');

    // Trim whitespace
    command.erase(0, command.find_first_not_of(" \t\n\r"));
    command.erase(command.find_last_not_of(" \t\n\r") + 1);

    // Convert to lowercase for comparison
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // Look up command handler
    auto it = command_handlers.find(command);
    if (it == command_handlers.end()) {
        LOG_WARN("Unknown command: " << command);
        return "ERROR:Unknown command: " + command;
    }

    try {
        // Get remaining arguments
        std::string args = command_str.substr(command.length());
        if (!args.empty() && args[0] == ':') {
            args = args.substr(1);  // Remove leading colon
        }

        // Execute command handler
        std::string response = it->second(args);
        LOG_INFO("Command executed successfully: " << command);
        return response;

    } catch (const std::exception& e) {
        LOG_ERROR("Error executing command " << command << ": " << e.what());
        return "ERROR:" + std::string(e.what());
    }
}
