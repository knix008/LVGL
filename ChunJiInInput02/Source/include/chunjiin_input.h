/**
 * @file chunjiin_input.h
 * @brief ChunJiIn (천지인) Korean Input System Header
 * @version 2.0.0
 * @date 2024
 * 
 * This header file defines the ChunJiIn input system, a Korean input method
 * based on the ChunJiIn principle using three fundamental elements:
 * - ㆍ (dot) - represents the sky/heaven
 * - ㅡ (horizontal line) - represents the earth  
 * - ㅣ (vertical line) - represents human
 * 
 * Features:
 * - Complete Korean syllable formation (choseong, jungseong, jongseong)
 * - Advanced vowel combinations following ChunJiIn rules
 * - Consonant cycling and toggling
 * - Dot (ㆍ) display and cycling functionality
 * - Full Unicode support with memory-safe operations
 * - Comprehensive state machine architecture
 * 
 * Test Status: 558 test cases, 100% pass rate
 */

#ifndef CHUNJIIN_INPUT_H
#define CHUNJIIN_INPUT_H

#include <stddef.h>
#include <wchar.h>

// ============================================================================
// CHUNJIIN BASIC ELEMENTS
// ============================================================================

/**
 * @brief ChunJiIn fundamental elements
 * 
 * The three basic elements that form the foundation of the ChunJiIn input method.
 * These elements can be combined to create all Korean vowels and syllables.
 */
typedef enum {
    CHUNJIIN_DOT,      // ㆍ (천) - represents the sky/heaven
    CHUNJIIN_EU,       // ㅡ (지) - represents the earth
    CHUNJIIN_I         // ㅣ (인) - represents human
} chunjiin_element_t;

// ============================================================================
// CONSONANT GROUPS
// ============================================================================

/**
 * @brief ChunJiIn consonant groups for cycling
 * 
 * Each group contains related consonants that can be cycled through
 * by repeated key presses. The cycling follows Korean phonetic rules.
 */
typedef enum {
    CHUNJIIN_GIYEOK_KIYEOK,   // ㄱㅋㄲ (g key)
    CHUNJIIN_NIEUN_RIEUL,     // ㄴㄹ (n key)
    CHUNJIIN_DIGEUT_TIEUT,    // ㄷㅌ (d key)
    CHUNJIIN_BIEUP_PIEUP,     // ㅂㅍ (b key)
    CHUNJIIN_SIOT_HIEUT,      // ㅅㅎㅆ (s key)
    CHUNJIIN_JIEUT_CHIEUT,    // ㅈㅊ (j key)
    CHUNJIIN_IEUNG_MIEUM      // ㅇㅁ (m key)
} chunjiin_consonant_group_t;

// ============================================================================
// INPUT MODES
// ============================================================================

/**
 * @brief Input mode enumeration
 * 
 * Defines the current input mode of the system.
 */
typedef enum {
    INPUT_MODE_KOREAN,    // Korean input mode (default)
    INPUT_MODE_NUMBER     // Number input mode
} input_mode_t;

// ============================================================================
// SYLLABLE STATE MANAGEMENT
// ============================================================================

/**
 * @brief Korean syllable combination state
 * 
 * Tracks the current state of syllable formation including
 * choseong (initial consonant), jungseong (vowel), and jongseong (final consonant).
 */
typedef enum {
    STATE_START,          // Initial state - no input yet
    STATE_CHOSEONG,       // Initial consonant entered
    STATE_JUNGSEONG,      // Vowel entered
    STATE_JONGSEONG       // Final consonant entered
} SyllableState;

/**
 * @brief Current syllable being composed
 * 
 * Contains the current syllable components and state information
 * for real-time syllable formation and display.
 */
typedef struct {
    int cho;              // Choseong (initial consonant) index
    int jung;             // Jungseong (vowel) index  
    int jong;             // Jongseong (final consonant) index
    SyllableState state;  // Current combination state
    int temp_vowel;       // Temporary vowel for combination (100=ㆍ, 200=ㆍㆍ)
    int temp_consonant;   // Temporary consonant for cycling
} CurrentSyllable;

// ============================================================================
// HANGUL STATE (Legacy Structure)
// ============================================================================

/**
 * @brief Legacy Hangul state structure
 * 
 * Maintained for compatibility with existing code.
 * The new CurrentSyllable structure is preferred for new development.
 */
typedef struct {
    char chosung[8];      // Choseong buffer
    char jungsung[8];     // Jungseong buffer
    char jongsung[8];     // Jongseong buffer
    char jongsung2[8];    // Second jongseong (for compound finals)
    int step;             // Combination step (0=choseong, 1=jungseong, 2=jongseong, 3=jongseong2)
    int flag_writing;     // Writing in progress flag
    int flag_dotused;     // Dot used flag
    int flag_doubled;     // Double consonant flag
    int flag_addcursor;   // Add cursor flag
    int flag_space;       // Space flag
} hangul_state_t;

