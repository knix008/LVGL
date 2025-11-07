# Makefile for Korean IME Tests
# This Makefile builds the Korean IME test programs

include Rules.make

# Compiler and flags
# Try to use system MiniGUI first, fall back to local installation
MINIGUI_CFLAGS := $(shell pkg-config --cflags minigui 2>/dev/null || echo "-I$(MINIGUI_ROOT_DIR)/include")
MINIGUI_LDFLAGS := $(shell pkg-config --libs minigui 2>/dev/null || echo "-L$(MINIGUI_ROOT_DIR)/lib $(MINIGUILIB)")

# Include paths
INCLUDES = -I. -I./ime -I./ime/libime $(MINIGUI_CFLAGS)

# Compiler flags
CFLAGS = -Wall -Wextra -g $(INCLUDES)
# Suppress unused parameter warnings for Korean IME callback functions
KOREAN_CFLAGS = $(CFLAGS) -Wno-unused-parameter
LDFLAGS = -lm $(MINIGUI_LDFLAGS)

# Source files for Korean IME library (only the Korean IME, not the full framework)
KOREAN_IME_SRCS = ime/libime/ime_korean.c

# Test targets
all: test_korean_simple test_korean_input

# Build simple test (aliased from test_simple)
test_simple: test_korean_simple

test_korean_simple.o: test_korean_simple.c
	$(CC) $(CFLAGS) -c -o $@ $<

ime_korean.o: ime/libime/ime_korean.c
	$(CC) $(KOREAN_CFLAGS) -c -o $@ $<

test_korean_simple: test_korean_simple.o ime_korean.o
	$(CC) -o $@ $^ $(LDFLAGS)

# Build comprehensive test (aliased from test_korean)
test_korean: test_korean_input

test_korean_input.o: test_korean_input.c
	$(CC) $(CFLAGS) -c -o $@ $<

test_korean_input: test_korean_input.o ime_korean.o
	$(CC) -o $@ $^ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f test_korean_simple test_korean_input *.o ime_korean.o test_korean_simple.o test_korean_input.o

# Phony targets
.PHONY: all clean
