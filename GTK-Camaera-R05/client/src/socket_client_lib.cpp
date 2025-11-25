#include "../include/socket_client_lib.h"
#include "protocol.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <arpa/inet.h>

using namespace Protocol;

SocketClientLib::SocketClientLib(const std::string& socket_path)
    : socket_path(socket_path) {}

SocketClientLib::Response SocketClientLib::send_command(
    const std::string& command,
    const std::map<std::string, std::string>& args) {

    Response response;
    response.success = false;
    response.message = "Unknown command";

    try {
        Message request;
        
        if (command == "camera_on") {
            CameraControlMessage msg(true);
            request = msg;
        } else if (command == "camera_off") {
            CameraControlMessage msg(false);
            request = msg;
        } else if (command == "capture") {
            auto it_initial = args.find("initial");
            auto it_id = args.find("id");
            if (it_initial != args.end() && it_id != args.end()) {
                CaptureMessage msg(it_initial->second, std::stoul(it_id->second));
                request = msg;
            } else {
                response.message = "Missing arguments: initial and id required";
                return response;
            }
        } else if (command == "train" || command == "registering") {
            TrainMessage msg;
            request = msg;
        } else if (command == "status") {
            StatusRequestMessage msg;
            request = msg;
        } else if (command == "list") {
            ListPersonsMessage msg;
            request = msg;
        } else if (command == "delete") {
            auto it_name = args.find("name");
            if (it_name != args.end()) {
                DeletePersonMessage msg(it_name->second);
                request = msg;
            } else {
                response.message = "Missing argument: name required";
                return response;
            }
        } else {
            response.message = "Unknown command: " + command;
            return response;
        }

        return execute_binary(request);
        
    } catch (const std::exception& e) {
        response.message = std::string("Exception: ") + e.what();
        return response;
    }
}

SocketClientLib::Response SocketClientLib::send_raw(const std::string& command_str) {
    // Legacy text protocol support - parse and convert to binary
    Response response;
    response.success = false;
    
    try {
        // Parse text command and convert to binary message
        size_t pos = command_str.find(':');
        std::string cmd = (pos != std::string::npos) ? command_str.substr(0, pos) : command_str;
        
        if (cmd == "camera_on") {
            CameraControlMessage msg(true);
            return execute_binary(msg);
        } else if (cmd == "camera_off") {
            CameraControlMessage msg(false);
            return execute_binary(msg);
        } else if (cmd == "capture") {
            // Parse capture:name:id format
            size_t pos1 = command_str.find(':', 0);
            size_t pos2 = command_str.find(':', pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string name = command_str.substr(pos1 + 1, pos2 - pos1 - 1);
                std::string id_str = command_str.substr(pos2 + 1);
                CaptureMessage msg(name, std::stoul(id_str));
                return execute_binary(msg);
            }
        } else if (cmd == "registering" || cmd == "train") {
            TrainMessage msg;
            return execute_binary(msg);
        } else if (cmd == "status") {
            StatusRequestMessage msg;
            return execute_binary(msg);
        }
        
        response.message = "Invalid command format: " + command_str;
        return response;
        
    } catch (const std::exception& e) {
        response.message = std::string("Exception: ") + e.what();
        return response;
    }
}

SocketClientLib::Response SocketClientLib::camera_on() {
    CameraControlMessage msg(true);
    return execute_binary(msg);
}

SocketClientLib::Response SocketClientLib::camera_off() {
    CameraControlMessage msg(false);
    return execute_binary(msg);
}

SocketClientLib::Response SocketClientLib::capture(const std::string& initial,
                                                   const std::string& id) {
    try {
        CaptureMessage msg(initial, std::stoul(id));
        return execute_binary(msg);
    } catch (const std::exception& e) {
        Response response;
        response.success = false;
        response.message = std::string("Invalid ID: ") + e.what();
        return response;
    }
}

SocketClientLib::Response SocketClientLib::registering() {
    TrainMessage msg;
    return execute_binary(msg);
}

SocketClientLib::Response SocketClientLib::status() {
    StatusRequestMessage msg;
    return execute_binary(msg);
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

    // Send binary stream start message
    try {
        StreamControlMessage msg(true);
        std::vector<uint8_t> data = msg.serialize();
        if (write(sock, data.data(), data.size()) < 0) {
            close(sock);
            return -1;
        }
    } catch (...) {
        close(sock);
        return -1;
    }

    // Return socket for reading stream (caller is responsible for closing)
    return sock;
}

SocketClientLib::Response SocketClientLib::execute_binary(const Message& request) {
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

    try {
        // Serialize and send request
        std::vector<uint8_t> request_data = request.serialize();
        if (write(sock, request_data.data(), request_data.size()) < 0) {
            response.message = "Failed to send request";
            close(sock);
            return response;
        }

        // Read response header
        uint8_t header_buf[HEADER_SIZE];
        ssize_t bytes_read = read(sock, header_buf, HEADER_SIZE);
        if (bytes_read != HEADER_SIZE) {
            response.message = "Failed to read response header";
            close(sock);
            return response;
        }

        // Parse header to get payload length
        std::vector<uint8_t> response_data(header_buf, header_buf + HEADER_SIZE);
        
        uint32_t length_net;
        std::memcpy(&length_net, header_buf + 6, sizeof(length_net));
        uint32_t payload_length = ntohl(length_net);

        // Read payload if present
        if (payload_length > 0) {
            std::vector<uint8_t> payload_buf(payload_length);
            bytes_read = read(sock, payload_buf.data(), payload_length);
            if (bytes_read != static_cast<ssize_t>(payload_length)) {
                response.message = "Failed to read response payload";
                close(sock);
                return response;
            }
            response_data.insert(response_data.end(), payload_buf.begin(), payload_buf.end());
        }

        close(sock);

        // Deserialize response
        Message resp_msg = Message::deserialize(response_data);
        MessageType resp_type = resp_msg.header.get_type();

        // Process response based on type
        if (resp_type == MessageType::RESP_SUCCESS) {
            SuccessResponse success = SuccessResponse::from_message(resp_msg);
            response.success = true;
            response.message = success.message;
            
        } else if (resp_type == MessageType::RESP_ERROR) {
            ErrorResponse error = ErrorResponse::from_message(resp_msg);
            response.success = false;
            response.message = "Error " + std::to_string(error.error_code) + ": " + error.error_message;
            
        } else if (resp_type == MessageType::RESP_STATUS) {
            StatusResponse status = StatusResponse::from_message(resp_msg);
            std::ostringstream oss;
            oss << "camera_running:" << (status.camera_running ? "true" : "false")
                << ",recognition_enabled:" << (status.recognition_enabled ? "true" : "false")
                << ",training_in_progress:" << (status.training_in_progress ? "true" : "false")
                << ",people_count:" << status.people_count
                << ",total_faces:" << status.total_faces
                << ",fps:" << status.fps;
            response.success = true;
            response.message = oss.str();
            
        } else if (resp_type == MessageType::RESP_PERSON_LIST) {
            PersonListResponse person_list = PersonListResponse::from_message(resp_msg);
            std::ostringstream oss;
            oss << "count:" << person_list.persons.size();
            for (const auto& person : person_list.persons) {
                oss << ",person:" << person.name << ":" << person.id 
                    << ":" << person.image_count << ":" << person.created_timestamp;
            }
            response.success = true;
            response.message = oss.str();
            
        } else {
            response.success = false;
            response.message = "Unexpected response type";
        }

    } catch (const std::exception& e) {
        response.success = false;
        response.message = std::string("Exception: ") + e.what();
        close(sock);
    }

    return response;
}
