#include "../include/ca_server.h"
#include "../include/http_server.h"
#include "../include/config_parser.h"
#include <signal.h>
#include <unistd.h>

static http_server_t server;
static int running = 1;
static http_server_t *global_server = NULL;

void signal_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    printf("\nShutting down CA server...\n");
    running = 0;
    if (global_server) {
        http_server_stop(global_server);
    }
}

int main(int argc, char *argv[]) {
    ca_config_t config;
    config_t file_config;
    
    // Load configuration from file
    if (load_config("config/ca.conf", &file_config) == 0) {
        // Copy from file config to ca_config
        strcpy(config.ca_cert_path, file_config.ca_cert_path);
        strcpy(config.ca_key_path, file_config.ca_key_path);
        strcpy(config.crl_path, file_config.crl_path);
        strcpy(config.certs_dir, file_config.certs_dir);
        strcpy(config.keys_dir, file_config.keys_dir);
        strcpy(config.db_path, file_config.db_path);
        config.port = file_config.port;
        config.key_size = file_config.key_size;
        config.validity_years = file_config.validity_years;
        config.min_key_size = file_config.min_key_size;
        config.max_validity_days = file_config.max_validity_days;
        strcpy(config.allowed_countries, file_config.allowed_countries);
        } else {
            // Set default configuration if file loading fails
            strcpy(config.ca_cert_path, "certs/ca.crt");
            strcpy(config.ca_key_path, "certs/ca.key");
            strcpy(config.crl_path, "certs/ca.crl");
            strcpy(config.certs_dir, "certs");
            strcpy(config.keys_dir, "certs");
            strcpy(config.db_path, "certs/ca.db");
            config.port = 8080;
            config.key_size = 4096;
            config.validity_years = 10;
            config.min_key_size = 2048;
            config.max_validity_days = 365;
            strcpy(config.allowed_countries, "");
        }
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "p:k:c:r:d:b:y:")) != -1) {
        switch (opt) {
            case 'p':
                config.port = atoi(optarg);
                break;
            case 'k':
                strncpy(config.ca_key_path, optarg, sizeof(config.ca_key_path) - 1);
                break;
            case 'c':
                strncpy(config.ca_cert_path, optarg, sizeof(config.ca_cert_path) - 1);
                break;
            case 'r':
                strncpy(config.crl_path, optarg, sizeof(config.crl_path) - 1);
                break;
            case 'd':
                strncpy(config.db_path, optarg, sizeof(config.db_path) - 1);
                break;
            case 'b':
                config.key_size = atoi(optarg);
                break;
            case 'y':
                config.validity_years = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-p port] [-k key_file] [-c cert_file] [-r crl_file] [-d db_file] [-b key_size] [-y validity_years]\n", argv[0]);
                return 1;
        }
    }
    
    // Create directories
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s %s", config.certs_dir, config.keys_dir);
    if (system(cmd) != 0) {
        fprintf(stderr, "Warning: Failed to create directories\n");
    }
    
    // Check if CA certificate exists, create if not
    FILE *fp = fopen(config.ca_cert_path, "r");
    if (!fp) {
        printf("CA certificate not found. Generating root CA...\n");
        if (ca_generate_root_ca(&config) != 0) {
            fprintf(stderr, "Failed to generate root CA\n");
            return 1;
        }
        printf("Root CA generated successfully\n");
    } else {
        fclose(fp);
        printf("Using existing CA certificate: %s\n", config.ca_cert_path);
    }
    
    // Initialize CA
    if (ca_init(&config) != 0) {
        fprintf(stderr, "Failed to initialize CA\n");
        return 1;
    }
    
    // Initialize HTTP server
    if (http_server_init(&server, config.port, &config) != 0) {
        fprintf(stderr, "Failed to initialize HTTP server\n");
        return 1;
    }
    
    // Set global server pointer for signal handler
    global_server = &server;
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("Certificate Authority Server\n");
    printf("============================\n");
    printf("CA Certificate: %s\n", config.ca_cert_path);
    printf("CA Private Key: %s\n", config.ca_key_path);
    printf("CRL: %s\n", config.crl_path);
    printf("Database: %s\n", config.db_path);
    printf("Certificates Directory: %s\n", config.certs_dir);
    printf("Keys Directory: %s\n", config.keys_dir);
    printf("HTTP Server Port: %d\n", config.port);
    printf("Minimum Key Size: %d bits\n", config.min_key_size);
    printf("Maximum Validity: %d days\n", config.max_validity_days);
    printf("Allowed Countries: %s\n", strlen(config.allowed_countries) > 0 ? config.allowed_countries : "All");
    printf("\nAPI Endpoints:\n");
    printf("  GET  /api/certificates     - List all certificates\n");
    printf("  GET  /api/certificates/{id} - Get certificate details\n");
    printf("  POST /api/certificates     - Create new certificate\n");
    printf("  DELETE /api/certificates/{id} - Revoke certificate\n");
    printf("  GET  /api/crl              - Get Certificate Revocation List\n");
    printf("  GET  /api/ca               - Get CA certificate\n");
    printf("\nPress Ctrl+C to stop the server\n\n");
    
    // Start HTTP server
    http_server_start(&server);
    
    // Cleanup
    http_server_cleanup(&server);
    ca_cleanup();
    
    return 0;
}
