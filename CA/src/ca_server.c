#include "../include/ca_server.h"
#include "../include/config_parser.h"

static X509 *ca_cert = NULL;
static EVP_PKEY *ca_key = NULL;
static sqlite3 *db = NULL;

int ca_init(ca_config_t *config) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    // Initialize database
    if (db_init(config->db_path) != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return -1;
    }
    
    // Load CA certificate and key
    if (load_ca_certificate(config->ca_cert_path, &ca_cert) != 0) {
        fprintf(stderr, "Failed to load CA certificate\n");
        return -1;
    }
    
    if (load_ca_private_key(config->ca_key_path, &ca_key) != 0) {
        fprintf(stderr, "Failed to load CA private key\n");
        return -1;
    }
    
    return 0;
}

int ca_generate_root_ca(ca_config_t *config) {
    X509 *cert = X509_new();
    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_CTX *pkey_ctx = NULL;
    BIGNUM *bn = BN_new();
    
    if (!cert || !key || !bn) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    
    // Generate RSA key using modern EVP API
    pkey_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!pkey_ctx) {
        fprintf(stderr, "Failed to create EVP_PKEY_CTX\n");
        return -1;
    }
    
    if (EVP_PKEY_keygen_init(pkey_ctx) <= 0) {
        fprintf(stderr, "Failed to initialize key generation\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pkey_ctx, config->key_size) <= 0) {
        fprintf(stderr, "Failed to set RSA key size\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    BN_set_word(bn, RSA_F4);
    if (EVP_PKEY_CTX_set1_rsa_keygen_pubexp(pkey_ctx, bn) <= 0) {
        fprintf(stderr, "Failed to set RSA public exponent\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    if (EVP_PKEY_keygen(pkey_ctx, &key) <= 0) {
        fprintf(stderr, "Failed to generate RSA key\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    EVP_PKEY_CTX_free(pkey_ctx);
    
    // Set certificate properties
    X509_set_version(cert, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);
    X509_gmtime_adj(X509_get_notBefore(cert), 0);
    X509_gmtime_adj(X509_get_notAfter(cert), 365L * 24L * 60L * 60L * config->validity_years);
    
    // Set subject and issuer
    X509_NAME *name = X509_get_subject_name(cert);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char*)"State", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char*)"City", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)"Certificate Authority", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"Root CA", -1, -1, 0);
    
    X509_set_issuer_name(cert, name);
    X509_set_pubkey(cert, key);
    
    // Add extensions
    X509V3_CTX ctx;
    X509V3_set_ctx(&ctx, cert, cert, NULL, NULL, 0);
    
    X509_EXTENSION *ext = X509V3_EXT_nconf_nid(NULL, &ctx, NID_basic_constraints, "CA:TRUE");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    ext = X509V3_EXT_nconf_nid(NULL, &ctx, NID_key_usage, "keyCertSign, cRLSign");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    // Sign the certificate
    if (X509_sign(cert, key, EVP_sha256()) <= 0) {
        fprintf(stderr, "Failed to sign certificate\n");
        return -1;
    }
    
    // Save certificate and key
    FILE *fp = fopen(config->ca_cert_path, "w");
    if (fp) {
        PEM_write_X509(fp, cert);
        fclose(fp);
    }
    
    fp = fopen(config->ca_key_path, "w");
    if (fp) {
        PEM_write_PrivateKey(fp, key, NULL, NULL, 0, NULL, NULL);
        fclose(fp);
    }
    
    // Cleanup
    X509_free(cert);
    EVP_PKEY_free(key);
    BN_free(bn);
    
    return 0;
}

int ca_sign_certificate(cert_request_t *request, ca_config_t *config, char *cert_path, char *key_path) {
    (void)config; // Suppress unused parameter warning
    X509 *cert = X509_new();
    EVP_PKEY *key = EVP_PKEY_new();
    EVP_PKEY_CTX *pkey_ctx = NULL;
    BIGNUM *bn = BN_new();
    
    if (!cert || !key || !bn) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    
    // Generate RSA key using modern EVP API
    pkey_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!pkey_ctx) {
        fprintf(stderr, "Failed to create EVP_PKEY_CTX\n");
        return -1;
    }
    
    if (EVP_PKEY_keygen_init(pkey_ctx) <= 0) {
        fprintf(stderr, "Failed to initialize key generation\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pkey_ctx, request->key_size) <= 0) {
        fprintf(stderr, "Failed to set RSA key size\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    BN_set_word(bn, RSA_F4);
    if (EVP_PKEY_CTX_set1_rsa_keygen_pubexp(pkey_ctx, bn) <= 0) {
        fprintf(stderr, "Failed to set RSA public exponent\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    if (EVP_PKEY_keygen(pkey_ctx, &key) <= 0) {
        fprintf(stderr, "Failed to generate RSA key\n");
        EVP_PKEY_CTX_free(pkey_ctx);
        return -1;
    }
    
    EVP_PKEY_CTX_free(pkey_ctx);
    
    // Set certificate properties
    X509_set_version(cert, 2);
    
    // Generate serial number
    char serial[32];
    generate_serial_number(serial, sizeof(serial));
    BIGNUM *bn_serial = BN_new();
    BN_hex2bn(&bn_serial, serial);
    ASN1_INTEGER *serial_int = BN_to_ASN1_INTEGER(bn_serial, NULL);
    X509_set_serialNumber(cert, serial_int);
    ASN1_INTEGER_free(serial_int);
    BN_free(bn_serial);
    
    X509_gmtime_adj(X509_get_notBefore(cert), 0);
    X509_gmtime_adj(X509_get_notAfter(cert), 86400L * request->validity_days);
    
    // Set subject
    X509_NAME *name = X509_get_subject_name(cert);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)request->country, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "ST", MBSTRING_ASC, (unsigned char*)request->state, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "L", MBSTRING_ASC, (unsigned char*)request->city, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)request->organization, -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)request->common_name, -1, -1, 0);
    
    X509_set_issuer_name(cert, X509_get_subject_name(ca_cert));
    X509_set_pubkey(cert, key);
    
    // Add extensions
    X509V3_CTX ctx;
    X509V3_set_ctx(&ctx, ca_cert, cert, NULL, NULL, 0);
    
    X509_EXTENSION *ext = X509V3_EXT_nconf_nid(NULL, &ctx, NID_basic_constraints, "CA:FALSE");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    ext = X509V3_EXT_nconf_nid(NULL, &ctx, NID_key_usage, "digitalSignature, keyEncipherment");
    X509_add_ext(cert, ext, -1);
    X509_EXTENSION_free(ext);
    
    // Sign the certificate with CA key
    if (X509_sign(cert, ca_key, EVP_sha256()) <= 0) {
        fprintf(stderr, "Failed to sign certificate\n");
        return -1;
    }
    
    // Save certificate and key
    FILE *fp = fopen(cert_path, "w");
    if (fp) {
        PEM_write_X509(fp, cert);
        fclose(fp);
    }
    
    fp = fopen(key_path, "w");
    if (fp) {
        PEM_write_PrivateKey(fp, key, NULL, NULL, 0, NULL, NULL);
        fclose(fp);
    }
    
    // Add to database
    cert_record_t record;
    strcpy(record.serial_number, serial);
    strcpy(record.common_name, request->common_name);
    strcpy(record.email, request->email);
    strcpy(record.status, "valid");
    record.created_at = time(NULL);
    record.expires_at = time(NULL) + (86400L * request->validity_days);
    strcpy(record.cert_path, cert_path);
    strcpy(record.key_path, key_path);
    
    db_add_certificate(&record);
    
    // Cleanup
    X509_free(cert);
    EVP_PKEY_free(key);
    BN_free(bn);
    
    return 0;
}

