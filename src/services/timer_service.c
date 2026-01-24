#include "services/timer_service.h"
#include "kernel/service_registry.h"
#include "kernel/serial.h"
#include <stddef.h>

#define TIMER_MAX_SUBSCRIBERS 8

static endpoint_id_t timer_endpoint = ENDPOINT_INVALID;
static endpoint_id_t subscribers[TIMER_MAX_SUBSCRIBERS];
static uint32_t subscriber_count = 0;
static uint32_t tick_counter = 0;

void timer_service_init(void) {
    // Create endpoint for timer service
    timer_endpoint = ipc_endpoint_create();
    
    if (timer_endpoint == ENDPOINT_INVALID) {
        serial_write("timer_service: failed to create endpoint\n");
        return;
    }
    
    // Register service
    if (service_register(TIMER_SERVICE_NAME, timer_endpoint) != 0) {
        serial_write("timer_service: failed to register\n");
        return;
    }
    
    // Initialize subscriber list
    for (uint32_t i = 0; i < TIMER_MAX_SUBSCRIBERS; i++) {
        subscribers[i] = ENDPOINT_INVALID;
    }
    subscriber_count = 0;
    tick_counter = 0;
    
    serial_write("timer_service: initialized (endpoint ");
    // Simple integer to string conversion for logging
    char buf[16];
    int i = 0;
    uint32_t ep = timer_endpoint;
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

endpoint_id_t timer_service_get_endpoint(void) {
    return timer_endpoint;
}

void timer_service_subscribe(endpoint_id_t subscriber) {
    if (subscriber == ENDPOINT_INVALID) {
        return;
    }
    
    // Check if already subscribed
    for (uint32_t i = 0; i < subscriber_count; i++) {
        if (subscribers[i] == subscriber) {
            return; // Already subscribed
        }
    }
    
    // Add new subscriber
    if (subscriber_count < TIMER_MAX_SUBSCRIBERS) {
        subscribers[subscriber_count++] = subscriber;
    }
}

void timer_service_tick(void) {
    if (timer_endpoint == ENDPOINT_INVALID) {
        return;
    }
    
    tick_counter++;
    
    // Send tick message to all subscribers
    for (uint32_t i = 0; i < subscriber_count; i++) {
        if (subscribers[i] != ENDPOINT_INVALID) {
            ipc_msg_t tick_msg;
            tick_msg.type = MSG_TIMER_TICK;
            tick_msg.sender = timer_endpoint;
            tick_msg.payload_len = sizeof(uint32_t);
            
            // Copy tick counter to payload
            *((uint32_t *)tick_msg.payload) = tick_counter;
            
            ipc_error_t err = ipc_send(subscribers[i], &tick_msg);
            
            if (err != IPC_SUCCESS && err != IPC_ERR_QUEUE_FULL) {
                // Ignore queue full errors (subscriber too slow)
                serial_write("timer_service: failed to send tick\n");
            }
        }
    }
}
