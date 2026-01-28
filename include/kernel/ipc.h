#pragma once

#include <stdint.h>
#include <stddef.h>

#define IPC_MAX_ENDPOINTS 32
#define IPC_QUEUE_SIZE 16
#define IPC_MAX_PAYLOAD 64

// Endpoint ID type
typedef uint32_t endpoint_id_t;

// Invalid endpoint constant
#define ENDPOINT_INVALID ((endpoint_id_t)-1)

// Message type
typedef enum {
    MSG_NONE = 0,
    MSG_LOG,        // Log message
    MSG_ECHO,       // Echo request
    MSG_ECHO_REPLY, // Echo response
    MSG_TIMER_TICK, // Timer tick
    MSG_HEARTBEAT,  // Heartbeat for monitoring
    MSG_CRASH,      // Trigger service crash (for demo)
    MSG_MAX
} msg_type_t;

// IPC message structure
typedef struct {
    msg_type_t type;
    endpoint_id_t sender;
    uint32_t payload_len;
    uint8_t payload[IPC_MAX_PAYLOAD];
} ipc_msg_t;

// IPC return codes
typedef enum {
    IPC_SUCCESS = 0,
    IPC_ERR_INVALID_ENDPOINT = -1,
    IPC_ERR_QUEUE_FULL = -2,
    IPC_ERR_QUEUE_EMPTY = -3,
    IPC_ERR_INVALID_MSG = -4,
} ipc_error_t;

// Initialize IPC subsystem
void ipc_init(void);

// Allocate a new endpoint
endpoint_id_t ipc_endpoint_create(void);

// Send a message to an endpoint (non-blocking)
ipc_error_t ipc_send(endpoint_id_t dst, const ipc_msg_t *msg);

// Receive a message from an endpoint (non-blocking)
ipc_error_t ipc_recv(endpoint_id_t src, ipc_msg_t *out_msg);

// Check if endpoint has pending messages
int ipc_has_messages(endpoint_id_t ep);
