#include "../include/http_server.h"
#include <errno.h>

int http_server_init(http_server_t *server, int port, ca_config_t *config) {
    server->port = port;
    server->config = config;
    server->thread_count = 0;
    
    // Create socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0) {
        perror("socket");
        return -1;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return -1;
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return -1;
    }
    
    // Listen for connections
    if (listen(server->server_socket, MAX_CLIENTS) < 0) {
        perror("listen");
        return -1;
    }
    
    return 0;
}

int http_server_start(http_server_t *server) {
    printf("HTTP server listening on port %d\n", server->port);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server->server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (errno == EBADF) {
                printf("Server socket closed, stopping...\n");
                break;
            }
            perror("accept");
            continue;
        }
        
        // Create client data
        client_data_t *client_data = malloc(sizeof(client_data_t));
        client_data->client_socket = client_socket;
        client_data->client_addr = client_addr;
        client_data->config = server->config;
        
        // Create thread for client
        if (server->thread_count < MAX_CLIENTS) {
            pthread_create(&server->threads[server->thread_count], NULL, handle_client, client_data);
            server->thread_count++;
        } else {
            close(client_socket);
            free(client_data);
        }
    }
    
    return 0;
}

int http_server_stop(http_server_t *server) {
    close(server->server_socket);
    return 0;
}

void http_server_cleanup(http_server_t *server) {
    for (int i = 0; i < server->thread_count; i++) {
        pthread_join(server->threads[i], NULL);
    }
}

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t*)arg;
    int client_socket = client_data->client_socket;
    
    char request[MAX_REQUEST_SIZE];
    int bytes_received = recv(client_socket, request, sizeof(request) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        free(client_data);
        return NULL;
    }
    
    request[bytes_received] = '\0';
    
    char method[16], path[256], body[4096];
    if (parse_http_request(request, method, path, body) != 0) {
        send_http_response(client_socket, 400, "text/plain", "Bad Request");
        close(client_socket);
        free(client_data);
        return NULL;
    }
    
    log_request(method, path, 200);
    
    // Route requests
    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/api/certificates") == 0) {
            api_get_certificates(client_socket, client_data->config);
        } else if (strncmp(path, "/api/certificates/", 18) == 0) {
            char *serial_number = url_decode(path + 18);
            api_get_certificate(client_socket, serial_number, client_data->config);
            free(serial_number);
        } else if (strcmp(path, "/api/crl") == 0) {
            api_get_crl(client_socket, client_data->config);
        } else if (strcmp(path, "/api/ca") == 0) {
            api_get_ca_certificate(client_socket, client_data->config);
        } else {
            // Try to serve static files from web directory
            serve_static_file(client_socket, path);
        }
    } else if (strcmp(method, "POST") == 0) {
        if (strcmp(path, "/api/certificates") == 0) {
            api_create_certificate(client_socket, body, client_data->config);
        } else {
            send_http_response(client_socket, 404, "text/plain", "Not Found");
        }
    } else if (strcmp(method, "DELETE") == 0) {
        if (strncmp(path, "/api/certificates/", 18) == 0) {
            char *serial_number = url_decode(path + 18);
            api_revoke_certificate(client_socket, serial_number, client_data->config);
            free(serial_number);
        } else {
            send_http_response(client_socket, 404, "text/plain", "Not Found");
        }
    } else {
        send_http_response(client_socket, 405, "text/plain", "Method Not Allowed");
    }
    
    close(client_socket);
    free(client_data);
    return NULL;
}

int parse_http_request(const char *request, char *method, char *path, char *body) {
    char *line_end = strstr(request, "\r\n");
    if (!line_end) return -1;
    
    sscanf(request, "%s %s", method, path);
    
    char *body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        strcpy(body, body_start + 4);
    } else {
        body[0] = '\0';
    }
    
    return 0;
}

