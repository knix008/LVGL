# Makefile for Audio Player Tests

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -D_DEFAULT_SOURCE
LDFLAGS = -lm -lpthread

# SDL2_mixer for audio tests
SDL_MIXER_CFLAGS = $(shell pkg-config --cflags SDL2_mixer 2>/dev/null || echo "-I/usr/include/SDL2")
SDL_MIXER_LIBS = $(shell pkg-config --libs SDL2_mixer 2>/dev/null || echo "-lSDL2_mixer -lSDL2")

# Parent directory
PARENT_DIR = ..

# Include paths
INCLUDES = -I$(PARENT_DIR)

# Combine flags
ALL_CFLAGS = $(CFLAGS) $(INCLUDES) $(SDL_MIXER_CFLAGS)
ALL_LIBS = $(SDL_MIXER_LIBS) $(LDFLAGS)

# Test executables
TESTS = test_audio_player test_utils

all: $(TESTS)
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║          Tests compiled successfully!                     ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Run tests with: make test"
	@echo ""

test_audio_player: test_audio_player.o $(PARENT_DIR)/audio_player.o mock_gui.o
	@echo "Linking $@..."
	$(CC) -o $@ $^ $(ALL_LIBS)

test_utils: test_utils.o
	@echo "Linking $@..."
	$(CC) -o $@ $^ $(LDFLAGS)

test_audio_player.o: test_audio_player.c $(PARENT_DIR)/audio_player.h
	@echo "Compiling $<..."
	$(CC) $(ALL_CFLAGS) -c test_audio_player.c

test_utils.o: test_utils.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c test_utils.c

mock_gui.o: mock_gui.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c mock_gui.c

$(PARENT_DIR)/audio_player.o: $(PARENT_DIR)/audio_player.c $(PARENT_DIR)/audio_player.h
	@echo "Compiling parent audio_player.c..."
	@cd $(PARENT_DIR) && $(MAKE) audio_player.o

# Run all tests
test: all
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║          RUNNING ALL TESTS                                 ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		./$$test; \
		if [ $$? -ne 0 ]; then \
			echo ""; \
			echo "Test $$test FAILED!"; \
			exit 1; \
		fi; \
	done
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║          ALL TESTS PASSED!                                 ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""

# Run individual tests
test-audio: test_audio_player
	@echo "Running audio player tests..."
	@./test_audio_player

test-utils: test_utils
	@echo "Running utility tests..."
	@./test_utils

clean:
	rm -f $(TESTS) *.o

help:
	@echo "Audio Player Test Suite"
	@echo "======================="
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build all tests"
	@echo "  test       - Run all tests"
	@echo "  test-audio - Run audio player tests only"
	@echo "  test-utils - Run utility tests only"
	@echo "  clean      - Remove built files"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Tests:"
	@echo "  test_audio_player - Tests for audio playback backend"
	@echo "  test_utils        - Tests for utility functions"

.PHONY: all test test-audio test-utils clean help

