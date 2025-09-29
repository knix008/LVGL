#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <json-c/json.h>
#include "ca_server.h"

#define MAX_REQUEST_SIZE 8192
#define MAX_RESPONSE_SIZE 16384
#define MAX_CLIENTS 100

typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
    ca_config_t *config;
} client_data_t;

typedef struct {
    int server_socket;
    int port;
    ca_config_t *config;
    pthread_t threads[MAX_CLIENTS];
    int thread_count;
} http_server_t;

// HTTP server functions
int http_server_init(http_server_t *server, int port, ca_config_t *config);
int http_server_start(http_server_t *server);
int http_server_stop(http_server_t *server);
void http_server_cleanup(http_server_t *server);

// Request handling
void *handle_client(void *arg);
int parse_http_request(const char *request, char *method, char *path, char *body);
int send_http_response(int client_socket, int status_code, const char *content_type, const char *body);
int send_json_response(int client_socket, int status_code, json_object *json);

// API endpoints
int api_get_certificates(int client_socket, ca_config_t *config);
int api_get_certificate(int client_socket, const char *serial_number, ca_config_t *config);
int api_create_certificate(int client_socket, const char *body, ca_config_t *config);
int api_revoke_certificate(int client_socket, const char *serial_number, ca_config_t *config);
int api_get_crl(int client_socket, ca_config_t *config);
int api_get_ca_certificate(int client_socket, ca_config_t *config);

// Utility functions
char *url_decode(const char *str);
char *get_mime_type(const char *filename);
void log_request(const char *method, const char *path, int status_code);
int serve_static_file(int client_socket, const char *path);

#endif // HTTP_SERVER_H
