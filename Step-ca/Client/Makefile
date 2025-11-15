# Makefile for HTTPS Client using Mongoose

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -DMG_ENABLE_OPENSSL=1
LDFLAGS = -lssl -lcrypto

# Directories
SRC_DIR = .
BUILD_DIR = build
CERTS_DIR = certs

# Mongoose settings
MONGOOSE_VERSION = 7.13
MONGOOSE_URL = https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.c
MONGOOSE_H_URL = https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.h

# Source files
SOURCES = client.c mongoose.c
OBJECTS = $(BUILD_DIR)/client.o $(BUILD_DIR)/mongoose.o
TARGET = $(BUILD_DIR)/client

.PHONY: all clean test setup-certs mongoose help

# Default target
all: mongoose $(TARGET)

# Help
help:
	@echo "HTTPS Client Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Download Mongoose and build client"
	@echo "  mongoose    - Download Mongoose library"
	@echo "  build       - Build the client"
	@echo "  setup-certs - Setup step-ca and generate certificates"
	@echo "  test        - Run test requests to the server"
	@echo "  clean       - Remove build artifacts"
	@echo ""
	@echo "Usage:"
	@echo "  1. make all          # Build everything"
	@echo "  2. make setup-certs  # Setup certificates with step-ca"
	@echo "  3. make test         # Test the client"

# Download Mongoose
mongoose: mongoose.c mongoose.h

mongoose.c:
	@echo "Downloading mongoose.c..."
	@wget -q $(MONGOOSE_URL) -O mongoose.c || curl -s $(MONGOOSE_URL) -o mongoose.c
	@echo "Downloaded mongoose.c"

mongoose.h:
	@echo "Downloading mongoose.h..."
	@wget -q $(MONGOOSE_H_URL) -O mongoose.h || curl -s $(MONGOOSE_H_URL) -o mongoose.h
	@echo "Downloaded mongoose.h"

# Build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile mongoose
$(BUILD_DIR)/mongoose.o: mongoose.c mongoose.h | $(BUILD_DIR)
	@echo "Compiling mongoose.c..."
	@$(CC) $(CFLAGS) -c mongoose.c -o $(BUILD_DIR)/mongoose.o

# Compile client
$(BUILD_DIR)/client.o: client.c mongoose.h | $(BUILD_DIR)
	@echo "Compiling client.c..."
	@$(CC) $(CFLAGS) -c client.c -o $(BUILD_DIR)/client.o

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking client..."
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Setup certificates using step-ca
setup-certs:
	@echo "Setting up certificates with step-ca..."
	@if [ ! -f "../bin/step" ] || [ ! -f "../bin/step-ca" ]; then \
		echo "Error: step-ca not found. Please run 'make install' in the parent directory first."; \
		exit 1; \
	fi
	@mkdir -p $(CERTS_DIR)
	@echo ""
	@echo "Certificate setup instructions:"
	@echo "1. Ensure step-ca is running in another terminal"
	@echo ""
	@echo "2. Get the root certificate:"
	@echo "   ../bin/step ca root $(CERTS_DIR)/root_ca.crt"
	@echo ""
	@echo "3. Request a certificate for the client:"
	@echo "   ../bin/step ca certificate client $(CERTS_DIR)/client.crt $(CERTS_DIR)/client.key"
	@echo ""
	@echo "Or run the automated setup: make run-cert-setup"

# Automated certificate setup (requires step-ca to be running)
run-cert-setup:
	@mkdir -p $(CERTS_DIR)
	@echo "Fetching root certificate..."
	@../bin/step ca root $(CERTS_DIR)/root_ca.crt
	@echo "Requesting client certificate..."
	@../bin/step ca certificate client $(CERTS_DIR)/client.crt $(CERTS_DIR)/client.key
	@echo "Certificates generated successfully!"
	@ls -la $(CERTS_DIR)/

# Test the client with various requests
test: $(TARGET)
	@if [ ! -f "$(CERTS_DIR)/client.crt" ] || [ ! -f "$(CERTS_DIR)/client.key" ]; then \
		echo "Error: Certificates not found. Please run 'make run-cert-setup' first."; \
		exit 1; \
	fi
	@echo "Testing HTTPS client..."
	@echo ""
	@echo "=== Test 1: GET / ==="
	@$(TARGET) -u https://localhost:8443/
	@echo ""
	@echo "=== Test 2: GET /api/status ==="
	@$(TARGET) -u https://localhost:8443/api/status
	@echo ""
	@echo "=== Test 3: GET /api/info ==="
	@$(TARGET) -u https://localhost:8443/api/info
	@echo ""
	@echo "All tests completed!"

# Test with custom URL
test-url: $(TARGET)
	@if [ -z "$(URL)" ]; then \
		echo "Error: Please specify URL. Example: make test-url URL=https://localhost:8443/api/status"; \
		exit 1; \
	fi
	@$(TARGET) -u $(URL)

# Test POST request
test-post: $(TARGET)
	@if [ -z "$(URL)" ] || [ -z "$(DATA)" ]; then \
		echo "Error: Please specify URL and DATA."; \
		echo "Example: make test-post URL=https://localhost:8443/api/data DATA='{\"key\":\"value\"}'"; \
		exit 1; \
	fi
	@$(TARGET) -u $(URL) -m POST -d '$(DATA)'

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@rm -f mongoose.c mongoose.h
	@echo "Clean complete."

# Clean everything including certificates
distclean: clean
	@rm -rf $(CERTS_DIR)
	@echo "All artifacts removed."