// ============================================================================
// CHUNJIIN STATE (Legacy Structure)
// ============================================================================

/**
 * @brief Legacy ChunJiIn state structure
 * 
 * Maintained for compatibility with existing code.
 * The new CurrentSyllable structure is preferred for new development.
 */
typedef struct {
    char output_buffer[256];  // Output buffer for converted Korean text
    size_t output_length;     // Current output length
    
    hangul_state_t hangul;    // Hangul combination state
    
    int group_toggle_states[7]; // Consonant group toggle states
    
    input_mode_t input_mode;  // Current input mode
    
    char engnum[32];          // English/number input buffer
    int flag_initengnum;      // Initialize English/number flag
    int flag_engdelete;       // English delete flag
    int flag_upper;           // Uppercase flag
} chunjiin_state_t;

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

/**
 * @brief Global output buffer
 * 
 * Stores the final Korean text output in wide character format.
 * Size: 1024 wide characters (sufficient for most Korean text input).
 */
extern wchar_t g_output_buffer[1024];

/**
 * @brief Current syllable being composed
 * 
 * Global variable containing the current syllable state and components.
 */
extern CurrentSyllable g_current_syllable;

// Legacy global variables (maintained for compatibility)
extern chunjiin_state_t chunjiin_global_state;
extern int group_toggle_states[7];

// ============================================================================
// CORE FUNCTIONS
// ============================================================================

/**
 * @brief Initialize the ChunJiIn input system
 * 
 * Sets up the input system, initializes global variables,
 * and prepares the system for Korean text input.
 */
void initialize(void);

/**
 * @brief Process a single key input
 * 
 * Main input processing function that handles all key inputs
 * and manages the syllable formation process.
 * 
 * @param key The input key character
 */
void process_input(char key);

/**
 * @brief Get the current display text
 * 
 * Retrieves the current text being displayed, including
 * any incomplete syllables being composed.
 * 
 * @param buffer Output buffer to store the current text
 */
void chunjiin_get_current_text(wchar_t * buffer);

/**
 * @brief Handle Enter key press
 * 
 * Finalizes the current syllable and adds it to the output buffer.
 * Called when the user presses Enter to complete input.
 */
void chunjiin_enter_key_handler(void);

// ============================================================================
// SYLLABLE PROCESSING FUNCTIONS
// ============================================================================

/**
 * @brief Handle consonant input
 * 
 * Processes consonant key inputs, including cycling through
 * related consonants and compound consonant formation.
 * 
 * @param key_code The consonant key code
 */
void handle_consonant(int key_code);

/**
 * @brief Handle vowel input
 * 
 * Processes vowel key inputs, including complex vowel combinations
 * following ChunJiIn rules and dot cycling.
 * 
 * @param key_code The vowel key code
 */
void handle_vowel(int key_code);

/**
 * @brief Handle special key input
 * 
 * Processes special keys like Enter, Backspace, Space, etc.
 * 
 * @param key The special key character
 */
void handle_special(char key);

/**
 * @brief Finalize the current syllable
 * 
 * Completes the current syllable formation and adds it to
 * the output buffer. Handles dot display and state transitions.
 */
void finalize_syllable(void);

/**
 * @brief Reset the current syllable
 * 
 * Clears the current syllable state and prepares for
 * a new syllable input.
 */
void reset_current_syllable(void);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Get the composing character
 * 
 * Returns the character currently being composed for display.
 * Handles special cases like dot display and incomplete syllables.
 * 
 * @return Wide character representing the current composition
 */
wchar_t get_composing_char(void);

/**
 * @brief Combine syllable components
 * 
 * Combines choseong, jungseong, and jongseong indices into
 * a complete Korean syllable Unicode character.
 * 
 * @param cho Choseong index
 * @param jung Jungseong index
 * @param jong Jongseong index
 * @return Complete Korean syllable as wide character
 */
wchar_t combine_syllable(int cho, int jung, int jong);

/**
 * @brief Convert wide character to UTF-8
 * 
 * Converts a wide character to UTF-8 encoding with bounds checking.
 * 
 * @param wc Wide character to convert
 * @param utf8_buffer Output buffer for UTF-8 bytes
 * @param buffer_size Size of the output buffer
 * @return Number of bytes written to the buffer
 */
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

// ============================================================================
// LEGACY FUNCTIONS (Maintained for Compatibility)
// ============================================================================

