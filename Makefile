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
	src/kernel/task.c \
  src/kernel/vga.c \
  src/kernel/serial.c \
  src/kernel/panic.c \
  src/kernel/ipc.c \
  src/kernel/service_registry.c \
  src/kernel/util.c \
  src/services/console_service.c \
  src/services/echo_service.c \
  src/services/timer_service.c \
  src/services/monitor_service.c

KERNEL_ASM_SRCS := \
	src/arch/$(ARCH)/boot.S \
	src/arch/$(ARCH)/context_switch.S

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
