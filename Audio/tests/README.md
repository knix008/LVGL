# Audio Player Test Suite

This directory contains unit tests for the audio player application.

## Test Files

### test_audio_player.c
Tests for the audio player backend (`audio_player.c`):
- Initialization and cleanup
- Volume control
- File loading (valid/invalid files)
- State transitions (STOPPED → PLAYING → PAUSED → STOPPED)
- Metadata extraction
- Seek functionality
- Multiple file loads

### test_utils.c
Tests for utility functions:
- Audio file detection (.mp3, .wav)
- Directory scanning
- Time formatting (MM:SS)
- File path construction

## Building Tests

```bash
cd tests
make
```

## Running Tests

### Run all tests:
```bash
make test
```

### Run individual test suites:
```bash
make test-audio    # Audio player tests only
make test-utils    # Utility tests only
```

### Run tests manually:
```bash
./test_audio_player
./test_utils
```

## Test Output

Tests produce colorized output:
- 🔵 BLUE - Test running
- 🟢 GREEN - Test passed
- 🔴 RED - Test failed
- 🟡 YELLOW - Test skipped (e.g., missing test files)

Example output:
```
╔════════════════════════════════════════════════════════════╗
║          AUDIO PLAYER UNIT TEST SUITE                     ║
╚════════════════════════════════════════════════════════════╝

TEST 1: Audio player initialization ... PASS
TEST 2: Volume control ... PASS
TEST 3: Initial player state ... PASS
TEST 4: Load valid audio file ... PASS
...

════════════════════════════════════════════════════════════
TEST SUMMARY
════════════════════════════════════════════════════════════
Total tests:  11
Passed:       11
Failed:       0
Success rate: 100.0%
════════════════════════════════════════════════════════════

All tests PASSED!
```

## Requirements

- Test audio files in `../assets/music/`
- SDL2_mixer library installed
- All audio player source files compiled

## Continuous Integration

Tests return:
- Exit code 0 on success (all tests passed)
- Exit code 1 on failure (any test failed)

This makes them suitable for CI/CD pipelines.

## Adding New Tests

To add a new test:

1. Create a new test function:
```c
static void test_my_feature(void)
{
    TEST_START("My feature description");
    setup_test();
    
    // Your test code here
    ASSERT(condition, "Error message");
    
    teardown_test();
    TEST_PASS();
}
```

2. Add it to `run_all_tests()`:
```c
void run_all_tests(void)
{
    // ... existing tests ...
    test_my_feature();
}
```

3. Rebuild and run:
```bash
make clean
make test
```

## Test Coverage

Current test coverage:

**Audio Player Backend:**
- ✅ Initialization
- ✅ Volume control
- ✅ State management
- ✅ File loading
- ✅ Metadata extraction
- ✅ Playback control
- ✅ Seek functionality

**Utilities:**
- ✅ File type detection
- ✅ Directory scanning
- ✅ Time formatting
- ✅ Path construction

## Known Limitations

1. Some tests require actual audio files in `assets/music/`
2. Tests skip if required files are not found
3. GUI components are not tested (require LVGL display)
4. Audio playback timing is not precisely tested (platform-dependent)

## Troubleshooting

**Tests skip due to missing files:**
- Run `../generate_test_audio.py` to create test files
- Ensure `assets/music/` directory exists with WAV files

**SDL2_mixer not found:**
- Install SDL2_mixer: `sudo apt-get install libsdl2-mixer-dev`

**Compilation errors:**
- Ensure parent `audio_player.c` is compiled first
- Check include paths in Makefile

