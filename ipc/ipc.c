#include "ipc.h"

// Provided by your OS / scheduler
extern void task_yield(void);

/**
 * Initialize an IPC queue (ring buffer)
 * Sets head, tail, and count to zero
 */
void ipc_init(ipc_queue_t *q) {
    if (q == NULL) {
        return;
    }
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

/**
 * Send a message to the queue (blocking if full)
 * Uses cooperative blocking: yields to scheduler when queue is full
 * 
 * @param q   Pointer to the IPC queue
 * @param msg Pointer to the message to send
 * @return    true if message was successfully queued
 */
bool ipc_send(ipc_queue_t *q, ipc_message_t *msg) {
    if (q == NULL || msg == NULL) {
        return false;
    }

    // Block cooperatively if queue is full
    while (q->count == IPC_MAX_MSG) {
        // Queue full → yield to scheduler and retry
        task_yield();
    }

    // Add message to ring buffer
    q->buffer[q->tail] = *msg;
    q->tail = (q->tail + 1) % IPC_MAX_MSG;
    q->count++;

    return true;
}

/**
 * Receive a message from the queue (blocking if empty)
 * Uses cooperative blocking: yields to scheduler when queue is empty
 * 
 * @param q   Pointer to the IPC queue
 * @param msg Pointer to store the received message
 * @return    true if message was successfully received
 */
bool ipc_recv(ipc_queue_t *q, ipc_message_t *msg) {
    if (q == NULL || msg == NULL) {
        return false;
    }

    // Block cooperatively if queue is empty
    while (q->count == 0) {
        // Queue empty → yield to scheduler and retry
        task_yield();
    }

    // Remove message from ring buffer
    *msg = q->buffer[q->head];
    q->head = (q->head + 1) % IPC_MAX_MSG;
    q->count--;

    return true;
}
