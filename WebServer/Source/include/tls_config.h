#ifndef TLS_CONFIG_H
#define TLS_CONFIG_H

// TLS Configuration for Mongoose Web Server
// Using Mongoose's built-in TLS 1.3 implementation (recommended)

// Enable built-in TLS backend (recommended by Mongoose docs)
#ifndef MG_TLS
#define MG_TLS MG_TLS_BUILTIN
#endif

// TLS configurations
#define MG_ENABLE_SSL 1
#define MG_ENABLE_TLS 1

// Built-in TLS specific configurations
#ifdef MG_TLS_BUILTIN
// Built-in TLS supports TLS 1.3 by default
// No need to specify min/max versions as they're handled internally
#define MG_TLS_BUILTIN_ENABLED 1
#endif

// Certificate and key file paths
#define TLS_CERT_FILE "certs/server.crt"
#define TLS_KEY_FILE "certs/server.key"
#define TLS_CA_FILE "certs/ca.crt"

// TLS port configuration
#define TLS_PORT 8443
#define HTTP_PORT 8080

// TLS security settings
#define TLS_CIPHER_SUITES "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256"
#define TLS_CURVES "X25519:P-256:P-384"

// Certificate validation settings
#define TLS_VERIFY_PEER 1
#define TLS_VERIFY_HOSTNAME 1

#endif // TLS_CONFIG_H
