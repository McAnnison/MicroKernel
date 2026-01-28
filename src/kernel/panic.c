#include "kernel/panic.h"

#include "kernel/serial.h"
#include "kernel/vga.h"
#include "kernel/task.h"

void panic(const char *msg) {
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_puts("PANIC: ");
    vga_puts(msg);
    vga_puts("\n");

    serial_write("PANIC: ");
    serial_write(msg);
    serial_write("\n");

    // Check if we're in a task context
    int current_task = task_get_current();
    if (current_task >= 0) {
        // We're in a task - terminate only this task.
        serial_write("PANIC: Task context detected - terminating task\n");
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
        task_exit_current();
        for (;;) {
            task_yield();
        }
    }
    
    // Not in a task - this is a kernel panic, halt the system
    serial_write("PANIC: Kernel panic - halting system\n");
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