int send_http_response(int client_socket, int status_code, const char *content_type, const char *body) {
    char response[MAX_RESPONSE_SIZE];
    const char *status_text;
    
    switch (status_code) {
        case 200: status_text = "OK"; break;
        case 201: status_text = "Created"; break;
        case 400: status_text = "Bad Request"; break;
        case 404: status_text = "Not Found"; break;
        case 405: status_text = "Method Not Allowed"; break;
        case 500: status_text = "Internal Server Error"; break;
        default: status_text = "Unknown"; break;
    }
    
    snprintf(response, sizeof(response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        status_code, status_text, content_type, strlen(body), body);
    
    return send(client_socket, response, strlen(response), 0);
}

int send_json_response(int client_socket, int status_code, json_object *json) {
    const char *json_string = json_object_to_json_string(json);
    return send_http_response(client_socket, status_code, "application/json", json_string);
}

int serve_static_file(int client_socket, const char *path) {
    char file_path[512];
    char *content_type;
    FILE *fp;
    long file_size;
    char *file_data;
    
    // Handle root path
    if (strcmp(path, "/") == 0) {
        strcpy(file_path, "web/index.html");
    } else {
        // Remove leading slash and prepend web/
        snprintf(file_path, sizeof(file_path), "web%s", path);
    }
    
    // Open file
    fp = fopen(file_path, "rb");
    if (!fp) {
        send_http_response(client_socket, 404, "text/plain", "File Not Found");
        return -1;
    }
    
    // Get file size
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Allocate memory for file content
    file_data = malloc(file_size + 1);
    if (!file_data) {
        fclose(fp);
        send_http_response(client_socket, 500, "text/plain", "Internal Server Error");
        return -1;
    }
    
    // Read file content
    if (fread(file_data, 1, (size_t)file_size, fp) != (size_t)file_size) {
        free(file_data);
        fclose(fp);
        send_http_response(client_socket, 500, "text/plain", "Failed to read file");
        return -1;
    }
    file_data[file_size] = '\0';
    fclose(fp);
    
    // Determine content type
    content_type = get_mime_type(file_path);
    
    // Send file content
    send_http_response(client_socket, 200, content_type, file_data);
    
    free(file_data);
    return 0;
}

int api_get_certificates(int client_socket, ca_config_t *config) {
    (void)config; // Suppress unused parameter warning
    cert_record_t *records;
    int count;
    
    if (db_list_certificates(&records, &count) != 0) {
        json_object *json = json_object_new_object();
        json_object_object_add(json, "error", json_object_new_string("Failed to retrieve certificates"));
        send_json_response(client_socket, 500, json);
        json_object_put(json);
        return -1;
    }
    
    json_object *json = json_object_new_object();
    json_object *certificates = json_object_new_array();
    
    for (int i = 0; i < count; i++) {
        json_object *cert = json_object_new_object();
        json_object_object_add(cert, "serial_number", json_object_new_string(records[i].serial_number));
        json_object_object_add(cert, "common_name", json_object_new_string(records[i].common_name));
        json_object_object_add(cert, "email", json_object_new_string(records[i].email));
        json_object_object_add(cert, "status", json_object_new_string(records[i].status));
        json_object_object_add(cert, "created_at", json_object_new_int64(records[i].created_at));
        json_object_object_add(cert, "expires_at", json_object_new_int64(records[i].expires_at));
        json_object_array_add(certificates, cert);
    }
    
    json_object_object_add(json, "certificates", certificates);
    send_json_response(client_socket, 200, json);
    
    json_object_put(json);
    free(records);
    return 0;
}

int api_get_certificate(int client_socket, const char *serial_number, ca_config_t *config) {
    (void)config; // Suppress unused parameter warning
    cert_record_t record;
    
    if (db_get_certificate(serial_number, &record) != 0) {
        json_object *json = json_object_new_object();
        json_object_object_add(json, "error", json_object_new_string("Certificate not found"));
        send_json_response(client_socket, 404, json);
        json_object_put(json);
        return -1;
    }
    
    json_object *json = json_object_new_object();
    json_object_object_add(json, "serial_number", json_object_new_string(record.serial_number));
    json_object_object_add(json, "common_name", json_object_new_string(record.common_name));
    json_object_object_add(json, "email", json_object_new_string(record.email));
    json_object_object_add(json, "status", json_object_new_string(record.status));
    json_object_object_add(json, "created_at", json_object_new_int64(record.created_at));
    json_object_object_add(json, "expires_at", json_object_new_int64(record.expires_at));
    
    send_json_response(client_socket, 200, json);
    json_object_put(json);
    return 0;
}

int api_create_certificate(int client_socket, const char *body, ca_config_t *config) {
    json_object *json = json_tokener_parse(body);
    if (!json) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("Invalid JSON"));
        send_json_response(client_socket, 400, error);
        json_object_put(error);
        return -1;
    }
    
    cert_request_t request;
    json_object *obj;
    
    if (json_object_object_get_ex(json, "common_name", &obj)) {
        strncpy(request.common_name, json_object_get_string(obj), sizeof(request.common_name) - 1);
        request.common_name[sizeof(request.common_name) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "email", &obj)) {
        strncpy(request.email, json_object_get_string(obj), sizeof(request.email) - 1);
        request.email[sizeof(request.email) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "organization", &obj)) {
        strncpy(request.organization, json_object_get_string(obj), sizeof(request.organization) - 1);
        request.organization[sizeof(request.organization) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "country", &obj)) {
        strncpy(request.country, json_object_get_string(obj), sizeof(request.country) - 1);
        request.country[sizeof(request.country) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "state", &obj)) {
        strncpy(request.state, json_object_get_string(obj), sizeof(request.state) - 1);
        request.state[sizeof(request.state) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "city", &obj)) {
        strncpy(request.city, json_object_get_string(obj), sizeof(request.city) - 1);
        request.city[sizeof(request.city) - 1] = '\0';
    }
    if (json_object_object_get_ex(json, "key_size", &obj)) {
        request.key_size = json_object_get_int(obj);
    } else {
        request.key_size = 2048;
    }
    if (json_object_object_get_ex(json, "validity_days", &obj)) {
        request.validity_days = json_object_get_int(obj);
    } else {
        request.validity_days = 365;
    }
    
    if (validate_cert_request(&request, config) != 0) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("Invalid certificate request - check country code, key size, and validity period"));
        send_json_response(client_socket, 400, error);
        json_object_put(error);
        json_object_put(json);
        return -1;
    }
    
    char cert_path[MAX_PATH_LEN];
    char key_path[MAX_PATH_LEN];
    
    // Check if paths will fit in buffer
    int cert_path_len = snprintf(cert_path, sizeof(cert_path), "%s/%s.crt", config->certs_dir, request.common_name);
    int key_path_len = snprintf(key_path, sizeof(key_path), "%s/%s.key", config->keys_dir, request.common_name);
    
    if (cert_path_len >= MAX_PATH_LEN || key_path_len >= MAX_PATH_LEN) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("Path too long"));
        send_json_response(client_socket, 400, error);
        json_object_put(error);
        json_object_put(json);
        return -1;
    }
    
    if (ca_sign_certificate(&request, config, cert_path, key_path) != 0) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("Failed to create certificate"));
        send_json_response(client_socket, 500, error);
        json_object_put(error);
        json_object_put(json);
        return -1;
    }
    
    json_object *response = json_object_new_object();
    json_object_object_add(response, "message", json_object_new_string("Certificate created successfully"));
    json_object_object_add(response, "cert_path", json_object_new_string(cert_path));
    json_object_object_add(response, "key_path", json_object_new_string(key_path));
    
    send_json_response(client_socket, 201, response);
    
    json_object_put(response);
    json_object_put(json);
    return 0;
}

