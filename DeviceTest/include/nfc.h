#ifndef NFC_H
#define NFC_H

#include "common.h"
#include <stdint.h>

// NFC interface types
typedef enum {
    NFC_INTERFACE_UNKNOWN = 0,
    NFC_INTERFACE_PCSC,      // PC/SC interface
    NFC_INTERFACE_LIBNFC,    // libnfc interface
    NFC_INTERFACE_LIBNFCX,   // libnfcx interface
    NFC_INTERFACE_SERIAL,    // Serial interface
    NFC_INTERFACE_USB        // USB interface
} nfc_interface_t;

// NFC card types
typedef enum {
    NFC_CARD_UNKNOWN = 0,
    NFC_CARD_MIFARE_CLASSIC,
    NFC_CARD_MIFARE_ULTRA,
    NFC_CARD_MIFARE_DESFIRE,
    NFC_CARD_ISO14443_A,
    NFC_CARD_ISO14443_B,
    NFC_CARD_ISO15693,
    NFC_CARD_FELICA,
    NFC_CARD_ISO7816,
    NFC_CARD_NDEF
} nfc_card_type_t;

// NFC operation modes
typedef enum {
    NFC_MODE_UNKNOWN = 0,
    NFC_MODE_READER_WRITER,
    NFC_MODE_CARD_EMULATION,
    NFC_MODE_P2P_INITIATOR,
    NFC_MODE_P2P_TARGET
} nfc_mode_t;

// NFC test structure
typedef struct {
    bool is_initialized;
    nfc_interface_t interface;
    char device_name[64];
    char serial_number[64];
    char firmware_version[32];
    char manufacturer[32];
    bool is_powered;
    bool is_connected;
    nfc_mode_t supported_modes[4];
    int supported_modes_count;
    nfc_card_type_t supported_cards[10];
    int supported_cards_count;
    int card_count;
    nfc_card_type_t detected_cards[10];
    char card_uid[10][16];  // UID for each detected card
} nfc_test_t;

// Function declarations
#ifdef __cplusplus
extern "C" {
#endif

// Initialize NFC test
int init_nfc_test(nfc_test_t* nfc);

// Cleanup NFC test
void cleanup_nfc_test(nfc_test_t* nfc);

// Individual test functions
test_result_t test_nfc_detection(nfc_test_t* nfc);
test_result_t test_nfc_device(nfc_test_t* nfc);
test_result_t test_nfc_power(nfc_test_t* nfc);
test_result_t test_nfc_card_detection(nfc_test_t* nfc);
test_result_t test_nfc_read_write(nfc_test_t* nfc);
test_result_t test_nfc_card_types(nfc_test_t* nfc);
test_result_t test_nfc_communication(nfc_test_t* nfc);
test_result_t test_nfc_security(nfc_test_t* nfc);
test_result_t test_nfc_range(nfc_test_t* nfc);
test_result_t test_nfc_all_capabilities(nfc_test_t* nfc);

// Run all NFC tests
test_summary_t run_all_nfc_tests(void);

// Handle NFC commands
int handle_nfc_commands(const char* test_type, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // NFC_H
