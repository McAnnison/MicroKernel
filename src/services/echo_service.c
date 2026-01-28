#include "services/echo_service.h"
#include "kernel/service_registry.h"
#include "kernel/serial.h"
#include "kernel/util.h"
#include <stddef.h>

static endpoint_id_t echo_endpoint = ENDPOINT_INVALID;

void echo_service_init(void) {
    // Create endpoint for echo service
    echo_endpoint = ipc_endpoint_create();
    
    if (echo_endpoint == ENDPOINT_INVALID) {
        serial_write("echo_service: failed to create endpoint\n");
        return;
    }
    
    // Register service
    if (service_register(ECHO_SERVICE_NAME, echo_endpoint) != 0) {
        serial_write("echo_service: failed to register\n");
        return;
    }
    
    serial_write("echo_service: initialized (endpoint ");
    char buf[16];
    uint_to_str(echo_endpoint, buf, sizeof(buf));
    serial_write(buf);
    serial_write(")\n");
}

endpoint_id_t echo_service_get_endpoint(void) {
    return echo_endpoint;
}

void echo_service_process(void) {
    if (echo_endpoint == ENDPOINT_INVALID) {
        return;
    }
    
    // Process all pending messages
    ipc_msg_t msg;
    while (ipc_recv(echo_endpoint, &msg) == IPC_SUCCESS) {
        if (msg.type == MSG_ECHO) {
            // Reply with echo response
            ipc_msg_t reply;
            reply.type = MSG_ECHO_REPLY;
            reply.sender = echo_endpoint;
            reply.payload_len = msg.payload_len;
            
            // Copy payload
            for (uint32_t i = 0; i < msg.payload_len && i < IPC_MAX_PAYLOAD; i++) {
                reply.payload[i] = msg.payload[i];
            }
            
            // Send reply back to sender
            ipc_error_t err = ipc_send(msg.sender, &reply);
            
            if (err != IPC_SUCCESS) {
                serial_write("echo_service: failed to send reply\n");
            }
        }
    }
}
