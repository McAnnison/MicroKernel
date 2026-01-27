#pragma once

#include "kernel/ipc.h"

// Echo service name
#define ECHO_SERVICE_NAME "echo"

// Initialize echo service
void echo_service_init(void);

// Get echo service endpoint
endpoint_id_t echo_service_get_endpoint(void);

// Process pending messages (call periodically)
void echo_service_process(void);
