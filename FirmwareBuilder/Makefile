CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -fPIC
LDFLAGS = -lz -lcrypto -lssl

# GTK flags (optional)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include
DOWNLOADER_DIR = downloader

# Source files
CORE_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/firmware_builder.c
MODULE_SRCS = $(wildcard $(SRC_DIR)/modules/*.c)
DOWNLOADER_SRCS = $(wildcard $(DOWNLOADER_DIR)/*.c)
GUI_SRC = $(SRC_DIR)/gui.c

CORE_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CORE_SRCS))
MODULE_OBJS = $(patsubst $(SRC_DIR)/modules/%.c,$(BUILD_DIR)/modules/%.o,$(MODULE_SRCS))
DOWNLOADER_OBJS = $(patsubst $(DOWNLOADER_DIR)/%.c,$(BUILD_DIR)/downloader/%.o,$(DOWNLOADER_SRCS))
GUI_OBJ = $(BUILD_DIR)/gui.o

# Targets
TARGET_CLI = $(BIN_DIR)/firmware-builder
TARGET_GUI = $(BIN_DIR)/firmware-builder-gui
TARGET_LIB = $(BIN_DIR)/libfirmware.a

.PHONY: all cli gui lib clean help deps install test tests

all: cli lib

cli: $(BUILD_DIR) $(BIN_DIR) $(TARGET_CLI)

gui: $(BUILD_DIR) $(BIN_DIR) $(TARGET_GUI)

lib: $(BUILD_DIR) $(BIN_DIR) $(TARGET_LIB)

test: tests

tests: all
	@echo "Running tests..."
	@tests/run_tests.sh

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/modules $(BUILD_DIR)/downloader

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/modules/%.o: $(SRC_DIR)/modules/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/downloader/%.o: $(DOWNLOADER_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/gui.o: $(GUI_SRC)
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET_CLI): $(CORE_OBJS) $(MODULE_OBJS) $(DOWNLOADER_OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)
	@echo "✓ Built: $(TARGET_CLI)"

$(TARGET_GUI): $(GUI_OBJ) $(MODULE_OBJS) $(DOWNLOADER_OBJS) $(BUILD_DIR)/firmware_builder.o
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -I$(INCLUDE_DIR) $^ -o $@ $(LDFLAGS) $(GTK_LIBS)
	@echo "✓ Built: $(TARGET_GUI)"

$(TARGET_LIB): $(MODULE_OBJS) $(DOWNLOADER_OBJS) $(BUILD_DIR)/firmware_builder.o
	ar rcs $@ $^
	@echo "✓ Built: $(TARGET_LIB)"

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Cleaned"

deps:
	@echo "Checking dependencies..."
	@pkg-config --exists zlib && echo "✓ zlib found" || echo "✗ zlib required"
	@pkg-config --exists openssl && echo "✓ OpenSSL found" || echo "✗ OpenSSL required"
	@pkg-config --exists gtk+-3.0 && echo "✓ GTK 3 found" || echo "✗ GTK 3 not found (for GUI)"

install: cli
	mkdir -p /usr/local/bin
	cp $(TARGET_CLI) /usr/local/bin/firmware-builder
	chmod +x /usr/local/bin/firmware-builder
	@echo "✓ Installed"

help:
	@echo "Firmware Builder - C Implementation"
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all      - Build CLI and library"
	@echo "  cli      - Build command-line tool"
	@echo "  gui      - Build GTK GUI (requires GTK 3)"
	@echo "  lib      - Build static library"
	@echo "  clean    - Remove build artifacts"
	@echo "  deps     - Check dependencies"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this message"
