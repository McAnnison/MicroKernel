#!/bin/bash
# Demo script for the three services

echo "=== MicroKernel Services Demo ==="
echo ""
echo "This script will demonstrate the three implemented services:"
echo "1. Console/Log Service"
echo "2. Echo Service"
echo "3. Timer Service"
echo ""
echo "Building the kernel..."
cd "$(dirname "$0")"
make clean > /dev/null 2>&1
make > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""
echo "Starting QEMU..."
echo "The kernel will boot and show the three services initializing."
echo ""
echo "Try these commands in the QEMU console:"
echo "  services         - List registered services"
echo "  log Hello        - Send log message to console service"
echo "  ipcecho Test     - Send echo request via IPC"
echo "  timertick        - Trigger timer tick"
echo "  help             - Show all commands"
echo ""
echo "Press Ctrl-A, then X to exit QEMU"
echo ""
sleep 2

make run
