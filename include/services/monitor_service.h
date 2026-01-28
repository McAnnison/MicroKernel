#pragma once

#include "kernel/ipc.h"

// Monitor service name
#define MONITOR_SERVICE_NAME "monitor"

// Initialize monitor service
void monitor_service_init(void);

// Get monitor service endpoint
endpoint_id_t monitor_service_get_endpoint(void);

// Register a service task for monitoring
void monitor_register_service(int task_id, endpoint_id_t ep, const char *name);

// Process monitoring (detect crashes, restart services)
void monitor_service_process(void);

// Report a service crash (called when crash detected)
void monitor_report_crash(endpoint_id_t crashed_ep);
