#ifndef IPC_H
#define IPC_H

#include <stdint.h>
#include <stdbool.h>

#define IPC_MAX_MSG   16
#define IPC_PAYLOAD_SIZE 32

typedef enum {
    IPC_MSG_PING = 1,
    IPC_MSG_PONG = 2,
} ipc_msg_type_t;

typedef struct {
    ipc_msg_type_t type;
    uint32_t sender;
    uint32_t reply_token;   // optional
    char payload[IPC_PAYLOAD_SIZE];
} ipc_message_t;

typedef struct {
    ipc_message_t buffer[IPC_MAX_MSG];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} ipc_queue_t;

// API
void ipc_init(ipc_queue_t *q);
bool ipc_send(ipc_queue_t *q, ipc_message_t *msg);
bool ipc_recv(ipc_queue_t *q, ipc_message_t *msg);

#endif
