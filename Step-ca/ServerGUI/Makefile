.PHONY: help run dev check-deps install-deps clean

# Detect Python
PYTHON := $(shell which python3 2>/dev/null || which python 2>/dev/null)
PIP := $(shell which pip3 2>/dev/null || which pip 2>/dev/null)

# Directories
SRC_DIR := src
SERVER_DIR := ../Server

# Colors for output
BLUE := \033[1;34m
GREEN := \033[1;32m
YELLOW := \033[1;33m
RED := \033[1;31m
NC := \033[0m # No Color

help:
	@echo "$(BLUE)HTTPS Server GUI - Available Commands$(NC)"
	@echo ""
	@echo "$(GREEN)Development:$(NC)"
	@echo "  make run          - Run the HTTPS Server GUI"
	@echo "  make dev          - Run in development mode with debug output"
	@echo ""
	@echo "$(GREEN)Setup:$(NC)"
	@echo "  make check-deps   - Check if all dependencies are installed"
	@echo "  make install-deps - Install Python dependencies"
	@echo "  make build-server - Build the C server executable"
	@echo ""
	@echo "$(GREEN)Maintenance:$(NC)"
	@echo "  make clean        - Clean generated files"
	@echo "  make help         - Show this help message"

run: check-deps
	@echo "$(BLUE)Starting HTTPS Server GUI...$(NC)"
	@cd $(SRC_DIR) && $(PYTHON) main.py

dev: check-deps
	@echo "$(BLUE)Starting HTTPS Server GUI (debug mode)...$(NC)"
	@cd $(SRC_DIR) && GTK_DEBUG=interactive $(PYTHON) main.py

check-deps:
	@echo "$(BLUE)Checking dependencies...$(NC)"
	@echo ""
	@echo "Python Dependencies:"
	@$(PYTHON) --version 2>/dev/null && \
		echo "  $(GREEN)✓$(NC) Python OK" || \
		(echo "  $(RED)✗$(NC) Python not found" && exit 1)
	@$(PYTHON) -c "import gi; gi.require_version('Gtk', '3.0'); from gi.repository import Gtk" 2>/dev/null && \
		echo "  $(GREEN)✓$(NC) GTK bindings OK" || \
		(echo "  $(RED)✗$(NC) GTK bindings not found" && \
		 echo "    Install: sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0" && \
		 exit 1)
	@$(PYTHON) -c "import psutil" 2>/dev/null && \
		echo "  $(GREEN)✓$(NC) psutil OK" || \
		(echo "  $(YELLOW)⚠$(NC) psutil not found (optional for stats)" && \
		 echo "    Install: pip3 install psutil")
	@echo ""
	@echo "Server Executable:"
	@if [ -x "$(SERVER_DIR)/build/server" ]; then \
		echo "  $(GREEN)✓$(NC) Server executable found"; \
	else \
		echo "  $(YELLOW)⚠$(NC) Server executable not found"; \
		echo "    Build it: cd $(SERVER_DIR) && make all"; \
	fi
	@echo ""
	@echo "Certificates:"
	@if [ -f "$(SERVER_DIR)/certs/server.crt" ]; then \
		echo "  $(GREEN)✓$(NC) Server certificate found"; \
	else \
		echo "  $(YELLOW)⚠$(NC) Server certificate not found"; \
		echo "    Generate: cd $(SERVER_DIR) && make run-cert-setup"; \
	fi
	@echo ""

install-deps:
	@echo "$(BLUE)Installing Python dependencies...$(NC)"
	@if [ -z "$(PIP)" ]; then \
		echo "$(RED)Error: pip not found$(NC)"; \
		exit 1; \
	fi
	@$(PIP) install -r requirements.txt
	@echo "$(GREEN)Dependencies installed successfully!$(NC)"

build-server:
	@echo "$(BLUE)Building HTTPS Server executable...$(NC)"
	@cd $(SERVER_DIR) && $(MAKE) all
	@echo "$(GREEN)Server built successfully!$(NC)"

clean:
	@echo "$(BLUE)Cleaning generated files...$(NC)"
	@find . -type d -name __pycache__ -exec rm -rf {} + 2>/dev/null || true
	@find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@echo "$(GREEN)Clean complete!$(NC)"
