# Makefile for Step-CA Manager

# Variables
PYTHON := python3
PIP := pip3
SRC_DIR := src
MAIN := $(SRC_DIR)/main.py
DESKTOP_FILE := step-ca-manager.desktop
INSTALL_DIR := $(HOME)/.local/share/applications

# Add parent directory to PATH for step binaries
export PATH := $(CURDIR)/..:$(PATH)

.PHONY: all run dev install-deps check-deps install uninstall clean help lint format

# Default target
all: help

# Help target
help:
	@echo "Step-CA Manager - GUI Management Application"
	@echo ""
	@echo "Available targets:"
	@echo "  run           - Run the application"
	@echo "  dev           - Run in development mode with debug output"
	@echo "  check-deps    - Check if all dependencies are installed"
	@echo "  install-deps  - Install Python dependencies"
	@echo "  install       - Install desktop launcher"
	@echo "  uninstall     - Remove desktop launcher"
	@echo "  clean         - Clean generated files"
	@echo "  lint          - Run code linter (requires pylint)"
	@echo "  format        - Format code (requires black)"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "System Requirements:"
	@echo "  - Python 3.8+"
	@echo "  - GTK 3.0+"
	@echo "  - Step-CA and Step CLI"
	@echo ""
	@echo "First time setup:"
	@echo "  1. sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0"
	@echo "  2. make install-deps"
	@echo "  3. make check-deps"
	@echo "  4. make run"

# Run the application
run: check-deps
	@echo "Starting Step-CA Manager..."
	@if [ ! -d "$(SRC_DIR)/core" ]; then \
		echo "Error: core module not found!"; \
		echo "The core module seems to be missing. This is a required component."; \
		exit 1; \
	fi
	@$(PYTHON) $(MAIN)

# Run in development mode
dev:
	@echo "Starting Step-CA Manager (development mode)..."
	@GTK_DEBUG=interactive $(PYTHON) $(MAIN)

# Install Python dependencies
install-deps:
	@echo "Installing Python dependencies..."
	@echo "Note: GTK dependencies must be installed via system package manager"
	@echo "Run: sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0"
	@$(PIP) install -r requirements.txt --user
	@echo "Dependencies installed!"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@echo ""
	@echo "Python Dependencies:"
	@$(PYTHON) -c "import gi; gi.require_version('Gtk', '3.0'); from gi.repository import Gtk" 2>/dev/null && \
		echo "  ✓ GTK bindings OK" || \
		(echo "  ✗ GTK bindings not found" && \
		 echo "    Install: sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0" && \
		 exit 1)
	@$(PYTHON) -c "from dateutil import parser" 2>/dev/null && \
		echo "  ✓ python-dateutil OK" || \
		echo "  ✗ python-dateutil not found. Run: make install-deps"
	@echo ""
	@echo "Step Tools:"
	@command -v step >/dev/null 2>&1 && \
		(echo "  ✓ step CLI found: $$(step version | head -1)" || echo "  ✓ step CLI found") || \
		echo "  ✗ step CLI not found (build with 'make build-step-cli' in parent directory)"
	@command -v step-ca >/dev/null 2>&1 && \
		(echo "  ✓ step-ca found: $$(step-ca version | head -1)" || echo "  ✓ step-ca found") || \
		echo "  ✗ step-ca not found (build with 'make build-step-ca' in parent directory)"
	@echo ""
	@echo "Core Module:"
	@if [ -d "$(SRC_DIR)/core" ]; then \
		echo "  ✓ core module found"; \
	else \
		echo "  ✗ core module not found"; \
		exit 1; \
	fi

# Install desktop launcher
install:
	@echo "Installing desktop launcher..."
	@mkdir -p $(INSTALL_DIR)
	@echo "[Desktop Entry]" > $(DESKTOP_FILE)
	@echo "Version=1.0" >> $(DESKTOP_FILE)
	@echo "Type=Application" >> $(DESKTOP_FILE)
	@echo "Name=Step-CA Manager" >> $(DESKTOP_FILE)
	@echo "Comment=Manage Step-CA Certificate Authority" >> $(DESKTOP_FILE)
	@echo "Exec=$(PYTHON) $(PWD)/$(MAIN)" >> $(DESKTOP_FILE)
	@echo "Icon=preferences-system-network" >> $(DESKTOP_FILE)
	@echo "Terminal=false" >> $(DESKTOP_FILE)
	@echo "Categories=System;Security;" >> $(DESKTOP_FILE)
	@cp $(DESKTOP_FILE) $(INSTALL_DIR)/
	@chmod +x $(INSTALL_DIR)/$(DESKTOP_FILE)
	@echo "Desktop launcher installed to $(INSTALL_DIR)"
	@echo "You can now launch Step-CA Manager from your applications menu"

# Uninstall desktop launcher
uninstall:
	@echo "Removing desktop launcher..."
	@rm -f $(INSTALL_DIR)/$(DESKTOP_FILE)
	@rm -f $(DESKTOP_FILE)
	@echo "Desktop launcher removed"

# Clean generated files
clean:
	@echo "Cleaning generated files..."
	@find . -type d -name __pycache__ -exec rm -rf {} + 2>/dev/null || true
	@find . -type f -name "*.pyc" -delete
	@rm -f $(DESKTOP_FILE)
	@echo "Clean complete"

# Development helpers
lint:
	@echo "Running linter..."
	@$(PYTHON) -m pylint $(SRC_DIR) || echo "Install pylint: pip3 install pylint"

format:
	@echo "Formatting code..."
	@$(PYTHON) -m black $(SRC_DIR) || echo "Install black: pip3 install black"
