# Makefile for Password Login Tests

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I..
LDFLAGS = 

# Test executables
TESTS = test_password_validation test_userid_validation

# Source files
PASSWORD_SRC = ../password.c
PASSWORD_OBJ = password.o

.PHONY: all clean run

all: $(TESTS)

# Compile password.c
$(PASSWORD_OBJ): $(PASSWORD_SRC)
	@echo "Compiling password.c..."
	$(CC) $(CFLAGS) -c $(PASSWORD_SRC) -o $(PASSWORD_OBJ)

# Build password validation test
test_password_validation: test_password_validation.c $(PASSWORD_OBJ)
	@echo "Building test_password_validation..."
	$(CC) $(CFLAGS) test_password_validation.c $(PASSWORD_OBJ) -o test_password_validation $(LDFLAGS)

# Build userid validation test
test_userid_validation: test_userid_validation.c $(PASSWORD_OBJ)
	@echo "Building test_userid_validation..."
	$(CC) $(CFLAGS) test_userid_validation.c $(PASSWORD_OBJ) -o test_userid_validation $(LDFLAGS)

# Run all tests
run: all
	@echo ""
	@echo "========================================="
	@echo "RUNNING ALL TESTS"
	@echo "========================================="
	@echo ""
	@./test_password_validation
	@./test_userid_validation
	@echo "========================================="
	@echo "ALL TESTS COMPLETED"
	@echo "========================================="

# Clean build artifacts
clean:
	@echo "Cleaning test artifacts..."
	rm -f $(TESTS) $(PASSWORD_OBJ)
	@echo "✓ Clean complete"

