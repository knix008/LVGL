/**
 * @brief Binary protocol socket client for testing face recognition server
 *
 * Usage:
 *   ./socket_client camera_on
 *   ./socket_client camera_off
 *   ./socket_client capture A 1
 *   ./socket_client train
 *   ./socket_client status
 *   ./socket_client list
 */

#include "protocol.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <arpa/inet.h>

using namespace Protocol;

const char* SOCKET_PATH = "/tmp/face_recognition.sock";

// Helper function to send message and receive response
Message send_and_receive(const Message& request) {
    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    // Connect to server
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        throw std::runtime_error("Failed to connect to server. Make sure the application is running.");
    }

    // Send request
    std::vector<uint8_t> request_data = request.serialize();
    if (write(sock, request_data.data(), request_data.size()) < 0) {
        close(sock);
        throw std::runtime_error("Failed to send request");
    }

    // Read response header
    uint8_t header_buf[HEADER_SIZE];
    ssize_t bytes_read = read(sock, header_buf, HEADER_SIZE);
    if (bytes_read != HEADER_SIZE) {
        close(sock);
        throw std::runtime_error("Failed to read response header");
    }

    // Parse header to get payload length
    std::vector<uint8_t> response_data(header_buf, header_buf + HEADER_SIZE);
    
    // Extract length (bytes 6-9 in network order)
    uint32_t length_net;
    std::memcpy(&length_net, header_buf + 6, sizeof(length_net));
    uint32_t payload_length = ntohl(length_net);

    // Read payload if present
    if (payload_length > 0) {
        std::vector<uint8_t> payload_buf(payload_length);
        bytes_read = read(sock, payload_buf.data(), payload_length);
        if (bytes_read != static_cast<ssize_t>(payload_length)) {
            close(sock);
            throw std::runtime_error("Failed to read response payload");
        }
        response_data.insert(response_data.end(), payload_buf.begin(), payload_buf.end());
    }

    close(sock);
    
    return Message::deserialize(response_data);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args]\n"
                  << "Commands:\n"
                  << "  camera_on              - Start camera\n"
                  << "  camera_off             - Stop camera\n"
                  << "  capture <name> <id>    - Capture and register person (e.g., capture Alice 1)\n"
                  << "  train                  - Train recognition model\n"
                  << "  status                 - Get application status\n"
                  << "  list                   - List registered persons\n"
                  << "  delete <name>          - Delete person\n";
        return 1;
    }

    try {
        std::string command = argv[1];
        Message response;

        if (command == "camera_on") {
            CameraControlMessage msg(true);
            response = send_and_receive(msg);
            
        } else if (command == "camera_off") {
            CameraControlMessage msg(false);
            response = send_and_receive(msg);
            
        } else if (command == "capture") {
            if (argc < 4) {
                std::cerr << "Error: capture requires <name> and <id>\n";
                std::cerr << "Usage: " << argv[0] << " capture <name> <id>\n";
                return 1;
            }
            std::string name = argv[2];
            uint32_t id = std::stoul(argv[3]);
            CaptureMessage msg(name, id);
            response = send_and_receive(msg);
            
        } else if (command == "train" || command == "registering") {
            TrainMessage msg;
            response = send_and_receive(msg);
            
        } else if (command == "status") {
            StatusRequestMessage msg;
            response = send_and_receive(msg);
            
        } else if (command == "list") {
            ListPersonsMessage msg;
            response = send_and_receive(msg);
            
        } else if (command == "delete") {
            if (argc < 3) {
                std::cerr << "Error: delete requires <name>\n";
                std::cerr << "Usage: " << argv[0] << " delete <name>\n";
                return 1;
            }
            std::string name = argv[2];
            DeletePersonMessage msg(name);
            response = send_and_receive(msg);
            
        } else {
            std::cerr << "Error: Unknown command '" << command << "'\n";
            return 1;
        }

        // Process response
        MessageType resp_type = response.header.get_type();
        
        if (resp_type == MessageType::RESP_SUCCESS) {
            SuccessResponse success = SuccessResponse::from_message(response);
            std::cout << "✓ Success: " << success.message << std::endl;
            return 0;
            
        } else if (resp_type == MessageType::RESP_ERROR) {
            ErrorResponse error = ErrorResponse::from_message(response);
            std::cerr << "✗ Error " << error.error_code << ": " 
                      << error.error_message << std::endl;
            return 1;
            
        } else if (resp_type == MessageType::RESP_STATUS) {
            StatusResponse status = StatusResponse::from_message(response);
            std::cout << "=== Server Status ===" << std::endl;
            std::cout << "Camera Running:        " << (status.camera_running ? "Yes" : "No") << std::endl;
            std::cout << "Recognition Enabled:   " << (status.recognition_enabled ? "Yes" : "No") << std::endl;
            std::cout << "Training In Progress:  " << (status.training_in_progress ? "Yes" : "No") << std::endl;
            std::cout << "People Count:          " << status.people_count << std::endl;
            std::cout << "Total Faces:           " << status.total_faces << std::endl;
            std::cout << "FPS:                   " << status.fps << std::endl;
            return 0;
            
        } else if (resp_type == MessageType::RESP_PERSON_LIST) {
            PersonListResponse person_list = PersonListResponse::from_message(response);
            std::cout << "=== Registered Persons (" << person_list.persons.size() << ") ===" << std::endl;
            for (const auto& person : person_list.persons) {
                std::cout << "  - " << person.name 
                          << " (ID: " << person.id
                          << ", Images: " << person.image_count
                          << ", Created: " << person.created_timestamp << ")" << std::endl;
            }
            return 0;
            
        } else {
            std::cerr << "Unexpected response type: " 
                      << get_message_type_name(resp_type) << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
