# Makefile for step-ca installation

# Variables
STEP_CA_VERSION ?= 0.25.2
STEP_CLI_VERSION ?= 0.25.2
GO_VERSION ?= 1.21
INSTALL_DIR ?= $(CURDIR)/bin
BUILD_DIR = build
STEP_CA_REPO = https://github.com/smallstep/certificates.git
STEP_CLI_REPO = https://github.com/smallstep/cli.git

.PHONY: all install clean check-go install-go build-step-ca build-step-cli help

# Default target
all: check-go build-step-ca build-step-cli

# Help target
help:
	@echo "Step-ca Installation Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build both step-ca and step CLI (default)"
	@echo "  check-go         - Check if Go is installed"
	@echo "  install-go       - Install Go (requires sudo)"
	@echo "  build-step-ca    - Build step-ca certificate authority"
	@echo "  build-step-cli   - Build step CLI tool"
	@echo "  install          - Install step-ca and step to $(INSTALL_DIR)"
	@echo "  clean            - Remove build directory and binaries"
	@echo "  uninstall        - Remove installed binaries"
	@echo ""
	@echo "Variables:"
	@echo "  STEP_CA_VERSION  - Version of step-ca to build (default: $(STEP_CA_VERSION))"
	@echo "  STEP_CLI_VERSION - Version of step CLI to build (default: $(STEP_CLI_VERSION))"
	@echo "  INSTALL_DIR      - Installation directory (default: $(INSTALL_DIR))"

# Check if Go is installed
check-go:
	@echo "Checking for Go installation..."
	@if ! command -v go > /dev/null 2>&1; then \
		echo "Go is not installed. Please run 'make install-go' or install Go manually."; \
		exit 1; \
	fi
	@echo "Go is installed: $$(go version)"

# Install Go (requires sudo)
install-go:
	@echo "Installing Go $(GO_VERSION)..."
	@if command -v go > /dev/null 2>&1; then \
		echo "Go is already installed: $$(go version)"; \
	else \
		echo "This requires sudo access..."; \
		wget https://go.dev/dl/go$(GO_VERSION).linux-amd64.tar.gz; \
		sudo rm -rf /usr/local/go; \
		sudo tar -C /usr/local -xzf go$(GO_VERSION).linux-amd64.tar.gz; \
		rm go$(GO_VERSION).linux-amd64.tar.gz; \
		echo "Add the following to your ~/.bashrc or ~/.profile:"; \
		echo 'export PATH=$$PATH:/usr/local/go/bin'; \
	fi

# Build step-ca
build-step-ca: check-go
	@echo "Building step-ca v$(STEP_CA_VERSION)..."
	@mkdir -p $(BUILD_DIR)
	@if [ ! -d "$(BUILD_DIR)/certificates" ]; then \
		cd $(BUILD_DIR) && git clone $(STEP_CA_REPO); \
	fi
	@cd $(BUILD_DIR)/certificates && \
		git fetch --tags && \
		git checkout v$(STEP_CA_VERSION) && \
		make bootstrap && \
		make build
	@cp $(BUILD_DIR)/certificates/bin/step-ca ./step-ca
	@echo "step-ca built successfully: ./step-ca"

# Build step CLI
build-step-cli: check-go
	@echo "Building step CLI v$(STEP_CLI_VERSION)..."
	@mkdir -p $(BUILD_DIR)
	@if [ ! -d "$(BUILD_DIR)/cli" ]; then \
		cd $(BUILD_DIR) && git clone $(STEP_CLI_REPO); \
	fi
	@cd $(BUILD_DIR)/cli && \
		git fetch --tags && \
		git checkout v$(STEP_CLI_VERSION) && \
		make bootstrap && \
		make build
	@cp $(BUILD_DIR)/cli/bin/step ./step
	@echo "step CLI built successfully: ./step"

# Install binaries to system
install: all
	@echo "Installing step-ca and step to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@install -m 755 step-ca $(INSTALL_DIR)/step-ca
	@install -m 755 step $(INSTALL_DIR)/step
	@echo "Installation complete!"
	@echo "Binaries are in $(INSTALL_DIR)"
	@echo "Add $(INSTALL_DIR) to your PATH or run directly: $(INSTALL_DIR)/step-ca"

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@rm -f step-ca step
	@echo "Clean complete."

# Uninstall binaries
uninstall:
	@echo "Removing step-ca and step from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/step-ca
	@rm -f $(INSTALL_DIR)/step
	@rmdir $(INSTALL_DIR) 2>/dev/null || true
	@echo "Uninstall complete."
