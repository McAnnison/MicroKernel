#include "ipc/ipc.h"
#include "scheduler/scheduler.h"
#include <stdio.h>
#include <string.h>

// Shared queues between tasks
ipc_queue_t ping_queue;
ipc_queue_t pong_queue;

// Task state tracking
static bool ping_done = false;
static bool pong_done = false;

void ping_task(void) {
    static int ping_count = 0;
    static bool initialized = false;
    
    if (!initialized) {
        printf("[Ping] Task started\n");
        initialized = true;
    }
    
    // Loop for multiple ping/pong exchanges
    while (ping_count < 3) {
        // Send PING message
        ipc_message_t msg = {
            .type = IPC_MSG_PING,
            .sender = 1,
            .reply_token = 100 + ping_count,
        };
        snprintf(msg.payload, IPC_PAYLOAD_SIZE, "PING #%d", ping_count);
        
        printf("[Ping] Sending PING (token: %u)\n", msg.reply_token);
        ipc_send(&pong_queue, &msg);
        
        // Wait for PONG reply
        ipc_message_t reply;
        ipc_recv(&ping_queue, &reply);
        
        printf("[Ping] Received PONG (token: %u, payload: %s)\n", 
               reply.reply_token, reply.payload);
        
        ping_count++;
    }
    
    printf("[Ping] Task completed\n");
    ping_done = true;
}

void pong_task(void) {
    static bool initialized = false;
    
    if (!initialized) {
        printf("[Pong] Task started\n");
        initialized = true;
    }
    
    // Loop to handle multiple ping/pong exchanges
    while (!ping_done) {
        // Wait for PING message
        ipc_message_t msg;
        ipc_recv(&pong_queue, &msg);
        
        printf("[Pong] Received PING (token: %u, payload: %s)\n", 
               msg.reply_token, msg.payload);
        
        // Send PONG reply
        ipc_message_t reply = {
            .type = IPC_MSG_PONG,
            .sender = 2,
            .reply_token = msg.reply_token, // Echo back the token
        };
        snprintf(reply.payload, IPC_PAYLOAD_SIZE, "PONG reply to token %u", msg.reply_token);
        
        printf("[Pong] Sending PONG (token: %u)\n", reply.reply_token);
        ipc_send(&ping_queue, &reply);
    }
    
    printf("[Pong] Task completed\n");
    pong_done = true;
}

int main(void) {
    printf("=== IPC Ping/Pong Demo ===\n\n");
    
    // Initialize IPC queues
    ipc_init(&ping_queue);
    ipc_init(&pong_queue);
    printf("Initialized IPC queues (max %d messages each)\n\n", IPC_MAX_MSG);
    
    // Initialize scheduler
    scheduler_init();
    
    // Add tasks
    scheduler_add_task(ping_task, "ping");
    scheduler_add_task(pong_task, "pong");
    
    // Run scheduler
    scheduler_run();
    
    printf("\n=== Demo completed ===\n");
    return 0;
}