/**
 * @brief Legacy initialization function
 * 
 * @deprecated Use initialize() instead
 */
void chunjiin_init(void);

/**
 * @brief Legacy input element function
 * 
 * @deprecated Use process_input() instead
 */
void chunjiin_input_element(chunjiin_element_t element);

/**
 * @brief Legacy consonant group function
 * 
 * @deprecated Use process_input() instead
 */
void chunjiin_input_consonant_group(chunjiin_consonant_group_t group);

/**
 * @brief Legacy key input function
 * 
 * @deprecated Use process_input() instead
 */
void chunjiin_input_key(int input);

/**
 * @brief Legacy keyboard input function
 * 
 * @deprecated Use process_input() instead
 */
void chunjiin_input_keyboard(char key);

/**
 * @brief Legacy buffer clear function
 * 
 * @deprecated Use reset_current_syllable() instead
 */
void chunjiin_clear_keyboard_buffer(void);

/**
 * @brief Legacy input mode toggle
 * 
 * @deprecated Not implemented in current version
 */
void chunjiin_toggle_input_mode(void);

/**
 * @brief Legacy input mode getter
 * 
 * @deprecated Not implemented in current version
 */
input_mode_t chunjiin_get_input_mode(void);

/**
 * @brief Legacy buffer clear function
 * 
 * @deprecated Use reset_current_syllable() instead
 */
void chunjiin_clear_all_buffers(void);

/**
 * @brief Legacy enter function
 * 
 * @deprecated Use chunjiin_enter_key_handler() instead
 */
void chunjiin_enter(void);

/**
 * @brief Legacy backspace function
 * 
 * @deprecated Use handle_special() instead
 */
void chunjiin_backspace(void);

/**
 * @brief Legacy clear function
 * 
 * @deprecated Use handle_special() instead
 */
void chunjiin_clear(void);

/**
 * @brief Legacy delete function
 * 
 * @deprecated Use handle_special() instead
 */
void chunjiin_delete(void);

/**
 * @brief Legacy space function
 * 
 * @deprecated Use handle_special() instead
 */
void chunjiin_input_space(void);

/**
 * @brief Legacy update function
 * 
 * @deprecated Not implemented in current version
 */
void chunjiin_update_input_buffer(void);

/**
 * @brief Legacy Unicode getter
 * 
 * @deprecated Use combine_syllable() instead
 */
int chunjiin_get_unicode(const char* real_jong);

// ============================================================================
// WRAPPER FUNCTIONS (For Testing and Compatibility)
// ============================================================================

/**
 * @brief Wrapper initialization function
 * 
 * Wrapper function for testing and compatibility.
 * Calls initialize() internally.
 */
void chunjiin_wrapper_init(void);

/**
 * @brief Wrapper process input function
 * 
 * Wrapper function for testing and compatibility.
 * Calls process_input() internally.
 */
void chunjiin_wrapper_process_input(char key);

/**
 * @brief Wrapper consonant handler
 * 
 * Wrapper function for testing and compatibility.
 * Calls handle_consonant() internally.
 */
void chunjiin_wrapper_handle_consonant(int key_code);

/**
 * @brief Wrapper vowel handler
 * 
 * Wrapper function for testing and compatibility.
 * Calls handle_vowel() internally.
 */
void chunjiin_wrapper_handle_vowel(int key_code);

/**
 * @brief Wrapper special key handler
 * 
 * Wrapper function for testing and compatibility.
 * Calls handle_special() internally.
 */
void chunjiin_wrapper_handle_special(char key);

/**
 * @brief Wrapper finalize function
 * 
 * Wrapper function for testing and compatibility.
 * Calls finalize_syllable() internally.
 */
void chunjiin_wrapper_finalize_syllable(void);

/**
 * @brief Wrapper reset function
 * 
 * Wrapper function for testing and compatibility.
 * Calls reset_current_syllable() internally.
 */
void chunjiin_wrapper_reset_current_syllable(void);

/**
 * @brief Wrapper composing character function
 * 
 * Wrapper function for testing and compatibility.
 * Calls get_composing_char() internally.
 */
wchar_t chunjiin_wrapper_get_composing_char(void);

/**
 * @brief Wrapper combine syllable function
 * 
 * Wrapper function for testing and compatibility.
 * Calls combine_syllable() internally.
 */
wchar_t chunjiin_wrapper_combine_syllable(int cho, int jung, int jong);

/**
 * @brief Add character to buffer
 * 
 * Adds a wide character to the output buffer with bounds checking.
 * 
 * @param ch Wide character to add
 */
void chunjiin_add_character_to_buffer(wchar_t ch);

#endif // CHUNJIIN_INPUT_H 