int ca_revoke_certificate(const char *serial_number, ca_config_t *config) {
    if (db_revoke_certificate(serial_number) != 0) {
        return -1;
    }
    
    return ca_generate_crl(config);
}

int ca_generate_crl(ca_config_t *config) {
    X509_CRL *crl = X509_CRL_new();
    if (!crl) {
        return -1;
    }
    
    X509_CRL_set_version(crl, 1);
    X509_CRL_set_issuer_name(crl, X509_get_subject_name(ca_cert));
    X509_CRL_set_lastUpdate(crl, X509_get_notBefore(ca_cert));
    X509_CRL_set_nextUpdate(crl, X509_get_notAfter(ca_cert));
    
    // Add revoked certificates from database
    cert_record_t *records;
    int count;
    if (db_list_certificates(&records, &count) == 0) {
        for (int i = 0; i < count; i++) {
            if (strcmp(records[i].status, "revoked") == 0) {
                X509_REVOKED *revoked = X509_REVOKED_new();
                BIGNUM *bn_serial = BN_new();
                BN_hex2bn(&bn_serial, records[i].serial_number);
                ASN1_INTEGER *serial_int = BN_to_ASN1_INTEGER(bn_serial, NULL);
                X509_REVOKED_set_serialNumber(revoked, serial_int);
                ASN1_INTEGER_free(serial_int);
                BN_free(bn_serial);
                X509_CRL_add0_revoked(crl, revoked);
            }
        }
        free(records);
    }
    
    // Sign CRL
    if (X509_CRL_sign(crl, ca_key, EVP_sha256()) <= 0) {
        X509_CRL_free(crl);
        return -1;
    }
    
    // Save CRL
    FILE *fp = fopen(config->crl_path, "w");
    if (fp) {
        PEM_write_X509_CRL(fp, crl);
        fclose(fp);
    }
    
    X509_CRL_free(crl);
    return 0;
}

int ca_cleanup(void) {
    if (ca_cert) X509_free(ca_cert);
    if (ca_key) EVP_PKEY_free(ca_key);
    if (db) sqlite3_close(db);
    
    EVP_cleanup();
    ERR_free_strings();
    CRYPTO_cleanup_all_ex_data();
    
    return 0;
}

void generate_serial_number(char *serial, size_t len) {
    unsigned char bytes[16];
    RAND_bytes(bytes, sizeof(bytes));
    
    char hex[33];
    for (int i = 0; i < 16; i++) {
        sprintf(hex + i * 2, "%02x", bytes[i]);
    }
    
    snprintf(serial, len, "%s", hex);
}

int validate_cert_request(cert_request_t *request, ca_config_t *config) {
    if (strlen(request->common_name) == 0) return -1;
    if (strlen(request->organization) == 0) return -1;
    if (strlen(request->country) == 0) return -1;
    if (request->key_size < config->min_key_size) return -1;
    if (request->validity_days <= 0) return -1;
    if (request->validity_days > config->max_validity_days) return -1;
    
    // Validate country code
    if (validate_country(request->country, config->allowed_countries) != 0) {
        return -1;
    }
    
    return 0;
}

void print_cert_info(X509 *cert) {
    char *subject = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
    char *issuer = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
    
    printf("Subject: %s\n", subject);
    printf("Issuer: %s\n", issuer);
    
    free(subject);
    free(issuer);
}

int load_ca_certificate(const char *path, X509 **cert) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    
    *cert = PEM_read_X509(fp, NULL, NULL, NULL);
    fclose(fp);
    
    return (*cert) ? 0 : -1;
}

int load_ca_private_key(const char *path, EVP_PKEY **key) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;
    
    *key = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    
    return (*key) ? 0 : -1;
}
