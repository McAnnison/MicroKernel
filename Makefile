<<<<<<< HEAD
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
=======
SHELL := /bin/sh

ARCH := i386

BUILD_DIR := build
ISO_DIR := isodir

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
ISO_IMAGE := $(BUILD_DIR)/microkernel.iso

CC ?= gcc
LD ?= ld
AS := $(CC)

CFLAGS := -std=c11 -O2 -g \
  -ffreestanding -fno-stack-protector -fno-pic -fno-pie \
  -Wall -Wextra -Wpedantic \
  -m32

ASFLAGS := -g \
	-ffreestanding -fno-stack-protector -fno-pic -fno-pie \
	-m32

LDFLAGS := -m elf_i386 -nostdlib -T src/arch/$(ARCH)/linker.ld

KERNEL_C_SRCS := \
  src/kernel/kmain.c \
	src/kernel/cli.c \
  src/kernel/vga.c \
  src/kernel/serial.c \
  src/kernel/panic.c \
  src/kernel/ipc.c \
  src/kernel/service_registry.c \
  src/kernel/util.c \
  src/services/console_service.c \
  src/services/echo_service.c \
  src/services/timer_service.c

KERNEL_ASM_SRCS := \
  src/arch/$(ARCH)/boot.S

KERNEL_OBJS := \
  $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_SRCS)) \
  $(patsubst src/%.S,$(BUILD_DIR)/%.o,$(KERNEL_ASM_SRCS))

.PHONY: all clean iso run

all: $(ISO_IMAGE)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -Isrc -c $< -o $@

$(BUILD_DIR)/%.o: src/%.S
	@mkdir -p $(dir $@)
	$(AS) -x assembler-with-cpp $(ASFLAGS) -c $< -o $@

$(KERNEL_ELF): $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

iso: $(ISO_IMAGE)

$(ISO_IMAGE): $(KERNEL_ELF) boot/grub/grub.cfg
	@rm -rf $(ISO_DIR)
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp boot/grub/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR) >/dev/null

run: $(ISO_IMAGE)
	qemu-system-i386 -cdrom $(ISO_IMAGE) -serial stdio

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)
>>>>>>> 8aa1da5d756dac0c5d64daea3c0d1c82d3485ae4
