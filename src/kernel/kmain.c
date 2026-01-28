#include <stdint.h>
#include <stddef.h>

#include "kernel/cli.h"
#include "kernel/panic.h"
#include "kernel/serial.h"
#include "kernel/task.h"
#include "kernel/vga.h"

void kmain(void) {
    vga_init();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    vga_puts("microkernel: booted (i386)\n");

    serial_init();
    serial_write("microkernel: serial online\n");

    vga_puts("UI: serial CLI ready (type into QEMU console).\n");
    vga_puts("Type `help` for commands.\n");

    cli_run();

    panic("kmain returned");
}
