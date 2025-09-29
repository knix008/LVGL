#ifndef CA_SERVER_H
#define CA_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <sqlite3.h>

#define MAX_PATH_LEN 256
#define MAX_CN_LEN 64
#define MAX_EMAIL_LEN 128
#define MAX_ORG_LEN 128
#define MAX_COUNTRY_LEN 3
#define MAX_STATE_LEN 64
#define MAX_CITY_LEN 64

typedef struct {
    char common_name[MAX_CN_LEN];
    char email[MAX_EMAIL_LEN];
    char organization[MAX_ORG_LEN];
    char country[MAX_COUNTRY_LEN];
    char state[MAX_STATE_LEN];
    char city[MAX_CITY_LEN];
    int key_size;
    int validity_days;
} cert_request_t;

typedef struct {
    char serial_number[32];
    char common_name[MAX_CN_LEN];
    char email[MAX_EMAIL_LEN];
    char status[16];
    time_t created_at;
    time_t expires_at;
    char cert_path[MAX_PATH_LEN];
    char key_path[MAX_PATH_LEN];
} cert_record_t;

typedef struct {
    char ca_cert_path[MAX_PATH_LEN];
    char ca_key_path[MAX_PATH_LEN];
    char crl_path[MAX_PATH_LEN];
    char certs_dir[MAX_PATH_LEN];
    char keys_dir[MAX_PATH_LEN];
    char db_path[MAX_PATH_LEN];
    int port;
    int key_size;
    int validity_years;
    int min_key_size;
    int max_validity_days;
    char allowed_countries[256];
} ca_config_t;

// Core CA functions
int ca_init(ca_config_t *config);
int ca_generate_root_ca(ca_config_t *config);
int ca_sign_certificate(cert_request_t *request, ca_config_t *config, char *cert_path, char *key_path);
int ca_revoke_certificate(const char *serial_number, ca_config_t *config);
int ca_generate_crl(ca_config_t *config);
int ca_cleanup(void);

// Database functions
int db_init(const char *db_path);
int db_add_certificate(cert_record_t *record);
int db_get_certificate(const char *serial_number, cert_record_t *record);
int db_list_certificates(cert_record_t **records, int *count);
int db_revoke_certificate(const char *serial_number);
int db_cleanup(void);

// Utility functions
void generate_serial_number(char *serial, size_t len);
int validate_cert_request(cert_request_t *request, ca_config_t *config);
void print_cert_info(X509 *cert);
int load_ca_certificate(const char *path, X509 **cert);
int load_ca_private_key(const char *path, EVP_PKEY **key);

#endif // CA_SERVER_H
