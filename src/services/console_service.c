#include "services/console_service.h"
#include "kernel/service_registry.h"
#include "kernel/vga.h"
#include "kernel/serial.h"
#include "kernel/util.h"
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
    char buf[16];
    uint_to_str(console_endpoint, buf, sizeof(buf));
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
            
            // Ensure null termination with proper bounds checking
            size_t safe_len = msg.payload_len;
            if (safe_len >= IPC_MAX_PAYLOAD) {
                safe_len = IPC_MAX_PAYLOAD - 1;
            }
            msg.payload[safe_len] = '\0';
            
            vga_puts((const char *)msg.payload);
            serial_write((const char *)msg.payload);
            
            if (safe_len > 0 && msg.payload[safe_len - 1] != '\n') {
                vga_puts("\n");
                serial_write("\n");
            }
        }
    }
}
