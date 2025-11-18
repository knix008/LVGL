# Makefile for HTTPS Client using OpenSSL

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto

# Directories
SRC_DIR = .
BUILD_DIR = build
CERTS_DIR = certs

# Source files
SOURCES = client.c
OBJECTS = $(BUILD_DIR)/client.o
TARGET = $(BUILD_DIR)/client

.PHONY: all clean test setup-certs help

# Default target
all: $(TARGET)

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

# Build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile client
$(BUILD_DIR)/client.o: client.c | $(BUILD_DIR)
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
	@echo "Setting up certificates..."
	@echo ""
	@echo "Step 1: Copying root certificate from ~/.step/certs/"
	@if [ -f "$(HOME)/.step/certs/root_ca.crt" ]; then \
		cp $(HOME)/.step/certs/root_ca.crt $(CERTS_DIR)/root_ca.crt; \
		echo "✓ Root certificate copied successfully"; \
	else \
		echo "Error: Root certificate not found at $(HOME)/.step/certs/root_ca.crt"; \
		echo "Please ensure step-ca is initialized first."; \
		exit 1; \
	fi
	@echo ""
	@echo "Step 2: Requesting client certificate..."
	@if [ -f "$(HOME)/.step/secrets/password" ]; then \
		echo "Using provisioner password from ~/.step/secrets/password"; \
		../bin/step ca certificate --ca-url https://localhost:9000 --force \
			--provisioner-password-file=$(HOME)/.step/secrets/password \
			client $(CERTS_DIR)/client.crt $(CERTS_DIR)/client.key; \
	else \
		echo "Error: Provisioner password file not found at $(HOME)/.step/secrets/password"; \
		echo "Please ensure step-ca is initialized first."; \
		exit 1; \
	fi
	@echo ""
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
	@echo "=== Test 2: GET /api/v1/getserverinfo ==="
	@$(TARGET) -u https://localhost:8443/api/v1/getserverinfo
	@echo ""
	@echo "=== Test 3: GET /api/status ==="
	@$(TARGET) -u https://localhost:8443/api/status
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
	@echo "Clean complete."

# Clean everything including certificates
distclean: clean
	@rm -rf $(CERTS_DIR)
	@echo "All artifacts removed."
