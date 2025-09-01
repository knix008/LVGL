#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"
#include <stddef.h>

// Forward declarations for C++ types
#ifdef __cplusplus
extern "C" {
#endif

// Network interface test structure
typedef struct {
    char interface_name[64];
    char ip_address[16];
    char mac_address[18];
    bool is_up;
    int speed_mbps;
    int mtu;
    unsigned long long rx_bytes;
    unsigned long long tx_bytes;
    unsigned long long rx_packets;
    unsigned long long tx_packets;
} network_test_t;



// Network test functions
int init_network_test(network_test_t* network, const char* interface_name);
void cleanup_network_test(network_test_t* network);
bool get_network_stats(network_test_t* network);
bool test_network_connectivity_internal(network_test_t* network, const char* target_host);
void run_network_test_console(network_test_t* network);

// Test functions
test_result_t test_network_initialization(const char* interface_name);
test_result_t test_network_connectivity(const char* interface_name, const char* target_host);
test_result_t test_network_speed(const char* interface_name);
test_result_t test_network_packet_loss(const char* interface_name, const char* target_host);

// Test suite function
test_summary_t run_all_network_tests(const char* interface_name);

// Command handler function
int handle_network_commands(const char* test_type, const char* interface_name, bool interactive_mode);

// Network server functions for remote command execution
typedef struct {
    int server_socket;
    int port;
    bool running;
    char bind_address[16];
} network_server_t;

typedef struct {
    char command[64];
    char device_type[32];
    char parameters[256];
    int client_socket;
} remote_command_t;

// Server management functions
int init_network_server(network_server_t* server, int port, const char* bind_address);
void cleanup_network_server(network_server_t* server);
int start_network_server(network_server_t* server);
void stop_network_server(network_server_t* server);
int handle_client_connection(network_server_t* server, int client_socket);

// Command processing functions
int process_remote_command(const remote_command_t* cmd, char* response, size_t response_size);
int parse_command_json(const char* json_str, remote_command_t* cmd);
int create_response_json(const char* command, bool success, const char* message, double score, char* response, size_t response_size);

// Main server loop function
int run_command_server(int port, const char* bind_address);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_H
