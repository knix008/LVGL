# Test Makefile for IP Address Validation
# ======================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O0
INCLUDES = -I..

# Source files
IP_SOURCE = ../ip_address.c
TEST_SOURCES = test_ipv4.c test_ipv6.c test_all.c
TEST_OBJECTS = $(TEST_SOURCES:%.c=%.o)

# Test executables
TEST_TARGETS = test_ipv4 test_ipv6 test_all

# Default target
all: $(TEST_TARGETS)

# Build individual test executables
test_ipv4: test_ipv4.o
	$(CC) $(CFLAGS) -o $@ $< $(IP_SOURCE)

test_ipv6: test_ipv6.o
	$(CC) $(CFLAGS) -o $@ $< $(IP_SOURCE)

test_all: test_all.o
	$(CC) $(CFLAGS) -o $@ $< $(IP_SOURCE)

# Build object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run all tests
test: $(TEST_TARGETS)
	@echo "Running IPv4 tests..."
	@./test_ipv4
	@echo ""
	@echo "Running IPv6 tests..."
	@./test_ipv6
	@echo ""
	@echo "Running comprehensive tests..."
	@./test_all

# Run individual tests
test-ipv4: test_ipv4
	@./test_ipv4

test-ipv6: test_ipv6
	@./test_ipv6

test-comprehensive: test_all
	@./test_all

# Clean up
clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGETS)

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build all test executables"
	@echo "  test             - Run all tests"
	@echo "  test-ipv4        - Run IPv4 tests only"
	@echo "  test-ipv6        - Run IPv6 tests only"
	@echo "  test-comprehensive - Run comprehensive tests"
	@echo "  clean            - Remove all generated files"
	@echo "  help             - Show this help message"

.PHONY: all test test-ipv4 test-ipv6 test-comprehensive clean help
