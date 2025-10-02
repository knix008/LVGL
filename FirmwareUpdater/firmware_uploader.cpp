/**
 * Firmware Uploader with TLS 1.3 Support
 * Uploads firmware to device simulator via HTTPS API
 *
 * Usage:
 *   ./firmware_uploader --host <host> --port <port> --file <firmware_file> --version <version>
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <json/json.h>

class FirmwareUploader {
private:
    static constexpr size_t MAX_FIRMWARE_SIZE = 2ULL * 1024 * 1024 * 1024; // 2GB
    static constexpr size_t CHUNK_SIZE = 8192;

    std::string host;
    int port;
    bool verify_ssl;

    void log(const std::string& level, const std::string& message) {
        std::cout << "[" << level << "] " << message << std::endl;
    }

    SSL_CTX* create_ssl_context() {
        const SSL_METHOD* method = TLS_client_method();
        SSL_CTX* ctx = SSL_CTX_new(method);

        if (!ctx) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Unable to create SSL context");
        }

        // Set TLS 1.3 only
        SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
        SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

        if (!verify_ssl) {
            // Disable certificate verification for self-signed certs
            SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
        }

        return ctx;
    }

    int create_socket(const std::string& hostname, int port) {
        struct hostent* host_entry = gethostbyname(hostname.c_str());
        if (!host_entry) {
            throw std::runtime_error("Failed to resolve hostname: " + hostname);
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr = *((struct in_addr*)host_entry->h_addr);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock);
            throw std::runtime_error("Failed to connect to " + hostname + ":" + std::to_string(port));
        }

        return sock;
    }

    std::string read_file(const std::string& filepath, size_t& file_size) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        file_size = file.tellg();

        if (file_size > MAX_FIRMWARE_SIZE) {
            throw std::runtime_error("File size exceeds maximum allowed size of 2GB");
        }

        file.seekg(0, std::ios::beg);

        std::string content;
        content.resize(file_size);
        file.read(&content[0], file_size);

        return content;
    }

    std::string create_json_request(const std::string& version, const std::string& firmware_url) {
        Json::Value request;
        request["version"] = version;
        request["firmware_url"] = firmware_url;

        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, request);
    }

    std::string create_http_request(const std::string& path, const std::string& body,
                                   const std::string& content_type = "application/json") {
        std::ostringstream request;
        request << "POST " << path << " HTTP/1.1\r\n";
        request << "Host: " << host << ":" << port << "\r\n";
        request << "Content-Type: " << content_type << "\r\n";
        request << "Content-Length: " << body.length() << "\r\n";
        request << "Connection: close\r\n";
        request << "\r\n";
        request << body;

        return request.str();
    }

    void send_data(SSL* ssl, const std::string& data, const std::string& description) {
        size_t total_sent = 0;
        size_t remaining = data.length();
        const char* ptr = data.c_str();

        log("INFO", "Sending " + description + ": " + std::to_string(data.length()) + " bytes");

        while (remaining > 0) {
            size_t to_send = std::min(remaining, CHUNK_SIZE);
            int sent = SSL_write(ssl, ptr + total_sent, to_send);

            if (sent <= 0) {
                int err = SSL_get_error(ssl, sent);
                throw std::runtime_error("SSL_write failed: " + std::to_string(err));
            }

            total_sent += sent;
            remaining -= sent;

            // Log progress for large transfers
            if (data.length() > 1024 * 1024) { // Log for > 1MB
                size_t sent_mb = total_sent / (1024 * 1024);
                size_t total_mb = data.length() / (1024 * 1024);
                if (sent_mb % 10 == 0 && remaining > 0) { // Every 10MB
                    log("INFO", "Sent: " + std::to_string(sent_mb) + " MB / " +
                        std::to_string(total_mb) + " MB");
                }
            }
        }

        log("INFO", "Successfully sent " + std::to_string(total_sent) + " bytes");
    }

    std::string receive_response(SSL* ssl) {
        char buffer[CHUNK_SIZE];
        std::string response;
        int bytes;

        log("INFO", "Receiving response...");

        while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            response.append(buffer, bytes);
        }

        if (bytes < 0) {
            int err = SSL_get_error(ssl, bytes);
            if (err != SSL_ERROR_ZERO_RETURN) { // Ignore normal close
                log("WARNING", "SSL_read error: " + std::to_string(err));
            }
        }

        log("INFO", "Received " + std::to_string(response.length()) + " bytes");
        return response;
    }

    Json::Value parse_json_response(const std::string& http_response) {
        // Find the start of JSON body (after headers)
        size_t body_start = http_response.find("\r\n\r\n");
        if (body_start == std::string::npos) {
            throw std::runtime_error("Invalid HTTP response: no headers found");
        }

        body_start += 4; // Skip "\r\n\r\n"
        std::string body = http_response.substr(body_start);

        // Parse JSON
        Json::CharReaderBuilder reader;
        Json::Value json_response;
        std::string errs;
        std::istringstream body_stream(body);

        if (!Json::parseFromStream(reader, body_stream, &json_response, &errs)) {
            throw std::runtime_error("Failed to parse JSON response: " + errs);
        }

        return json_response;
    }

    void print_response(const Json::Value& response) {
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "  ";
        std::string output = Json::writeString(writer, response);
        std::cout << "\nServer Response:\n" << output << std::endl;
    }

public:
    FirmwareUploader(const std::string& host, int port, bool verify_ssl = false)
        : host(host), port(port), verify_ssl(verify_ssl) {
    }

    void upload_json(const std::string& version, const std::string& firmware_url) {
        log("INFO", "============================================================");
        log("INFO", "Firmware Upload - JSON Mode");
        log("INFO", "============================================================");
        log("INFO", "Target: https://" + host + ":" + std::to_string(port));
        log("INFO", "Version: " + version);
        log("INFO", "Firmware URL: " + firmware_url);
        log("INFO", "============================================================");

        // Initialize OpenSSL
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        SSL_CTX* ctx = nullptr;
        SSL* ssl = nullptr;
        int sock = -1;

        try {
            // Create SSL context
            ctx = create_ssl_context();

            // Create socket and connect
            log("INFO", "Connecting to " + host + ":" + std::to_string(port));
            sock = create_socket(host, port);
            log("INFO", "Connected");

            // Create SSL connection
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);

            log("INFO", "Performing TLS handshake...");
            if (SSL_connect(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("TLS handshake failed");
            }
            log("INFO", "TLS 1.3 handshake successful");

            // Create and send request
            std::string json_body = create_json_request(version, firmware_url);
            std::string http_request = create_http_request("/api/v1.0/updatefirmware", json_body);

            send_data(ssl, http_request, "HTTP request");

            // Receive response
            std::string http_response = receive_response(ssl);

            // Parse and display response
            Json::Value response = parse_json_response(http_response);
            print_response(response);

            // Check status
            if (response.isMember("status") && response["status"].asString() == "success") {
                log("INFO", "✓ Firmware update successful!");
            } else {
                log("ERROR", "✗ Firmware update failed!");
            }

        } catch (const std::exception& e) {
            log("ERROR", std::string("Error: ") + e.what());
            throw;
        }

        // Cleanup
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        if (sock >= 0) {
            close(sock);
        }
        if (ctx) {
            SSL_CTX_free(ctx);
        }

        log("INFO", "============================================================");
    }

    void upload_file(const std::string& filepath, const std::string& version) {
        log("INFO", "============================================================");
        log("INFO", "Firmware Upload - File Mode");
        log("INFO", "============================================================");
        log("INFO", "Target: https://" + host + ":" + std::to_string(port));
        log("INFO", "File: " + filepath);
        log("INFO", "Version: " + version);
        log("INFO", "============================================================");

        // Read file
        size_t file_size;
        log("INFO", "Reading file...");
        std::string file_content = read_file(filepath, file_size);

        double file_size_mb = file_size / (1024.0 * 1024.0);
        log("INFO", "File size: " + std::to_string(file_size) + " bytes (" +
            std::to_string(file_size_mb) + " MB)");

        // Create JSON body with file data
        std::string json_body = create_json_request(version, "file://" + filepath);

        // Initialize OpenSSL
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        SSL_CTX* ctx = nullptr;
        SSL* ssl = nullptr;
        int sock = -1;

        try {
            // Create SSL context
            ctx = create_ssl_context();

            // Create socket and connect
            log("INFO", "Connecting to " + host + ":" + std::to_string(port));
            sock = create_socket(host, port);
            log("INFO", "Connected");

            // Create SSL connection
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);

            log("INFO", "Performing TLS handshake...");
            if (SSL_connect(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("TLS handshake failed");
            }
            log("INFO", "TLS 1.3 handshake successful");

            // Create and send request
            std::string http_request = create_http_request("/api/v1.0/updatefirmware", json_body);

            send_data(ssl, http_request, "firmware data");

            // Receive response
            std::string http_response = receive_response(ssl);

            // Parse and display response
            Json::Value response = parse_json_response(http_response);
            print_response(response);

            // Check status
            if (response.isMember("status") && response["status"].asString() == "success") {
                log("INFO", "✓ Firmware update successful!");
            } else {
                log("ERROR", "✗ Firmware update failed!");
            }

        } catch (const std::exception& e) {
            log("ERROR", std::string("Error: ") + e.what());
            throw;
        }

        // Cleanup
        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        if (sock >= 0) {
            close(sock);
        }
        if (ctx) {
            SSL_CTX_free(ctx);
        }

        log("INFO", "============================================================");
    }

    void get_status() {
        log("INFO", "Getting device status...");

        // Initialize OpenSSL
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();

        SSL_CTX* ctx = nullptr;
        SSL* ssl = nullptr;
        int sock = -1;

        try {
            ctx = create_ssl_context();
            sock = create_socket(host, port);
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock);

            if (SSL_connect(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("TLS handshake failed");
            }

            // Create GET request
            std::ostringstream request;
            request << "GET /api/v1.0/status HTTP/1.1\r\n";
            request << "Host: " << host << ":" << port << "\r\n";
            request << "Connection: close\r\n";
            request << "\r\n";

            std::string http_request = request.str();
            SSL_write(ssl, http_request.c_str(), http_request.length());

            std::string http_response = receive_response(ssl);
            Json::Value response = parse_json_response(http_response);
            print_response(response);

        } catch (const std::exception& e) {
            log("ERROR", std::string("Error: ") + e.what());
            throw;
        }

        if (ssl) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
        if (sock >= 0) {
            close(sock);
        }
        if (ctx) {
            SSL_CTX_free(ctx);
        }
    }
};

void print_usage(const char* program_name) {
    std::cout << "Firmware Uploader - Upload firmware to device via HTTPS\n\n"
              << "Usage:\n"
              << "  " << program_name << " [options]\n\n"
              << "Options:\n"
              << "  --host HOST           Target host (default: localhost)\n"
              << "  --port PORT           Target port (default: 8443)\n"
              << "  --file FILE           Firmware file to upload\n"
              << "  --version VERSION     Firmware version\n"
              << "  --url URL             Firmware URL (alternative to --file)\n"
              << "  --status              Get device status\n"
              << "  --verify-ssl          Verify SSL certificate (default: false)\n"
              << "  --help                Show this help message\n\n"
              << "Examples:\n"
              << "  # Upload with firmware URL\n"
              << "  " << program_name << " --version 2.0.0 --url https://example.com/fw.bin\n\n"
              << "  # Upload firmware file\n"
              << "  " << program_name << " --file firmware.bin --version 2.0.0\n\n"
              << "  # Get device status\n"
              << "  " << program_name << " --status\n\n";
}

int main(int argc, char* argv[]) {
    std::string host = "localhost";
    int port = 8443;
    std::string firmware_file;
    std::string firmware_url;
    std::string version;
    bool verify_ssl = false;
    bool get_status_flag = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--file" && i + 1 < argc) {
            firmware_file = argv[++i];
        } else if (arg == "--url" && i + 1 < argc) {
            firmware_url = argv[++i];
        } else if (arg == "--version" && i + 1 < argc) {
            version = argv[++i];
        } else if (arg == "--verify-ssl") {
            verify_ssl = true;
        } else if (arg == "--status") {
            get_status_flag = true;
        } else if (arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n\n";
            print_usage(argv[0]);
            return 1;
        }
    }

    try {
        FirmwareUploader uploader(host, port, verify_ssl);

        if (get_status_flag) {
            uploader.get_status();
        } else if (!firmware_file.empty()) {
            if (version.empty()) {
                std::cerr << "Error: --version is required when uploading a file\n";
                return 1;
            }
            uploader.upload_file(firmware_file, version);
        } else if (!firmware_url.empty()) {
            if (version.empty()) {
                std::cerr << "Error: --version is required when uploading\n";
                return 1;
            }
            uploader.upload_json(version, firmware_url);
        } else {
            std::cerr << "Error: Either --file, --url, or --status is required\n\n";
            print_usage(argv[0]);
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
