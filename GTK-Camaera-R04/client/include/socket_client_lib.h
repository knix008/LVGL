#ifndef SOCKET_CLIENT_LIB_H
#define SOCKET_CLIENT_LIB_H

#include <string>
#include <map>

/**
 * @brief Socket client library for communicating with face recognition server
 *
 * Provides a C++ interface to send commands to the Unix domain socket
 * server and receive responses.
 */
class SocketClientLib {
public:
    /**
     * @brief Response from server
     */
    struct Response {
        bool success;           // true if OK:, false if ERROR:
        std::string message;    // Response message without OK:/ERROR: prefix
    };

    SocketClientLib(const std::string& socket_path = "/tmp/face_recognition.sock");
    ~SocketClientLib() = default;

    /**
     * @brief Send command to server
     * @param command Command name (e.g., "camera_on")
     * @param args Command arguments as key-value pairs
     * @return Response from server
     */
    Response send_command(const std::string& command,
                         const std::map<std::string, std::string>& args = {});

    /**
     * @brief Send raw command string
     * @param command_str Full command string (e.g., "capture:A:1")
     * @return Response from server
     */
    Response send_raw(const std::string& command_str);

    /**
     * @brief High-level API - Camera On
     */
    Response camera_on();

    /**
     * @brief High-level API - Camera Off
     */
    Response camera_off();

    /**
     * @brief High-level API - Capture Person
     * @param initial Person initial (A-Z)
     * @param id Person ID (1-9999)
     */
    Response capture(const std::string& initial, const std::string& id);

    /**
     * @brief High-level API - Start Training
     */
    Response registering();

    /**
     * @brief High-level API - Get Status
     */
    Response status();

    /**
     * @brief Start streaming recognition results
     * @return Response with initial status, keep socket open for streaming
     */
    int stream_recognition();

    /**
     * @brief Get server socket path
     */
    const std::string& get_socket_path() const { return socket_path; }

private:
    std::string socket_path;

    /**
     * @brief Connect to server and send command
     */
    Response execute(const std::string& command_str);
};

#endif // SOCKET_CLIENT_LIB_H
