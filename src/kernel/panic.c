#include "kernel/panic.h"

#include "kernel/serial.h"
#include "kernel/vga.h"

void panic(const char *msg) {
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_puts("PANIC: ");
    vga_puts(msg);
    vga_puts("\n");

    serial_write("PANIC: ");
    serial_write(msg);
    serial_write("\n");

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
