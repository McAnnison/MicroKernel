#include "ipc/ipc.h"
#include <stdio.h>

ipc_queue_t ping_queue;
ipc_queue_t pong_queue;

void ping_task(void) {
    ipc_message_t msg = {
        .type = IPC_MSG_PING,
        .sender = 1,
        .reply_token = 100
    };

    printf("[Ping] Sending PING\n");
    ipc_send(&pong_queue, &msg);

    ipc_message_t reply;
    ipc_recv(&ping_queue, &reply);

    printf("[Ping] Received PONG\n");
}

void pong_task(void) {
    ipc_message_t msg;
    ipc_recv(&pong_queue, &msg);

    printf("[Pong] Received PING\n");

    ipc_message_t reply = {
        .type = IPC_MSG_PONG,
        .sender = 2,
        .reply_token = msg.reply_token
    };

    printf("[Pong] Sending PONG\n");
    ipc_send(&ping_queue, &reply);
}

void ipc_demo_init(void) {
    ipc_init(&ping_queue);
    ipc_init(&pong_queue);
}
