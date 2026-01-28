CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
CFLAGS_RELEASE = -Wall -Wextra -std=c11 -O2
INCLUDES = -I.
LDFLAGS = 

# Directories
IPC_DIR = ipc
SCHEDULER_DIR = scheduler
DEMO_DIR = demo
BENCHMARK_DIR = benchmark
BUILD_DIR = build

# Source files
IPC_SRCS = $(IPC_DIR)/ipc.c
SCHEDULER_SRCS = $(SCHEDULER_DIR)/scheduler.c
DEMO_SRCS = $(DEMO_DIR)/main.c
BENCHMARK_SRCS = $(BENCHMARK_DIR)/perf_benchmark.c

# Object files
IPC_OBJS = $(BUILD_DIR)/ipc.o
SCHEDULER_OBJS = $(BUILD_DIR)/scheduler.o
DEMO_OBJS = $(BUILD_DIR)/main.o
BENCHMARK_OBJS = $(BUILD_DIR)/perf_benchmark.o

# Target executables
DEMO_TARGET = $(BUILD_DIR)/ipc_demo
BENCHMARK_TARGET = $(BUILD_DIR)/perf_benchmark

.PHONY: all clean run help benchmark run-benchmark

all: $(DEMO_TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build IPC object
$(BUILD_DIR)/ipc.o: $(IPC_DIR)/ipc.c $(IPC_DIR)/ipc.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build IPC object (optimized for benchmark)
$(BUILD_DIR)/ipc_opt.o: $(IPC_DIR)/ipc.c $(IPC_DIR)/ipc.h | $(BUILD_DIR)
	$(CC) $(CFLAGS_RELEASE) $(INCLUDES) -c $< -o $@

# Build scheduler object
$(BUILD_DIR)/scheduler.o: $(SCHEDULER_DIR)/scheduler.c $(SCHEDULER_DIR)/scheduler.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build demo object
$(BUILD_DIR)/main.o: $(DEMO_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build benchmark object
$(BUILD_DIR)/perf_benchmark.o: $(BENCHMARK_DIR)/perf_benchmark.c $(BENCHMARK_DIR)/timing.h | $(BUILD_DIR)
	$(CC) $(CFLAGS_RELEASE) $(INCLUDES) -c $< -o $@

# Link demo executable
$(DEMO_TARGET): $(IPC_OBJS) $(SCHEDULER_OBJS) $(DEMO_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $(DEMO_TARGET)"

# Link benchmark executable
$(BENCHMARK_TARGET): $(BUILD_DIR)/ipc_opt.o $(BENCHMARK_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS_RELEASE) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $(BENCHMARK_TARGET)"

# Build benchmark
benchmark: $(BENCHMARK_TARGET)

# Run the demo
run: $(DEMO_TARGET)
	@echo "Running IPC demo..."
	./$(DEMO_TARGET)

# Run the benchmark
run-benchmark: $(BENCHMARK_TARGET)
	@echo "Running performance benchmark..."
	./$(BENCHMARK_TARGET)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Help target
help:
	@echo "IPC Implementation Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build the IPC demo (default)"
	@echo "  run           - Build and run the IPC demo"
	@echo "  benchmark     - Build the performance benchmark"
	@echo "  run-benchmark - Build and run the performance benchmark"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Example:"
	@echo "  make              # Build the demo"
	@echo "  make run          # Build and run demo"
	@echo "  make benchmark    # Build benchmark"
	@echo "  make run-benchmark # Build and run benchmark"
	@echo "  make clean        # Clean build files"
