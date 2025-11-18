# Makefile for HTTPS Server using Mongoose

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2 -DMG_TLS=MG_TLS_OPENSSL -DMG_TLS_VERSION_MIN=TLS1_3_VERSION
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
SOURCES = server.c mongoose.c
OBJECTS = $(BUILD_DIR)/server.o $(BUILD_DIR)/mongoose.o
TARGET = $(BUILD_DIR)/server

.PHONY: all clean run setup-certs mongoose help

# Default target
all: mongoose $(TARGET)

# Help
help:
	@echo "HTTPS Server Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Download Mongoose and build server"
	@echo "  mongoose    - Download Mongoose library"
	@echo "  build       - Build the server"
	@echo "  setup-certs - Setup step-ca and generate certificates"
	@echo "  run         - Run the server"
	@echo "  clean       - Remove build artifacts"
	@echo ""
	@echo "Usage:"
	@echo "  1. make all          # Build everything"
	@echo "  2. make setup-certs  # Setup certificates with step-ca"
	@echo "  3. make run          # Start the server"

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

# Compile server
$(BUILD_DIR)/server.o: server.c mongoose.h | $(BUILD_DIR)
	@echo "Compiling server.c..."
	@$(CC) $(CFLAGS) -c server.c -o $(BUILD_DIR)/server.o

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking server..."
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
	@echo "1. Initialize step-ca (if not already done):"
	@echo "   ../bin/step ca init"
	@echo ""
	@echo "2. Start step-ca in another terminal:"
	@echo "   ../bin/step-ca \$$HOME/.step/config/ca.json"
	@echo ""
	@echo "3. Get the root certificate:"
	@echo "   ../bin/step ca root $(CERTS_DIR)/root_ca.crt"
	@echo ""
	@echo "4. Request a certificate for your server:"
	@echo "   ../bin/step ca certificate localhost $(CERTS_DIR)/server.crt $(CERTS_DIR)/server.key"
	@echo ""
	@echo "Or run the setup script: make run-cert-setup"

# Automated certificate setup (requires step-ca to be running)
run-cert-setup:
	@mkdir -p $(CERTS_DIR)
	@echo "Fetching root certificate..."
	@../bin/step ca root $(CERTS_DIR)/root_ca.crt
	@echo "Requesting server certificate for localhost..."
	@../bin/step ca certificate localhost $(CERTS_DIR)/server.crt $(CERTS_DIR)/server.key
	@echo "Certificates generated successfully!"
	@ls -la $(CERTS_DIR)/

# Run the server
run: $(TARGET)
	@if [ ! -f "$(CERTS_DIR)/server.crt" ] || [ ! -f "$(CERTS_DIR)/server.key" ]; then \
		echo "Error: Certificates not found. Please run 'make setup-certs' first."; \
		exit 1; \
	fi
	@echo "Starting HTTPS server..."
	@$(TARGET)

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
