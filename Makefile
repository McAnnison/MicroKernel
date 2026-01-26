CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
INCLUDES = -I.
LDFLAGS = 

# Directories
IPC_DIR = ipc
SCHEDULER_DIR = scheduler
DEMO_DIR = demo
BUILD_DIR = build

# Source files
IPC_SRCS = $(IPC_DIR)/ipc.c
SCHEDULER_SRCS = $(SCHEDULER_DIR)/scheduler.c
DEMO_SRCS = $(DEMO_DIR)/main.c

# Object files
IPC_OBJS = $(BUILD_DIR)/ipc.o
SCHEDULER_OBJS = $(BUILD_DIR)/scheduler.o
DEMO_OBJS = $(BUILD_DIR)/main.o

# Target executable
TARGET = $(BUILD_DIR)/ipc_demo

.PHONY: all clean run help

all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build IPC object
$(BUILD_DIR)/ipc.o: $(IPC_DIR)/ipc.c $(IPC_DIR)/ipc.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build scheduler object
$(BUILD_DIR)/scheduler.o: $(SCHEDULER_DIR)/scheduler.c $(SCHEDULER_DIR)/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build demo object
$(BUILD_DIR)/main.o: $(DEMO_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link executable
$(TARGET): $(IPC_OBJS) $(SCHEDULER_OBJS) $(DEMO_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $(TARGET)"

# Run the demo
run: $(TARGET)
	@echo "Running IPC demo..."
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Help target
help:
	@echo "IPC Implementation Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build the IPC demo (default)"
	@echo "  run     - Build and run the IPC demo"
	@echo "  clean   - Remove build artifacts"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Example:"
	@echo "  make        # Build the project"
	@echo "  make run    # Build and run"
	@echo "  make clean  # Clean build files"
