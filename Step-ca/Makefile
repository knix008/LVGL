# Makefile for step-ca installation

# Variables
STEP_CA_VERSION ?= 0.28.4
STEP_CLI_VERSION ?= 0.28.4
GO_VERSION ?= 1.24,10
INSTALL_DIR ?= $(CURDIR)/bin
BUILD_DIR = build
STEP_CA_REPO = https://github.com/smallstep/certificates.git
STEP_CLI_REPO = https://github.com/smallstep/cli.git

# Detect architecture
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
    ARCH = amd64
    GO_ARCH = amd64
else ifeq ($(UNAME_M),aarch64)
    ARCH = arm64
    GO_ARCH = arm64
else ifeq ($(UNAME_M),arm64)
    ARCH = arm64
    GO_ARCH = arm64
else
    $(error Unsupported architecture: $(UNAME_M))
endif

# Detect OS
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OS = linux
else ifeq ($(UNAME_S),Darwin)
    OS = darwin
else
    $(error Unsupported operating system: $(UNAME_S))
endif

GO_PACKAGE = go$(GO_VERSION).$(OS)-$(GO_ARCH).tar.gz

# Build timestamp
BUILD_TIME := $(shell date -u '+%Y-%m-%d %H:%M UTC')

# Export GO111MODULE and CGO settings
export GO111MODULE = on
export CGO_ENABLED = 0

.PHONY: all install clean check-go install-go build-step-ca build-step-cli help clean-build rebuild test-step-ca test-step-cli

# Default target
all: check-go build-step-ca build-step-cli

# Help target
help:
	@echo "Step-ca Installation Makefile"
	@echo ""
	@echo "Detected platform: $(OS)-$(GO_ARCH)"
	@echo "Go version: $$(go version 2>/dev/null || echo 'not installed')"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build both step-ca and step CLI (default)"
	@echo "  check-go         - Check if Go is installed"
	@echo "  install-go       - Install Go (requires sudo)"
	@echo "  build-step-ca    - Build step-ca certificate authority"
	@echo "  build-step-cli   - Build step CLI tool"
	@echo "  install          - Install step-ca and step to $(INSTALL_DIR)"
	@echo "  clean            - Remove build directory and binaries"
	@echo "  clean-build      - Remove build directory only (keep binaries)"
	@echo "  rebuild          - Clean and rebuild everything"
	@echo "  test-step-ca     - Test step-ca binary"
	@echo "  test-step-cli    - Test step CLI binary"
	@echo "  uninstall        - Remove installed binaries"
	@echo ""
	@echo "Variables:"
	@echo "  STEP_CA_VERSION  - Version of step-ca to build (default: $(STEP_CA_VERSION))"
	@echo "  STEP_CLI_VERSION - Version of step CLI to build (default: $(STEP_CLI_VERSION))"
	@echo "  INSTALL_DIR      - Installation directory (default: $(INSTALL_DIR))"
	@echo ""
	@echo "Notes:"
	@echo "  - Builds are done directly with 'go build' (no bootstrap required)"
	@echo "  - CGO is disabled for static binaries"
	@echo "  - Binaries are stripped (-ldflags '-w') for smaller size"

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
	@echo "Installing Go $(GO_VERSION) for $(OS)-$(GO_ARCH)..."
	@if command -v go > /dev/null 2>&1; then \
		echo "Go is already installed: $$(go version)"; \
	else \
		echo "This requires sudo access..."; \
		wget https://go.dev/dl/$(GO_PACKAGE); \
		sudo rm -rf /usr/local/go; \
		sudo tar -C /usr/local -xzf $(GO_PACKAGE); \
		rm $(GO_PACKAGE); \
		echo "Add the following to your ~/.bashrc or ~/.profile:"; \
		echo 'export PATH=$$PATH:/usr/local/go/bin'; \
	fi

# Build step-ca
build-step-ca: check-go
	@echo "Building step-ca v$(STEP_CA_VERSION)..."
	@mkdir -p $(BUILD_DIR)
	@if [ ! -d "$(BUILD_DIR)/certificates" ]; then \
		echo "Cloning step-ca repository..."; \
		cd $(BUILD_DIR) && git clone $(STEP_CA_REPO); \
	fi
	@echo "Checking out version v$(STEP_CA_VERSION)..."
	@cd $(BUILD_DIR)/certificates && \
		git fetch --tags && \
		git checkout v$(STEP_CA_VERSION)
	@echo "Downloading dependencies..."
	@cd $(BUILD_DIR)/certificates && go mod download
	@echo "Building step-ca binary..."
	@cd $(BUILD_DIR)/certificates && \
		mkdir -p bin && \
		go build -v \
			-o bin/step-ca \
			-ldflags='-w -X "main.Version=$(STEP_CA_VERSION)" -X "main.BuildTime=$(BUILD_TIME)"' \
			./cmd/step-ca
	@cp $(BUILD_DIR)/certificates/bin/step-ca ./step-ca
	@echo "step-ca built successfully: ./step-ca"
	@./step-ca version

# Build step CLI
build-step-cli: check-go
	@echo "Building step CLI v$(STEP_CLI_VERSION)..."
	@mkdir -p $(BUILD_DIR)
	@if [ ! -d "$(BUILD_DIR)/cli" ]; then \
		echo "Cloning step CLI repository..."; \
		cd $(BUILD_DIR) && git clone $(STEP_CLI_REPO); \
	fi
	@echo "Checking out version v$(STEP_CLI_VERSION)..."
	@cd $(BUILD_DIR)/cli && \
		git fetch --tags && \
		git checkout v$(STEP_CLI_VERSION)
	@echo "Downloading dependencies..."
	@cd $(BUILD_DIR)/cli && go mod download
	@echo "Building step CLI binary..."
	@cd $(BUILD_DIR)/cli && \
		mkdir -p bin && \
		go build -v \
			-o bin/step \
			-ldflags='-w -X "main.Version=$(STEP_CLI_VERSION)" -X "main.BuildTime=$(BUILD_TIME)"' \
			github.com/smallstep/cli/cmd/step
	@cp $(BUILD_DIR)/cli/bin/step ./step
	@echo "step CLI built successfully: ./step"
	@./step version

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
	@echo "Cleaning build directory and binaries..."
	@rm -rf $(BUILD_DIR)
	@rm -f step-ca step
	@echo "Clean complete."

# Clean build directory only (keep binaries)
clean-build:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned."

# Rebuild everything from scratch
rebuild: clean all

# Test step-ca binary
test-step-ca:
	@if [ -f ./step-ca ]; then \
		echo "Testing step-ca binary..."; \
		./step-ca version; \
	else \
		echo "Error: step-ca binary not found. Run 'make build-step-ca' first."; \
		exit 1; \
	fi

# Test step CLI binary
test-step-cli:
	@if [ -f ./step ]; then \
		echo "Testing step CLI binary..."; \
		./step version; \
	else \
		echo "Error: step binary not found. Run 'make build-step-cli' first."; \
		exit 1; \
	fi

# Uninstall binaries
uninstall:
	@echo "Removing step-ca and step from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/step-ca
	@rm -f $(INSTALL_DIR)/step
	@rmdir $(INSTALL_DIR) 2>/dev/null || true
	@echo "Uninstall complete."