int api_revoke_certificate(int client_socket, const char *serial_number, ca_config_t *config) {
    if (ca_revoke_certificate(serial_number, config) != 0) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("Failed to revoke certificate"));
        send_json_response(client_socket, 500, error);
        json_object_put(error);
        return -1;
    }
    
    json_object *response = json_object_new_object();
    json_object_object_add(response, "message", json_object_new_string("Certificate revoked successfully"));
    
    send_json_response(client_socket, 200, response);
    json_object_put(response);
    return 0;
}

int api_get_crl(int client_socket, ca_config_t *config) {
    FILE *fp = fopen(config->crl_path, "r");
    if (!fp) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("CRL not found"));
        send_json_response(client_socket, 404, error);
        json_object_put(error);
        return -1;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *crl_data = malloc(size + 1);
    if (fread(crl_data, 1, (size_t)size, fp) != (size_t)size) {
        free(crl_data);
        fclose(fp);
        send_http_response(client_socket, 500, "text/plain", "Failed to read CRL file");
        return -1;
    }
    crl_data[size] = '\0';
    fclose(fp);
    
    // Send CRL with proper filename
    char header[512];
    snprintf(header, sizeof(header), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/x-pem-file\r\n"
        "Content-Disposition: attachment; filename=\"ca.crl\"\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", size);
    
    send(client_socket, header, strlen(header), 0);
    send(client_socket, crl_data, size, 0);
    free(crl_data);
    return 0;
}

int api_get_ca_certificate(int client_socket, ca_config_t *config) {
    FILE *fp = fopen(config->ca_cert_path, "r");
    if (!fp) {
        json_object *error = json_object_new_object();
        json_object_object_add(error, "error", json_object_new_string("CA certificate not found"));
        send_json_response(client_socket, 404, error);
        json_object_put(error);
        return -1;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *cert_data = malloc(size + 1);
    if (fread(cert_data, 1, (size_t)size, fp) != (size_t)size) {
        free(cert_data);
        fclose(fp);
        send_http_response(client_socket, 500, "text/plain", "Failed to read CA certificate file");
        return -1;
    }
    cert_data[size] = '\0';
    fclose(fp);
    
    // Send CA certificate with proper filename
    char header[512];
    snprintf(header, sizeof(header), 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/x-pem-file\r\n"
        "Content-Disposition: attachment; filename=\"ca.crt\"\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", size);
    
    send(client_socket, header, strlen(header), 0);
    send(client_socket, cert_data, size, 0);
    free(cert_data);
    return 0;
}

char *url_decode(const char *str) {
    char *decoded = malloc(strlen(str) + 1);
    char *dst = decoded;
    
    while (*str) {
        if (*str == '%' && strlen(str) >= 3) {
            char hex[3] = {str[1], str[2], '\0'};
            *dst++ = (char)strtol(hex, NULL, 16);
            str += 3;
        } else {
            *dst++ = *str++;
        }
    }
    *dst = '\0';
    
    return decoded;
}

char *get_mime_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";
    
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    
    return "application/octet-stream";
}

void log_request(const char *method, const char *path, int status_code) {
    printf("[%s] %s %s - %d\n", "INFO", method, path, status_code);
}
