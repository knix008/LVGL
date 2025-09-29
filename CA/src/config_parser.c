#include "../include/config_parser.h"

void set_default_config(config_t *config) {
    strcpy(config->ca_cert_path, "ca.crt");
    strcpy(config->ca_key_path, "ca.key");
    strcpy(config->crl_path, "ca.crl");
    strcpy(config->db_path, "ca.db");
    strcpy(config->certs_dir, "certs");
    strcpy(config->keys_dir, "keys");
    config->port = 8080;
    config->key_size = 4096;
    config->validity_years = 10;
    config->min_key_size = 2048;
    config->max_validity_days = 365;
    strcpy(config->allowed_countries, "");
    strcpy(config->log_level, "INFO");
    strcpy(config->log_file, "ca.log");
}

int parse_config_line(const char *line, config_t *config) {
    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
        return 0;
    }
    
    // Find the equals sign
    char *equals = strchr(line, '=');
    if (!equals) {
        return 0;
    }
    
    // Extract key and value
    char key[64], value[256];
    size_t key_len = equals - line;
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    
    // Trim whitespace from key
    while (key_len > 0 && isspace(key[key_len - 1])) {
        key[--key_len] = '\0';
    }
    
    strcpy(value, equals + 1);
    
    // Trim whitespace from value
    char *start = value;
    while (*start && isspace(*start)) start++;
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) end--;
    end[1] = '\0';
    strcpy(value, start);
    
    // Parse configuration values
    if (strcmp(key, "ca_cert_path") == 0) {
        strncpy(config->ca_cert_path, value, sizeof(config->ca_cert_path) - 1);
    } else if (strcmp(key, "ca_key_path") == 0) {
        strncpy(config->ca_key_path, value, sizeof(config->ca_key_path) - 1);
    } else if (strcmp(key, "crl_path") == 0) {
        strncpy(config->crl_path, value, sizeof(config->crl_path) - 1);
    } else if (strcmp(key, "db_path") == 0) {
        strncpy(config->db_path, value, sizeof(config->db_path) - 1);
    } else if (strcmp(key, "certs_dir") == 0) {
        strncpy(config->certs_dir, value, sizeof(config->certs_dir) - 1);
    } else if (strcmp(key, "keys_dir") == 0) {
        strncpy(config->keys_dir, value, sizeof(config->keys_dir) - 1);
    } else if (strcmp(key, "port") == 0) {
        config->port = atoi(value);
    } else if (strcmp(key, "key_size") == 0) {
        config->key_size = atoi(value);
    } else if (strcmp(key, "validity_years") == 0) {
        config->validity_years = atoi(value);
    } else if (strcmp(key, "min_key_size") == 0) {
        config->min_key_size = atoi(value);
    } else if (strcmp(key, "max_validity_days") == 0) {
        config->max_validity_days = atoi(value);
    } else if (strcmp(key, "allowed_countries") == 0) {
        strncpy(config->allowed_countries, value, sizeof(config->allowed_countries) - 1);
    } else if (strcmp(key, "log_level") == 0) {
        strncpy(config->log_level, value, sizeof(config->log_level) - 1);
    } else if (strcmp(key, "log_file") == 0) {
        strncpy(config->log_file, value, sizeof(config->log_file) - 1);
    }
    
    return 0;
}

int load_config(const char *config_file, config_t *config) {
    // Set default values first
    set_default_config(config);
    
    FILE *fp = fopen(config_file, "r");
    if (!fp) {
        printf("Warning: Could not open config file '%s', using defaults\n", config_file);
        return 0;
    }
    
    char line[512];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        
        // Remove trailing newline
        line[strcspn(line, "\r\n")] = '\0';
        
        if (parse_config_line(line, config) != 0) {
            printf("Warning: Error parsing line %d in config file\n", line_num);
        }
    }
    
    fclose(fp);
    printf("Configuration loaded from '%s'\n", config_file);
    return 0;
}

int validate_country(const char *country, const char *allowed_countries) {
    // If no restrictions, allow all countries
    if (strlen(allowed_countries) == 0) {
        return 0;
    }
    
    // Check if the country is in the allowed list
    char *countries_copy = strdup(allowed_countries);
    char *token = strtok(countries_copy, ",");
    
    while (token != NULL) {
        // Trim whitespace from token
        char *start = token;
        while (*start && isspace(*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace(*end)) end--;
        end[1] = '\0';
        
        if (strcmp(start, country) == 0) {
            free(countries_copy);
            return 0; // Country is allowed
        }
        
        token = strtok(NULL, ",");
    }
    
    free(countries_copy);
    return -1; // Country is not allowed
}

int validate_validity_days(int validity_days, int max_validity_days) {
    if (validity_days <= 0) {
        return -1; // Invalid: must be positive
    }
    
    if (validity_days > max_validity_days) {
        return -1; // Invalid: exceeds maximum
    }
    
    return 0; // Valid
}
