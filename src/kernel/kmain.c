#include <stdint.h>

#include "kernel/cli.h"
#include "kernel/panic.h"
#include "kernel/serial.h"
#include "kernel/vga.h"
#include "kernel/ipc.h"
#include "kernel/service_registry.h"
#include "services/console_service.h"
#include "services/echo_service.h"
#include "services/timer_service.h"

void kmain(void) {
    vga_init();
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
    vga_puts("microkernel: booted (i386)\n");

    serial_init();
    serial_write("microkernel: serial online\n");

    // Initialize IPC subsystem
    ipc_init();
    serial_write("microkernel: IPC initialized\n");

    // Initialize service registry
    service_registry_init();
    serial_write("microkernel: Service registry initialized\n");

    // Initialize services
    console_service_init();
    echo_service_init();
    timer_service_init();

    // List registered services
    service_list_all();

    vga_puts("UI: serial CLI ready (type into QEMU console).\n");
    vga_puts("Type `help` for commands.\n");

    cli_run();

    panic("kmain returned");
}
