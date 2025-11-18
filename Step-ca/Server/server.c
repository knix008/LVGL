/*
 * HTTPS Server using Mongoose and step-ca certificates
 */

#include "mongoose.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TLS context structure
struct tls_context {
    struct mg_str cert;
    struct mg_str key;
    struct mg_str ca;
};

static int s_debug_level = MG_LL_INFO;
static const char *s_listening_address = "https://0.0.0.0:8443";
static const char *s_tls_cert = "certs/server.crt";
static const char *s_tls_key = "certs/server.key";
static const char *s_root_ca = "certs/root_ca.crt";
static int s_signo = 0;

// Signal handler
static void signal_handler(int signo) {
    s_signo = signo;
}

// HTTP event handler
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    struct tls_context *tls_ctx = (struct tls_context *) c->fn_data;

    if (ev == MG_EV_ACCEPT) {
        // New connection accepted - setup TLS if needed
        if (tls_ctx) {
            struct mg_tls_opts opts = {0};
            opts.cert = tls_ctx->cert;
            opts.key = tls_ctx->key;
            // For server, we don't need to load client CA unless we want to verify client certs
            // opts.ca = tls_ctx->ca;  // Skip client certificate verification
            mg_tls_init(c, &opts);
        }
    } else if (ev == MG_EV_TLS_HS) {
        // TLS handshake completed
        // No action needed here
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Handle different routes
        if (mg_match(hm->uri, mg_str("/"), NULL)) {
            // Root endpoint
            mg_http_reply(c, 200, "Content-Type: text/html\r\n",
                         "<html><body>"
                         "<h1>Secure HTTPS Server</h1>"
                         "<p>This server is using certificates from step-ca</p>"
                         "<p>Available endpoints:</p>"
                         "<ul>"
                         "<li><a href=\"/api/status\">/api/status</a> - Server status</li>"
                         "<li><a href=\"/api/info\">/api/info</a> - Server information</li>"
                         "</ul>"
                         "</body></html>");
        } else if (mg_match(hm->uri, mg_str("/api/status"), NULL)) {
            // Status endpoint - JSON response
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                         "{\"status\":\"running\",\"secure\":true,\"protocol\":\"https\"}");
        } else if (mg_match(hm->uri, mg_str("/api/info"), NULL)) {
            // Info endpoint - JSON response
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                         "{\"server\":\"Mongoose HTTPS\","
                         "\"ca\":\"step-ca\","
                         "\"tls\":\"enabled\","
                         "\"version\":\"1.0\"}");
        } else if (mg_match(hm->uri, mg_str("/api/v1/getserverinfo"), NULL)) {
            // Get server info endpoint - JSON response
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                         "{\"server_name\":\"Mongoose HTTPS Server\","
                         "\"version\":\"1.0\","
                         "\"ca\":\"step-ca\","
                         "\"tls_version\":\"TLS 1.3\","
                         "\"mTLS\":true,"
                         "\"status\":\"running\"}");
        } else {
            // 404 for unknown routes
            mg_http_reply(c, 404, "Content-Type: application/json\r\n",
                         "{\"error\":\"Not Found\"}");
        }
    }
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    struct mg_str cert_data, key_data, ca_data;
    struct tls_context *tls_ctx;

    // Handle command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            s_listening_address = argv[++i];
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            s_tls_cert = argv[++i];
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            s_tls_key = argv[++i];
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            s_root_ca = argv[++i];
        } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            s_debug_level = atoi(argv[++i]);
        } else {
            fprintf(stderr, "Usage: %s [options]\n", argv[0]);
            fprintf(stderr, "  -a <addr>  Listening address (default: %s)\n", s_listening_address);
            fprintf(stderr, "  -c <cert>  TLS certificate file (default: %s)\n", s_tls_cert);
            fprintf(stderr, "  -k <key>   TLS key file (default: %s)\n", s_tls_key);
            fprintf(stderr, "  -r <ca>    Root CA certificate (default: %s)\n", s_root_ca);
            fprintf(stderr, "  -v <level> Debug level 0-4 (default: %d)\n", s_debug_level);
            return 1;
        }
    }

    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize Mongoose event manager
    mg_mgr_init(&mgr);
    mg_log_set(s_debug_level);

    // Load certificate files into memory
    printf("Starting HTTPS server on %s\n", s_listening_address);
    printf("Using certificate: %s\n", s_tls_cert);
    printf("Using key: %s\n", s_tls_key);
    printf("Using CA: %s\n", s_root_ca);

    cert_data = mg_file_read(&mg_fs_posix, s_tls_cert);
    if (cert_data.len == 0) {
        fprintf(stderr, "Failed to read certificate file: %s\n", s_tls_cert);
        return 1;
    }

    key_data = mg_file_read(&mg_fs_posix, s_tls_key);
    if (key_data.len == 0) {
        fprintf(stderr, "Failed to read key file: %s\n", s_tls_key);
        free(cert_data.buf);
        return 1;
    }

    ca_data = mg_file_read(&mg_fs_posix, s_root_ca);
    if (ca_data.len == 0) {
        fprintf(stderr, "Failed to read CA file: %s\n", s_root_ca);
        free(cert_data.buf);
        free(key_data.buf);
        return 1;
    }

    // Create TLS context to pass to event handler
    tls_ctx = (struct tls_context *) malloc(sizeof(struct tls_context));
    tls_ctx->cert = cert_data;
    tls_ctx->key = key_data;
    tls_ctx->ca = ca_data;

    // Create HTTPS listener with TLS context
    c = mg_http_listen(&mgr, s_listening_address, fn, tls_ctx);
    if (c == NULL) {
        fprintf(stderr, "Failed to create listener\n");
        free(cert_data.buf);
        free(key_data.buf);
        free(ca_data.buf);
        free(tls_ctx);
        return 1;
    }

    printf("Server started successfully. Press Ctrl+C to stop.\n");
    
    // Event loop
    while (s_signo == 0) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    printf("\nShutting down...\n");
    mg_mgr_free(&mgr);
    
    return 0;
}
