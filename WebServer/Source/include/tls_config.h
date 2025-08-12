#ifndef TLS_CONFIG_H
#define TLS_CONFIG_H

// TLS Configuration for Mongoose Web Server
// This enables TLS 1.3 support using OpenSSL

// Enable OpenSSL TLS backend (only if not already defined)
#ifndef MG_TLS
#define MG_TLS MG_TLS_OPENSSL
#endif

// TLS 1.3 specific configurations
#define MG_ENABLE_SSL 1
#define MG_ENABLE_TLS 1

// OpenSSL specific configurations for TLS 1.3
#ifdef MG_TLS_OPENSSL
// Force TLS 1.3 minimum version
#define MG_SSL_MIN_VERSION TLS1_3_VERSION
#define MG_SSL_MAX_VERSION TLS1_3_VERSION

// Enable modern cipher suites
#define MG_SSL_CIPHERS "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256"

// Security options
#define MG_SSL_OPTIONS (SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2)
#endif

// Certificate and key file paths
#define TLS_CERT_FILE "certs/server.crt"
#define TLS_KEY_FILE "certs/server.key"
#define TLS_CA_FILE "certs/ca.crt"

// TLS port configuration
#define TLS_PORT 8443
#define HTTP_PORT 8080

#endif // TLS_CONFIG_H
