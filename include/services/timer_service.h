#pragma once

#include "kernel/ipc.h"

// Timer service name
#define TIMER_SERVICE_NAME "timer"

// Initialize timer service
void timer_service_init(void);

// Get timer service endpoint
endpoint_id_t timer_service_get_endpoint(void);

// Subscribe to timer ticks
void timer_service_subscribe(endpoint_id_t subscriber);

// Process/send timer ticks (call periodically)
void timer_service_tick(void);
