#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

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

#ifdef __cplusplus
}
#endif

#endif // NETWORK_H
