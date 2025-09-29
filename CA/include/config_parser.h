#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ca_server.h"

// Configuration structure
typedef struct {
    char ca_cert_path[MAX_PATH_LEN];
    char ca_key_path[MAX_PATH_LEN];
    char crl_path[MAX_PATH_LEN];
    char db_path[MAX_PATH_LEN];
    char certs_dir[MAX_PATH_LEN];
    char keys_dir[MAX_PATH_LEN];
    int port;
    int key_size;
    int validity_years;
    int min_key_size;
    int max_validity_days;
    char allowed_countries[256];
    char log_level[16];
    char log_file[MAX_PATH_LEN];
} config_t;

// Configuration functions
int load_config(const char *config_file, config_t *config);
int parse_config_line(const char *line, config_t *config);
void set_default_config(config_t *config);
int validate_country(const char *country, const char *allowed_countries);
int validate_validity_days(int validity_days, int max_validity_days);

#endif // CONFIG_PARSER_H
