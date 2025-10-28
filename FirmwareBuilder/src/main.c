#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "firmware_builder.h"

#define VERSION "1.0.0"

typedef struct {
    char *source;
    char *output;
    int compress;
    int compression_level;
    int add_crc;
    int encrypt;
    char *password;
    int generate_hash;
    int verify;
    int extract;
    int help;
    int version;
} cli_options_t;

void print_help(const char *program)
{
    printf("Firmware Builder v%s\n\n", VERSION);
    printf("Usage: %s [OPTIONS] <source>\n\n", program);
    printf("Build firmware packages with compression, CRC, encryption, and hashing.\n\n");

    printf("Options:\n");
    printf("  -s, --source <path>           Source directory or file\n");
    printf("  -o, --output <dir>            Output directory (default: ./firmware-build)\n");
    printf("  -c, --compress [true|false]   Enable compression (default: true)\n");
    printf("  -l, --level <0-9>             Compression level (default: 6)\n");
    printf("  -r, --crc [true|false]        Add CRC checksum (default: true)\n");
    printf("  -e, --encrypt                 Enable encryption\n");
    printf("  -p, --password <pwd>          Encryption password\n");
    printf("  -g, --hash [true|false]       Generate hash (default: true)\n");
    printf("  -v, --verify                  Verify firmware after build\n");
    printf("  -x, --extract                 Extract firmware\n");
    printf("  -h, --help                    Show this help message\n");
    printf("  --version                     Show version\n\n");

    printf("Examples:\n");
    printf("  %s -s ./firmware -o ./build\n", program);
    printf("  %s -s ./firmware -o ./build --encrypt -p mypassword\n", program);
    printf("  %s --extract -s firmware.enc -p mypassword\n", program);
}

void print_version(void)
{
    printf("Firmware Builder v%s\n", VERSION);
}

void parse_arguments(int argc, char *argv[], cli_options_t *opts)
{
    memset(opts, 0, sizeof(cli_options_t));
    opts->compress = 1;
    opts->compression_level = 6;
    opts->add_crc = 1;
    opts->generate_hash = 1;
    opts->output = "./firmware-build";

    struct option long_options[] = {
        {"source", required_argument, 0, 's'},
        {"output", required_argument, 0, 'o'},
        {"compress", optional_argument, 0, 'c'},
        {"level", required_argument, 0, 'l'},
        {"crc", optional_argument, 0, 'r'},
        {"encrypt", no_argument, 0, 'e'},
        {"password", required_argument, 0, 'p'},
        {"hash", optional_argument, 0, 'g'},
        {"verify", no_argument, 0, 'v'},
        {"extract", no_argument, 0, 'x'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "s:o:c::l:r::ep:g::vxhV", long_options, &option_index)) != -1) {
        switch (c) {
            case 's':
                opts->source = optarg;
                break;
            case 'o':
                opts->output = optarg;
                break;
            case 'c':
                if (optarg && strcmp(optarg, "false") == 0) {
                    opts->compress = 0;
                }
                break;
            case 'l':
                opts->compression_level = atoi(optarg);
                if (opts->compression_level < 0 || opts->compression_level > 9) {
                    opts->compression_level = 6;
                }
                break;
            case 'r':
                if (optarg && strcmp(optarg, "false") == 0) {
                    opts->add_crc = 0;
                }
                break;
            case 'e':
                opts->encrypt = 1;
                break;
            case 'p':
                opts->password = optarg;
                break;
            case 'g':
                if (optarg && strcmp(optarg, "false") == 0) {
                    opts->generate_hash = 0;
                }
                break;
            case 'v':
                opts->verify = 1;
                break;
            case 'x':
                opts->extract = 1;
                break;
            case 'h':
                opts->help = 1;
                break;
            case 'V':
                opts->version = 1;
                break;
            default:
                opts->help = 1;
        }
    }

    /* Get source from positional argument if not specified */
    if (!opts->source && optind < argc) {
        opts->source = argv[optind];
    }
}

int main(int argc, char *argv[])
{
    cli_options_t opts;
    parse_arguments(argc, argv, &opts);

    if (opts.help) {
        print_help(argv[0]);
        return 0;
    }

    if (opts.version) {
        print_version();
        return 0;
    }

    if (!opts.source) {
        fprintf(stderr, "Error: Source path is required\n");
        fprintf(stderr, "Use --help for usage information\n");
        return 1;
    }

    if (opts.extract) {
        /* Extract mode */
        printf("🔓 Extracting firmware...\n");
        if (fw_builder_extract(opts.source, opts.output, opts.password) != 0) {
            fprintf(stderr, "❌ Extraction failed\n");
            return 1;
        }
        printf("✅ Extraction completed\n");
    } else {
        /* Build mode */
        printf("🔨 Firmware Builder v%s\n\n", VERSION);

        fw_build_options_t build_opts = {
            .compress = opts.compress,
            .compression_level = opts.compression_level,
            .add_crc = opts.add_crc,
            .encrypt = opts.encrypt,
            .encryption_password = opts.password,
            .generate_hash = opts.generate_hash
        };

        fw_package_t *pkg = fw_builder_create(opts.source, opts.output, build_opts);
        if (!pkg) {
            fprintf(stderr, "❌ Failed to create firmware builder\n");
            return 1;
        }

        if (fw_builder_build(pkg) != 0) {
            fprintf(stderr, "❌ Build failed\n");
            fw_builder_free(pkg);
            return 1;
        }

        fw_builder_print_package(pkg);

        if (opts.verify) {
            if (fw_builder_verify(pkg) != 0) {
                fprintf(stderr, "❌ Verification failed\n");
                fw_builder_free(pkg);
                return 1;
            }
        }

        fw_builder_free(pkg);
        printf("✅ Build completed successfully\n");
    }

    return 0;
}
