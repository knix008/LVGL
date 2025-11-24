# GTK Webcam Application Makefile with FAISS + Deep Learning

CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17 -fPIC
INCLUDES := -Iinclude $(shell pkg-config --cflags gtk+-3.0 gdk-pixbuf-2.0 opencv4)

# ONNX Runtime - check if locally installed or use system package
ONNX_LOCAL_DIR := $(wildcard onnxruntime-linux-*/)
ifdef ONNX_LOCAL_DIR
  # Local ONNX Runtime installation (from install_onnxruntime.sh)
  ONNX_INCLUDES := -I$(ONNX_LOCAL_DIR)/include
  ONNX_LIBS := -L$(ONNX_LOCAL_DIR)/lib -lonnxruntime
  INCLUDES += $(ONNX_INCLUDES)
  $(info Using local ONNX Runtime: $(ONNX_LOCAL_DIR))
else
  # System ONNX Runtime installation
  ONNX_LIBS := -lonnxruntime
  $(info Using system ONNX Runtime)
endif

# FAISS - local installation is optional (we use in-memory implementation)
FAISS_LOCAL_DIR := faiss
ifeq ($(wildcard $(FAISS_LOCAL_DIR)/lib/libfaiss.so),)
  $(info FAISS not found - using in-memory index implementation)
  FAISS_LIBS :=
else
  FAISS_INCLUDES := -I$(FAISS_LOCAL_DIR)/include
  FAISS_LIBS := -L$(FAISS_LOCAL_DIR)/lib -lfaiss -Wl,-rpath,$(shell pwd)/$(FAISS_LOCAL_DIR)/lib
  INCLUDES += $(FAISS_INCLUDES)
  $(info Using local FAISS: $(FAISS_LOCAL_DIR))
endif

# Combine all libraries
LIBS := $(shell pkg-config --libs gtk+-3.0 gdk-pixbuf-2.0 opencv4) -pthread -lsqlite3 $(ONNX_LIBS) $(FAISS_LIBS)

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

# Clean build artifacts (keep external dependencies)
clean:
	@rm -rf $(OBJ_DIR) $(TARGET)
	@rm -rf *.db *.bin
	@echo "Cleaned build artifacts"

# Full clean including external dependencies (be careful!)
distclean: clean
	@rm -rf onnxruntime-*/
	@rm -rf faiss/
	@echo "Cleaned all artifacts including external dependencies"

# Help target
help:
	@echo "GTK Webcam Application - Build Targets"
	@echo "======================================"
	@echo "make          - Build the application"
	@echo "make run      - Build and run the application"
	@echo "make debug    - Build with debug symbols"
	@echo "make debug-run - Build and run with GDB debugger"
	@echo "make clean    - Remove build artifacts (keeps ONNX Runtime & FAISS)"
	@echo "make distclean - Remove all artifacts including ONNX Runtime & FAISS"
	@echo "make help     - Show this help message"

.PHONY: all run debug debug-run clean distclean help
