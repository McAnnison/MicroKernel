#include <stdint.h>
#include <stddef.h>

#include "kernel/cli.h"
#include "kernel/panic.h"
#include "kernel/serial.h"
#include "kernel/task.h"
#include "kernel/vga.h"

static void demo_task_a(void *arg) {
    (void)arg;
    for (int i = 0; i < 5; i++) {
        vga_puts("[task A] running\n");
        serial_write("[task A] running\n");
        task_yield();
    }
}

static void demo_task_b(void *arg) {
    (void)arg;
    for (int i = 0; i < 5; i++) {
        vga_puts("[task B] running\n");
        serial_write("[task B] running\n");
        task_yield();
    }
}

void kmain(void) {
    vga_init();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    vga_puts("microkernel: booted (i386)\n");

    serial_init();
    serial_write("microkernel: serial online\n");

    // Issue #1 demo: two cooperative tasks alternating output via yield().
    task_init();
    if (task_create("demo-a", demo_task_a, NULL) < 0 ||
        task_create("demo-b", demo_task_b, NULL) < 0) {
        panic("failed to create demo tasks");
    }
    scheduler_run();

    vga_puts("UI: serial CLI ready (type into QEMU console).\n");
    vga_puts("Type `help` for commands.\n");

    cli_run();

    panic("kmain returned");
}
