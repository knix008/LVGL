#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>
#include <map>

/**
 * @brief Unix domain socket server for remote command control
 *
 * Provides a socket interface for controlling the application via commands:
 * - camera_on: Start camera
 * - camera_off: Stop camera
 * - capture: Capture and register new person
 * - registering: Train recognition model
 * - status: Get application status
 */
class SocketServer {
public:
    // Command callback types
    using CommandCallback = std::function<std::string(const std::string&)>;

    SocketServer(const std::string& socket_path = "/tmp/face_recognition.sock");
    ~SocketServer();

    /**
     * @brief Start socket server
     * @return true if server started successfully
     */
    bool start();

    /**
     * @brief Stop socket server
     */
    void stop();

    /**
     * @brief Check if server is running
     */
    bool is_running() const { return running; }

    /**
     * @brief Register a command handler
     * @param command Command name (e.g., "camera_on")
     * @param callback Function to execute when command is received
     *                  Takes command arguments and returns response
     */
    void register_command(const std::string& command, CommandCallback callback);

    /**
     * @brief Get server socket path
     */
    const std::string& get_socket_path() const { return socket_path; }

private:
    /**
     * @brief Main server loop (runs in separate thread)
     */
    void server_loop();

    /**
     * @brief Handle incoming client connection
     */
    void handle_client(int client_fd);

    /**
     * @brief Parse and execute command
     * @return Response message to send to client
     */
    std::string execute_command(const std::string& command_str);

    std::string socket_path;
    int server_socket;
    std::atomic<bool> running;
    std::unique_ptr<std::thread> server_thread;
    std::map<std::string, CommandCallback> command_handlers;
};

#endif // SOCKET_SERVER_H
