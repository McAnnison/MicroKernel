#pragma once

#include "kernel/ipc.h"

// Console service name
#define CONSOLE_SERVICE_NAME "console"

// Initialize console service
void console_service_init(void);

// Get console service endpoint
endpoint_id_t console_service_get_endpoint(void);

// Process pending messages (call periodically)
void console_service_process(void);
