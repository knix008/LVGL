# GTK Webcam Application Makefile

CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17 -fPIC
INCLUDES := -Iinclude $(shell pkg-config --cflags gtk+-3.0 gdk-pixbuf-2.0 opencv4)
LIBS := $(shell pkg-config --libs gtk+-3.0 gdk-pixbuf-2.0 opencv4) -pthread -lsqlite3

# Directories
SRC_DIR := src
OBJ_DIR := build
INCLUDE_DIR := include

# Source and object files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET := gtk_webcam

# Default target
all: $(TARGET)

# Create directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@
	@echo "Build completed: $(TARGET)"

# Run the application
run: $(TARGET)
	@echo "Starting GTK Webcam Viewer..."
	./$(TARGET)

# Debug build
debug: CXXFLAGS := -Wall -Wextra -g -std=c++17 -fPIC
debug: clean $(TARGET)
	@echo "Debug build completed"

# Run with GDB debugger
debug-run: debug
	gdb ./$(TARGET)

# Clean build artifacts
clean:
	@rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned build artifacts"

# Help target
help:
	@echo "GTK Webcam Application - Build Targets"
	@echo "======================================"
	@echo "make          - Build the application"
	@echo "make run      - Build and run the application"
	@echo "make debug    - Build with debug symbols"
	@echo "make debug-run - Build and run with GDB debugger"
	@echo "make clean    - Remove build artifacts"
	@echo "make help     - Show this help message"

.PHONY: all run debug debug-run clean help
