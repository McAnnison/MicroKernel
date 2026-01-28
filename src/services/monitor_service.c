#include "services/monitor_service.h"
#include "kernel/service_registry.h"
#include "kernel/serial.h"
#include "kernel/task.h"
#include "kernel/util.h"
#include <stddef.h>

#define MAX_MONITORED_SERVICES 8

typedef struct {
    int task_id;
    endpoint_id_t endpoint;
    char name[32];
    int active;
    int crashed;
} monitored_service_t;

static endpoint_id_t monitor_endpoint = ENDPOINT_INVALID;
static monitored_service_t monitored[MAX_MONITORED_SERVICES];

void monitor_service_init(void) {
    // Create endpoint for monitor service
    monitor_endpoint = ipc_endpoint_create();
    
    if (monitor_endpoint == ENDPOINT_INVALID) {
        serial_write("monitor_service: failed to create endpoint\n");
        return;
    }
    
    // Register service
    if (service_register(MONITOR_SERVICE_NAME, monitor_endpoint) != 0) {
        serial_write("monitor_service: failed to register\n");
        return;
    }
    
    // Initialize monitored services list
    for (int i = 0; i < MAX_MONITORED_SERVICES; i++) {
        monitored[i].active = 0;
        monitored[i].crashed = 0;
        monitored[i].task_id = -1;
        monitored[i].endpoint = ENDPOINT_INVALID;
    }
    
    serial_write("monitor_service: initialized (endpoint ");
    char buf[16];
    uint_to_str(monitor_endpoint, buf, sizeof(buf));
    serial_write(buf);
    serial_write(")\n");
}

endpoint_id_t monitor_service_get_endpoint(void) {
    return monitor_endpoint;
}

void monitor_register_service(int task_id, endpoint_id_t ep, const char *name) {
    for (int i = 0; i < MAX_MONITORED_SERVICES; i++) {
        if (!monitored[i].active) {
            monitored[i].task_id = task_id;
            monitored[i].endpoint = ep;
            monitored[i].active = 1;
            monitored[i].crashed = 0;
            
            // Copy name
            int j = 0;
            while (j < 31 && name[j] != '\0') {
                monitored[i].name[j] = name[j];
                j++;
            }
            monitored[i].name[j] = '\0';
            
            serial_write("monitor: registered service '");
            serial_write(monitored[i].name);
            serial_write("'\n");
            return;
        }
    }
    
    serial_write("monitor: failed to register service (no slots)\n");
}

void monitor_service_process(void) {
    if (monitor_endpoint == ENDPOINT_INVALID) {
        return;
    }
    
    // Process messages (could be heartbeats, crash notifications, etc.)
    ipc_msg_t msg;
    while (ipc_recv(monitor_endpoint, &msg) == IPC_SUCCESS) {
        // Handle monitoring messages here if needed
    }
    
    // Check for crashed services and restart them
    for (int i = 0; i < MAX_MONITORED_SERVICES; i++) {
        if (monitored[i].active && monitored[i].crashed) {
            serial_write("[MONITOR] Restarting crashed service: ");
            serial_write(monitored[i].name);
            serial_write("\n");
            
            // Attempt to restart the task
            if (task_restart(monitored[i].task_id) == 0) {
                monitored[i].crashed = 0;
                serial_write("[MONITOR] Service restarted successfully\n");
            } else {
                serial_write("[MONITOR] Failed to restart service\n");
            }
        }
    }
}

// Called externally when a service crash is detected
void monitor_report_crash(endpoint_id_t crashed_ep) {
    for (int i = 0; i < MAX_MONITORED_SERVICES; i++) {
        if (monitored[i].active && monitored[i].endpoint == crashed_ep) {
            serial_write("[MONITOR] CRASH DETECTED: ");
            serial_write(monitored[i].name);
            serial_write("\n");
            monitored[i].crashed = 1;
            return;
        }
    }
}
