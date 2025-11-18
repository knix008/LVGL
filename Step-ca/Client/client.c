/*
 * Simplified HTTPS Client using OpenSSL for TLS 1.3
 * Makes API calls to HTTPS server with mutual TLS authentication
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/tls1.h>

#define DEFAULT_PORT 8443
#define DEFAULT_HOST "localhost"
#define BUFFER_SIZE 4096

// TLS configuration
static const char *g_host = DEFAULT_HOST;
static int g_port = DEFAULT_PORT;
static const char *g_path = "/";
static const char *g_method = "GET";
static const char *g_data = NULL;
static const char *g_cert_file = "certs/client.crt";
static const char *g_key_file = "certs/client.key";
static const char *g_ca_file = "certs/root_ca.crt";
static int g_verify_peer = 1;
static int g_verbose = 0;

void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -u <url>      Full URL (https://host:port/path) (default: https://localhost:8443/)\n");
    fprintf(stderr, "  -h <host>     Server hostname (default: %s)\n", DEFAULT_HOST);
    fprintf(stderr, "  -p <port>     Server port (default: %d)\n", DEFAULT_PORT);
    fprintf(stderr, "  -P <path>     Request path (default: /)\n");
    fprintf(stderr, "  -m <method>   HTTP method: GET, POST, PUT, DELETE (default: GET)\n");
    fprintf(stderr, "  -d <data>     Request body data\n");
    fprintf(stderr, "  -c <cert>     Client certificate file (default: %s)\n", g_cert_file);
    fprintf(stderr, "  -k <key>      Client private key file (default: %s)\n", g_key_file);
    fprintf(stderr, "  -r <ca>       Root CA certificate file (default: %s)\n", g_ca_file);
    fprintf(stderr, "  -v <level>    Verbose level 0-3 (default: 0)\n");
    fprintf(stderr, "  --no-verify   Skip peer verification\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s -u https://localhost:8443/api/status\n", prog);
    fprintf(stderr, "  %s -h localhost -p 8443 -P /api/status\n", prog);
    fprintf(stderr, "  %s -u https://localhost:8443/api/data -m POST -d '{\"key\":\"value\"}'\n", prog);
}

// Parse URL into components
int parse_url(const char *url, char **host_out, int *port_out, char **path_out) {
    char *temp = strdup(url);
    if (!temp) return 0;

    // Check for https://
    char *ptr = temp;
    if (strncmp(ptr, "https://", 8) != 0) {
        fprintf(stderr, "Error: Only HTTPS URLs are supported\n");
        free(temp);
        return 0;
    }
    ptr += 8;

    // Find host[:port] and path
    char *slash = strchr(ptr, '/');
    char *colon = strchr(ptr, ':');

    int host_len;
    if (colon && (!slash || colon < slash)) {
        // Has port
        host_len = colon - ptr;
    } else if (slash) {
        // No port
        host_len = slash - ptr;
    } else {
        // No port, no path
        host_len = strlen(ptr);
    }

    // Extract host
    *host_out = malloc(host_len + 1);
    strncpy(*host_out, ptr, host_len);
    (*host_out)[host_len] = '\0';
    ptr += host_len;

    // Extract port if present
    *port_out = DEFAULT_PORT;
    if (*ptr == ':') {
        ptr++;
        *port_out = atoi(ptr);
        while (*ptr && *ptr != '/') ptr++;
    }

    // Extract path
    if (*ptr == '/') {
        *path_out = strdup(ptr);
    } else {
        *path_out = strdup("/");
    }

    free(temp);
    return 1;
}

// Create TCP socket connection
int create_socket(const char *host, int port) {
    struct hostent *he;
    struct sockaddr_in sa;
    int sock;

    // Resolve hostname
    he = gethostbyname(host);
    if (!he) {
        fprintf(stderr, "Error: Cannot resolve host '%s'\n", host);
        return -1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // Connect
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr = *(struct in_addr*)he->h_addr;

    if (g_verbose >= 2) {
        printf("[DEBUG] Connecting to %s:%d\n", host, port);
    }

    if (connect(sock, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    if (g_verbose >= 2) {
        printf("[DEBUG] Connected to %s:%d\n", host, port);
    }

    return sock;
}

// Setup TLS connection with mutual TLS
SSL *setup_tls(int sock, const char *host) {
    SSL_CTX *ctx;
    SSL *ssl;

    // Create SSL context for TLS 1.3
    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create SSL context\n");
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    // Set minimum TLS version to TLS 1.3
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION)) {
        fprintf(stderr, "Error: Failed to set TLS 1.3 as minimum version\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (g_verbose >= 2) {
        printf("[DEBUG] TLS version: TLS 1.3\n");
    }

    // Load client certificate and key for mutual TLS
    if (SSL_CTX_use_certificate_file(ctx, g_cert_file, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Error: Failed to load client certificate from '%s'\n", g_cert_file);
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, g_key_file, SSL_FILETYPE_PEM) != 1) {
        fprintf(stderr, "Error: Failed to load private key from '%s'\n", g_key_file);
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Verify client certificate and key match
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Error: Private key does not match certificate\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (g_verbose >= 1) {
        printf("[INFO] Client certificate and key loaded successfully\n");
    }

    // Load CA certificate for server verification
    if (SSL_CTX_load_verify_locations(ctx, g_ca_file, NULL) != 1) {
        fprintf(stderr, "Error: Failed to load CA certificate from '%s'\n", g_ca_file);
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (g_verbose >= 1) {
        printf("[INFO] CA certificate loaded successfully\n");
    }

    // Set peer verification
    if (g_verify_peer) {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    }

    // Create SSL connection object
    ssl = SSL_new(ctx);
    if (!ssl) {
        fprintf(stderr, "Error: Failed to create SSL connection\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // Set hostname for SNI (Server Name Indication)
    SSL_set_tlsext_host_name(ssl, host);

    // Connect SSL to socket
    SSL_set_fd(ssl, sock);

    if (g_verbose >= 2) {
        printf("[DEBUG] Performing TLS handshake...\n");
    }

    // Perform TLS handshake
    if (SSL_connect(ssl) != 1) {
        fprintf(stderr, "Error: TLS handshake failed\n");
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (g_verbose >= 1) {
        printf("[INFO] TLS handshake successful\n");
        printf("[INFO] TLS Version: %s\n", SSL_get_version(ssl));
        printf("[INFO] Cipher: %s\n", SSL_get_cipher_name(ssl));
    }

    return ssl;
}

// Send HTTP request through TLS connection
int send_request(SSL *ssl, const char *host, const char *method,
                 const char *path, const char *data) {
    char request[BUFFER_SIZE];
    int len;

    if (data) {
        len = snprintf(request, sizeof(request),
            "%s %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            method, path, host, strlen(data), data);
    } else {
        len = snprintf(request, sizeof(request),
            "%s %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n",
            method, path, host);
    }

    if (g_verbose >= 3) {
        printf("[DEBUG] Sending request:\n%s\n", request);
    }

    if (SSL_write(ssl, request, len) != len) {
        fprintf(stderr, "Error: Failed to send HTTP request\n");
        ERR_print_errors_fp(stderr);
        return 0;
    }

    return 1;
}

// Receive and print HTTP response
int receive_response(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    printf("\n=== Response ===\n");

    while ((bytes_read = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    // Check if there was an error (not just connection closed)
    int ssl_err = SSL_get_error(ssl, bytes_read);
    if (bytes_read < 0 && ssl_err != SSL_ERROR_ZERO_RETURN) {
        fprintf(stderr, "\nError: Failed to read response\n");
        ERR_print_errors_fp(stderr);
        return 0;
    }

    printf("\n================\n");
    return 1;
}

int main(int argc, char *argv[]) {
    int sock;
    SSL *ssl;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            char *url = argv[++i];
            if (!parse_url(url, (char**)&g_host, &g_port, (char**)&g_path)) {
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            g_host = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            g_port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
            g_path = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            g_method = argv[++i];
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            g_data = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            g_cert_file = argv[++i];
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            g_key_file = argv[++i];
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            g_ca_file = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            g_verbose = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-verify") == 0) {
            g_verify_peer = 0;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    if (g_verbose >= 1) {
        printf("[INFO] Client starting\n");
        printf("[INFO] Server: %s:%d\n", g_host, g_port);
        printf("[INFO] Path: %s\n", g_path);
        printf("[INFO] Method: %s\n", g_method);
        printf("[INFO] Certificate: %s\n", g_cert_file);
        printf("[INFO] Key: %s\n", g_key_file);
        printf("[INFO] CA: %s\n", g_ca_file);
        printf("\n");
    }

    // Initialize OpenSSL
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    // Create socket connection
    sock = create_socket(g_host, g_port);
    if (sock < 0) {
        return 1;
    }

    // Setup TLS connection
    ssl = setup_tls(sock, g_host);
    if (!ssl) {
        close(sock);
        return 1;
    }

    // Send HTTP request
    if (!send_request(ssl, g_host, g_method, g_path, g_data)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        return 1;
    }

    // Receive and print response
    if (!receive_response(ssl)) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        return 1;
    }

    // Cleanup
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
