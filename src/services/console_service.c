#include "services/console_service.h"
#include "kernel/service_registry.h"
#include "kernel/vga.h"
#include "kernel/serial.h"
#include <stddef.h>

static endpoint_id_t console_endpoint = ENDPOINT_INVALID;

void console_service_init(void) {
    // Create endpoint for console service
    console_endpoint = ipc_endpoint_create();
    
    if (console_endpoint == ENDPOINT_INVALID) {
        serial_write("console_service: failed to create endpoint\n");
        return;
    }
    
    // Register service
    if (service_register(CONSOLE_SERVICE_NAME, console_endpoint) != 0) {
        serial_write("console_service: failed to register\n");
        return;
    }
    
    serial_write("console_service: initialized (endpoint ");
    // Simple integer to string conversion for logging
    char buf[16];
    int i = 0;
    uint32_t ep = console_endpoint;
    do {
        buf[i++] = '0' + (ep % 10);
        ep /= 10;
    } while (ep > 0 && i < 15);
    buf[i] = '\0';
    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
    serial_write(buf);
    serial_write(")\n");
}

endpoint_id_t console_service_get_endpoint(void) {
    return console_endpoint;
}

void console_service_process(void) {
    if (console_endpoint == ENDPOINT_INVALID) {
        return;
    }
    
    // Process all pending messages
    ipc_msg_t msg;
    while (ipc_recv(console_endpoint, &msg) == IPC_SUCCESS) {
        if (msg.type == MSG_LOG) {
            // Print log message to both VGA and serial
            vga_puts("[LOG] ");
            serial_write("[LOG] ");
            
            // Ensure null termination
            if (msg.payload_len < IPC_MAX_PAYLOAD) {
                msg.payload[msg.payload_len] = '\0';
            } else {
                msg.payload[IPC_MAX_PAYLOAD - 1] = '\0';
            }
            
            vga_puts((const char *)msg.payload);
            serial_write((const char *)msg.payload);
            
            if (msg.payload_len > 0 && msg.payload[msg.payload_len - 1] != '\n') {
                vga_puts("\n");
                serial_write("\n");
            }
        }
    }
}
