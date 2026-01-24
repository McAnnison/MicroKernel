#include "kernel/ipc.h"
#include <stddef.h>

// Message queue (ring buffer)
typedef struct {
    ipc_msg_t messages[IPC_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} msg_queue_t;

// Endpoint structure
typedef struct {
    int active;
    msg_queue_t queue;
} endpoint_t;

// Global endpoint table
static endpoint_t endpoints[IPC_MAX_ENDPOINTS];
static uint32_t next_endpoint_id = 0;

void ipc_init(void) {
    for (uint32_t i = 0; i < IPC_MAX_ENDPOINTS; i++) {
        endpoints[i].active = 0;
        endpoints[i].queue.head = 0;
        endpoints[i].queue.tail = 0;
        endpoints[i].queue.count = 0;
    }
    next_endpoint_id = 0;
}

endpoint_id_t ipc_endpoint_create(void) {
    if (next_endpoint_id >= IPC_MAX_ENDPOINTS) {
        return ENDPOINT_INVALID;
    }
    
    endpoint_id_t id = next_endpoint_id++;
    endpoints[id].active = 1;
    endpoints[id].queue.head = 0;
    endpoints[id].queue.tail = 0;
    endpoints[id].queue.count = 0;
    
    return id;
}

ipc_error_t ipc_send(endpoint_id_t dst, const ipc_msg_t *msg) {
    if (dst >= IPC_MAX_ENDPOINTS || !endpoints[dst].active) {
        return IPC_ERR_INVALID_ENDPOINT;
    }
    
    if (!msg) {
        return IPC_ERR_INVALID_MSG;
    }
    
    msg_queue_t *q = &endpoints[dst].queue;
    
    if (q->count >= IPC_QUEUE_SIZE) {
        return IPC_ERR_QUEUE_FULL;
    }
    
    // Enqueue message
    q->messages[q->tail] = *msg;
    q->tail = (q->tail + 1) % IPC_QUEUE_SIZE;
    q->count++;
    
    return IPC_SUCCESS;
}

ipc_error_t ipc_recv(endpoint_id_t src, ipc_msg_t *out_msg) {
    if (src >= IPC_MAX_ENDPOINTS || !endpoints[src].active) {
        return IPC_ERR_INVALID_ENDPOINT;
    }
    
    if (!out_msg) {
        return IPC_ERR_INVALID_MSG;
    }
    
    msg_queue_t *q = &endpoints[src].queue;
    
    if (q->count == 0) {
        return IPC_ERR_QUEUE_EMPTY;
    }
    
    // Dequeue message
    *out_msg = q->messages[q->head];
    q->head = (q->head + 1) % IPC_QUEUE_SIZE;
    q->count--;
    
    return IPC_SUCCESS;
}

int ipc_has_messages(endpoint_id_t ep) {
    if (ep >= IPC_MAX_ENDPOINTS || !endpoints[ep].active) {
        return 0;
    }
    
    return endpoints[ep].queue.count > 0;
}
