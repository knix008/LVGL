/**
 * Device Simulator with TLS 1.3 Support
 * Simulates a device that accepts firmware updates via HTTPS API
 *
 * Endpoints:
 *   POST /api/v1.0/updatefirmware - Firmware update endpoint
 *   GET  /api/v1.0/status          - Device status
 *   GET  /api/v1.0/health          - Health check
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <json/json.h>

class DeviceSimulator {
private:
    static constexpr size_t MAX_FIRMWARE_SIZE = 2ULL * 1024 * 1024 * 1024; // 2GB

    std::string device_id;
    std::string current_firmware_version;
    std::string status;
    std::string last_update;
    std::vector<Json::Value> update_history;

    int port;
    SSL_CTX* ssl_ctx;
    int server_socket;

    std::string get_timestamp() {
        time_t now = time(nullptr);
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", localtime(&now));
        return std::string(buf);
    }

    void log(const std::string& level, const std::string& message) {
        std::cout << "[" << get_timestamp() << "] " << level << " - " << message << std::endl;
    }

    void show_progress_bar(size_t current, size_t total, int bar_width = 50) {
        float progress = (float)current / (float)total;
        int pos = bar_width * progress;

        std::cout << "\r[" << get_timestamp() << "] PROGRESS - [";
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << "% ";
        std::cout << "(" << current / (1024 * 1024) << " MB / "
                  << total / (1024 * 1024) << " MB)";
        std::cout << std::flush;

        // Print newline when complete
        if (current >= total) {
            std::cout << std::endl;
        }
    }

    std::string save_firmware_file(const std::string& data, const std::string& version) {
        // Create received_firmwares directory if it doesn't exist
        const std::string dir = "received_firmwares";
        struct stat st;
        if (stat(dir.c_str(), &st) == -1) {
            mkdir(dir.c_str(), 0755);
        }

        // Create filename with timestamp and version
        std::string timestamp = get_timestamp();
        // Replace colons in timestamp for filename compatibility
        std::replace(timestamp.begin(), timestamp.end(), ':', '-');

        std::ostringstream filename;
        filename << dir << "/firmware_" << version << "_" << timestamp << ".bin";

        // Save file
        std::ofstream file(filename.str(), std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to create firmware file: " + filename.str());
        }

        file.write(data.c_str(), data.length());
        file.close();

        log("INFO", "Firmware saved: " + filename.str() + " (" +
            std::to_string(data.length()) + " bytes)");

        return filename.str();
    }

    SSL_CTX* create_ssl_context() {
        const SSL_METHOD* method = TLS_server_method();
        SSL_CTX* ctx = SSL_CTX_new(method);

        if (!ctx) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Unable to create SSL context");
        }

        // Set TLS 1.3 only
        SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
        SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

        return ctx;
    }

    void configure_ssl_context(const std::string& cert_file, const std::string& key_file) {
        // Load certificate
        if (SSL_CTX_use_certificate_file(ssl_ctx, cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to load certificate file");
        }

        // Load private key
        if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to load private key file");
        }

        // Verify private key
        if (!SSL_CTX_check_private_key(ssl_ctx)) {
            throw std::runtime_error("Private key does not match certificate");
        }

        log("INFO", "SSL context configured with TLS 1.3");
    }

    int create_server_socket(int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            close(sock);
            throw std::runtime_error("Failed to set socket options");
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            throw std::runtime_error("Failed to bind socket");
        }

        if (listen(sock, 10) < 0) {
            close(sock);
            throw std::runtime_error("Failed to listen on socket");
        }

        return sock;
    }

    std::string parse_http_request(const std::string& request, std::string& method,
                                   std::string& path, std::map<std::string, std::string>& headers) {
        std::istringstream stream(request);
        std::string line;

        // Parse request line
        if (std::getline(stream, line)) {
            std::istringstream request_line(line);
            request_line >> method >> path;
        }

        // Parse headers
        while (std::getline(stream, line) && line != "\r") {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);
                // Trim whitespace
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                headers[key] = value;
            }
        }

        // Get body
        std::string body;
        std::getline(stream, body, '\0');
        return body;
    }

    std::string create_json_response(int status_code, const std::string& status_text,
                                    const Json::Value& json_body) {
        Json::StreamWriterBuilder writer;
        std::string json_str = Json::writeString(writer, json_body);

        std::ostringstream response;
        response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Content-Length: " << json_str.length() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << json_str;

        return response.str();
    }

    std::string handle_update_firmware(const std::string& body,
                                      const std::map<std::string, std::string>& /* headers */) {
        log("INFO", "Processing firmware update request");

        try {
            Json::CharReaderBuilder reader;
            Json::Value request_data;
            std::string errs;

            std::istringstream body_stream(body);
            if (!Json::parseFromStream(reader, body_stream, &request_data, &errs)) {
                Json::Value error_response;
                error_response["status"] = "error";
                error_response["message"] = "Invalid JSON: " + errs;
                return create_json_response(400, "Bad Request", error_response);
            }

            // Check required fields
            if (!request_data.isMember("version") || !request_data.isMember("firmware_url")) {
                Json::Value error_response;
                error_response["status"] = "error";
                error_response["message"] = "Missing required fields: version, firmware_url";
                return create_json_response(400, "Bad Request", error_response);
            }

            std::string new_version = request_data["version"].asString();
            std::string firmware_url = request_data["firmware_url"].asString();

            log("INFO", "Firmware update: version=" + new_version + ", url=" + firmware_url);

            // Save firmware data if present in the request body
            std::string saved_file;
            if (body.length() > 1024) { // If body is larger than 1KB, save it
                try {
                    saved_file = save_firmware_file(body, new_version);
                } catch (const std::exception& e) {
                    log("WARNING", "Failed to save firmware file: " + std::string(e.what()));
                }
            }

            // Create update record
            Json::Value update_record;
            update_record["timestamp"] = get_timestamp();
            update_record["firmware_url"] = firmware_url;
            update_record["version"] = new_version;
            update_record["previous_version"] = current_firmware_version;
            if (!saved_file.empty()) {
                update_record["saved_file"] = saved_file;
            }

            // Update device state
            std::string previous_version = current_firmware_version;
            current_firmware_version = new_version;
            last_update = get_timestamp();
            update_history.push_back(update_record);

            // Create success response
            Json::Value response;
            response["status"] = "success";
            response["message"] = "Firmware update completed successfully";
            response["device_id"] = device_id;
            response["previous_version"] = previous_version;
            response["new_version"] = new_version;
            response["update_timestamp"] = last_update;

            log("INFO", "Firmware update successful: " + previous_version + " -> " + new_version);

            return create_json_response(200, "OK", response);

        } catch (const std::exception& e) {
            log("ERROR", std::string("Error processing firmware update: ") + e.what());
            status = "error";

            Json::Value error_response;
            error_response["status"] = "error";
            error_response["message"] = std::string("Firmware update failed: ") + e.what();
            return create_json_response(500, "Internal Server Error", error_response);
        }
    }

    std::string handle_get_status() {
        Json::Value response;
        response["device_id"] = device_id;
        response["current_firmware_version"] = current_firmware_version;
        response["status"] = status;
        response["last_update"] = last_update;

        Json::Value history(Json::arrayValue);
        for (const auto& record : update_history) {
            history.append(record);
        }
        response["update_history"] = history;

        return create_json_response(200, "OK", response);
    }

    std::string handle_health_check() {
        Json::Value response;
        response["status"] = "healthy";
        response["device_id"] = device_id;
        response["timestamp"] = get_timestamp();

        return create_json_response(200, "OK", response);
    }

    std::string handle_request(const std::string& method, const std::string& path,
                              const std::string& body, const std::map<std::string, std::string>& headers) {
        log("INFO", method + " " + path);

        if (method == "POST" && path == "/api/v1.0/updatefirmware") {
            return handle_update_firmware(body, headers);
        } else if (method == "GET" && path == "/api/v1.0/status") {
            return handle_get_status();
        } else if (method == "GET" && path == "/api/v1.0/health") {
            return handle_health_check();
        } else {
            Json::Value error_response;
            error_response["status"] = "error";
            error_response["message"] = "Not found";
            return create_json_response(404, "Not Found", error_response);
        }
    }

    void handle_client(SSL* ssl) {
        const size_t BUFFER_SIZE = 8192;
        char buffer[BUFFER_SIZE];
        std::string request_data;
        size_t total_bytes = 0;

        // Read request headers first
        bool headers_complete = false;
        size_t content_length = 0;
        size_t headers_end_pos = 0;

        while (!headers_complete) {
            int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);

            if (bytes <= 0) {
                int err = SSL_get_error(ssl, bytes);
                log("ERROR", "SSL_read error: " + std::to_string(err));
                return;
            }

            buffer[bytes] = '\0';
            request_data.append(buffer, bytes);
            total_bytes += bytes;

            // Check if headers are complete (look for \r\n\r\n)
            headers_end_pos = request_data.find("\r\n\r\n");
            if (headers_end_pos != std::string::npos) {
                headers_complete = true;

                // Parse Content-Length from headers
                std::string headers_part = request_data.substr(0, headers_end_pos);
                size_t cl_pos = headers_part.find("Content-Length:");
                if (cl_pos != std::string::npos) {
                    size_t cl_start = cl_pos + 15; // Length of "Content-Length:"
                    size_t cl_end = headers_part.find("\r\n", cl_start);
                    std::string cl_str = headers_part.substr(cl_start, cl_end - cl_start);
                    // Trim whitespace
                    cl_str.erase(0, cl_str.find_first_not_of(" \t"));
                    cl_str.erase(cl_str.find_last_not_of(" \t\r\n") + 1);
                    content_length = std::stoull(cl_str);

                    // Check if content length exceeds maximum
                    if (content_length > MAX_FIRMWARE_SIZE) {
                        log("ERROR", "Firmware size exceeds maximum: " + std::to_string(content_length) +
                            " bytes (max: " + std::to_string(MAX_FIRMWARE_SIZE) + " bytes)");

                        Json::Value error_response;
                        error_response["status"] = "error";
                        error_response["message"] = "Firmware size exceeds maximum allowed size of 2GB";
                        error_response["max_size_bytes"] = static_cast<Json::UInt64>(MAX_FIRMWARE_SIZE);
                        error_response["requested_size_bytes"] = static_cast<Json::UInt64>(content_length);

                        std::string response = create_json_response(413, "Payload Too Large", error_response);
                        SSL_write(ssl, response.c_str(), response.length());
                        return;
                    }
                }
            }

            // Safety check to prevent reading too much header data
            if (total_bytes > 16384 && !headers_complete) {
                log("ERROR", "Headers too large or malformed");
                return;
            }
        }

        // Read the rest of the body if needed
        size_t body_start = headers_end_pos + 4;
        size_t body_received = request_data.length() - body_start;

        // Show initial progress for large files
        bool show_progress = content_length > 1024 * 1024; // Show for files > 1MB
        if (show_progress && body_received < content_length) {
            log("INFO", "Receiving firmware upload: " +
                std::to_string(content_length / (1024 * 1024)) + " MB");
        }

        if (content_length > 0 && body_received < content_length) {
            size_t remaining = content_length - body_received;
            size_t last_update = 0;

            while (remaining > 0) {
                size_t to_read = std::min(remaining, BUFFER_SIZE);
                int bytes = SSL_read(ssl, buffer, to_read);

                if (bytes <= 0) {
                    int err = SSL_get_error(ssl, bytes);
                    if (show_progress) std::cout << std::endl; // Clear progress bar
                    log("ERROR", "SSL_read error while reading body: " + std::to_string(err));
                    return;
                }

                request_data.append(buffer, bytes);
                remaining -= bytes;
                total_bytes += bytes;

                // Show progress bar for large files
                if (show_progress) {
                    size_t received = total_bytes - body_start;
                    // Update every 1MB or when complete
                    if (received - last_update >= 1024 * 1024 || remaining == 0) {
                        show_progress_bar(received, content_length);
                        last_update = received;
                    }
                }
            }
        }

        if (!show_progress) {
            log("INFO", "Received " + std::to_string(total_bytes) + " bytes total");
        }

        // Parse request
        std::string method, path, body;
        std::map<std::string, std::string> headers;
        body = parse_http_request(request_data, method, path, headers);

        // Handle request
        std::string response = handle_request(method, path, body, headers);

        // Send response
        SSL_write(ssl, response.c_str(), response.length());
    }

