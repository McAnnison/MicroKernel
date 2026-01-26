#!/bin/bash
# Simple build and test script for IPC implementation

set -e  # Exit on error

echo "=== Building IPC Demo ==="
echo ""

# Create build directory
mkdir -p build

# Compile IPC
echo "Compiling IPC..."
gcc -Wall -Wextra -std=c11 -g -I. -c ipc/ipc.c -o build/ipc.o

# Compile Scheduler
echo "Compiling Scheduler..."
gcc -Wall -Wextra -std=c11 -g -I. -c scheduler/scheduler.c -o build/scheduler.o

# Compile Demo
echo "Compiling Demo..."
gcc -Wall -Wextra -std=c11 -g -I. -c demo/main.c -o build/main.o

# Link
echo "Linking..."
gcc -Wall -Wextra -std=c11 -g build/ipc.o build/scheduler.o build/main.o -o build/ipc_demo

echo ""
echo "=== Build Complete ==="
echo ""
echo "Running demo..."
echo ""

# Run the demo
./build/ipc_demo

echo ""
echo "=== Test Complete ==="
