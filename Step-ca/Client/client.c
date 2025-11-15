/*
 * HTTPS Client using Mongoose and step-ca certificates
 */

#include "mongoose.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_debug_level = MG_LL_INFO;
static const char *s_url = "https://localhost:8443/";
static const char *s_tls_cert = "certs/client.crt";
static const char *s_tls_key = "certs/client.key";
static const char *s_root_ca = "certs/root_ca.crt";
static const char *s_method = "GET";
static const char *s_post_data = NULL;
static int s_response_received = 0;
static int s_timeout = 10000;  // 10 seconds

// HTTP event handler
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_CONNECT) {
        struct mg_str host = mg_url_host(s_url);
        
        // Send HTTP request
        if (s_post_data != NULL) {
            mg_printf(c,
                     "%s %s HTTP/1.1\r\n"
                     "Host: %.*s\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: %d\r\n"
                     "\r\n"
                     "%s",
                     s_method, mg_url_uri(s_url), (int) host.len, host.ptr,
                     (int) strlen(s_post_data), s_post_data);
        } else {
            mg_printf(c,
                     "%s %s HTTP/1.1\r\n"
                     "Host: %.*s\r\n"
                     "\r\n",
                     s_method, mg_url_uri(s_url), (int) host.len, host.ptr);
        }
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        printf("\n=== Response ===\n");
        printf("Status: %.*s\n", (int) hm->uri.len, hm->uri.ptr);
        printf("Headers:\n");
        for (int i = 0; i < MG_MAX_HTTP_HEADERS; i++) {
            if (hm->headers[i].name.len == 0) break;
            printf("  %.*s: %.*s\n",
                   (int) hm->headers[i].name.len, hm->headers[i].name.ptr,
                   (int) hm->headers[i].value.len, hm->headers[i].value.ptr);
        }
        printf("\nBody:\n%.*s\n", (int) hm->body.len, hm->body.ptr);
        printf("================\n");
        
        s_response_received = 1;
        c->is_draining = 1;  // Close connection
    } else if (ev == MG_EV_ERROR) {
        fprintf(stderr, "Error: %s\n", (char *) ev_data);
        s_response_received = -1;
    } else if (ev == MG_EV_CLOSE) {
        if (!s_response_received) {
            fprintf(stderr, "Connection closed without response\n");
            s_response_received = -1;
        }
    }
}

void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -u <url>      Server URL (default: %s)\n", s_url);
    fprintf(stderr, "  -m <method>   HTTP method: GET, POST, PUT, DELETE (default: %s)\n", s_method);
    fprintf(stderr, "  -d <data>     POST/PUT data (JSON format)\n");
    fprintf(stderr, "  -c <cert>     Client certificate file (default: %s)\n", s_tls_cert);
    fprintf(stderr, "  -k <key>      Client key file (default: %s)\n", s_tls_key);
    fprintf(stderr, "  -r <ca>       Root CA certificate (default: %s)\n", s_root_ca);
    fprintf(stderr, "  -t <timeout>  Request timeout in ms (default: %d)\n", s_timeout);
    fprintf(stderr, "  -v <level>    Debug level 0-4 (default: %d)\n", s_debug_level);
    fprintf(stderr, "  -h            Show this help\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "  %s -u https://localhost:8443/api/status\n", prog);
    fprintf(stderr, "  %s -u https://localhost:8443/api/data -m POST -d '{\"key\":\"value\"}'\n", prog);
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    struct mg_tls_opts opts = {0};
    int start_time;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i + 1 < argc) {
            s_url = argv[++i];
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            s_method = argv[++i];
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            s_post_data = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            s_tls_cert = argv[++i];
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            s_tls_key = argv[++i];
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            s_root_ca = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            s_timeout = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            s_debug_level = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Validate method and data combination
    if (s_post_data != NULL && 
        strcmp(s_method, "GET") == 0 && 
        strcmp(s_method, "DELETE") == 0) {
        fprintf(stderr, "Warning: Data provided but method is %s\n", s_method);
    }
    
    // Initialize Mongoose event manager
    mg_mgr_init(&mgr);
    mg_log_set(s_debug_level);
    
    // Setup TLS options
    opts.ca = mg_str(s_root_ca);
    opts.cert = mg_str(s_tls_cert);
    opts.key = mg_str(s_tls_key);
    
    printf("Connecting to: %s\n", s_url);
    printf("Method: %s\n", s_method);
    if (s_post_data) {
        printf("Data: %s\n", s_post_data);
    }
    printf("Using certificate: %s\n", s_tls_cert);
    printf("Using key: %s\n", s_tls_key);
    printf("Using CA: %s\n", s_root_ca);
    printf("\n");
    
    // Create HTTPS connection
    c = mg_http_connect(&mgr, s_url, fn, NULL);
    if (c == NULL) {
        fprintf(stderr, "Failed to create connection\n");
        mg_mgr_free(&mgr);
        return 1;
    }
    
    // Set TLS options
    mg_tls_init(c, &opts);
    
    // Event loop with timeout
    start_time = mg_millis();
    while (s_response_received == 0) {
        mg_mgr_poll(&mgr, 1000);
        
        // Check timeout
        if (mg_millis() - start_time > s_timeout) {
            fprintf(stderr, "Request timeout after %d ms\n", s_timeout);
            s_response_received = -1;
            break;
        }
    }
    
    mg_mgr_free(&mgr);
    
    return (s_response_received == 1) ? 0 : 1;
}