public:
    DeviceSimulator(const std::string& device_id, const std::string& version, int port)
        : device_id(device_id), current_firmware_version(version),
          status("idle"), port(port), ssl_ctx(nullptr), server_socket(-1) {
    }

    ~DeviceSimulator() {
        if (server_socket >= 0) {
            close(server_socket);
        }
        if (ssl_ctx) {
            SSL_CTX_free(ssl_ctx);
        }
    }

    void initialize(const std::string& cert_file, const std::string& key_file) {
        // Initialize OpenSSL
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        // Create and configure SSL context
        ssl_ctx = create_ssl_context();
        configure_ssl_context(cert_file, key_file);

        // Create server socket
        server_socket = create_server_socket(port);

        log("INFO", "============================================================");
        log("INFO", "Device Simulator Starting");
        log("INFO", "============================================================");
        log("INFO", "Device ID: " + device_id);
        log("INFO", "Current Firmware Version: " + current_firmware_version);
        log("INFO", "Listening on: https://0.0.0.0:" + std::to_string(port));
        log("INFO", "API Endpoint: https://0.0.0.0:" + std::to_string(port) + "/api/v1.0/updatefirmware");
        log("INFO", "Status Endpoint: https://0.0.0.0:" + std::to_string(port) + "/api/v1.0/status");
        log("INFO", "Health Check: https://0.0.0.0:" + std::to_string(port) + "/api/v1.0/health");
        log("INFO", "TLS Version: 1.3 only");
        log("INFO", "============================================================");
    }

    void run() {
        log("INFO", "Server ready to accept connections");

        while (true) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);

            int client_sock = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock < 0) {
                log("ERROR", "Failed to accept connection");
                continue;
            }

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            log("INFO", "Connection from " + std::string(client_ip));

            SSL* ssl = SSL_new(ssl_ctx);
            SSL_set_fd(ssl, client_sock);

            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                log("ERROR", "SSL handshake failed");
            } else {
                handle_client(ssl);
            }

            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(client_sock);
        }
    }
};

int main(int argc, char* argv[]) {
    std::string device_id = "DEVICE-SIM-001";
    std::string firmware_version = "1.0.0";
    int port = 8443;
    std::string cert_file = "certs/device.crt";
    std::string key_file = "certs/device.key";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--device-id" && i + 1 < argc) {
            device_id = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--cert" && i + 1 < argc) {
            cert_file = argv[++i];
        } else if (arg == "--key" && i + 1 < argc) {
            key_file = argv[++i];
        } else if (arg == "--version" && i + 1 < argc) {
            firmware_version = argv[++i];
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --device-id ID    Device ID (default: DEVICE-SIM-001)\n"
                      << "  --port PORT       Port to listen on (default: 8443)\n"
                      << "  --cert FILE       SSL certificate file (default: certs/device.crt)\n"
                      << "  --key FILE        SSL private key file (default: certs/device.key)\n"
                      << "  --version VER     Initial firmware version (default: 1.0.0)\n"
                      << "  --help            Show this help message\n";
            return 0;
        }
    }

    try {
        DeviceSimulator simulator(device_id, firmware_version, port);
        simulator.initialize(cert_file, key_file);
        simulator.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
