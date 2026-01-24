#pragma once

#include "kernel/ipc.h"

#define SERVICE_MAX_NAME_LEN 32
#define SERVICE_MAX_ENTRIES 16

// Service registry entry
typedef struct {
    char name[SERVICE_MAX_NAME_LEN];
    endpoint_id_t endpoint;
    int active;
} service_entry_t;

// Initialize service registry
void service_registry_init(void);

// Register a service
int service_register(const char *name, endpoint_id_t endpoint);

// Lookup a service by name
endpoint_id_t service_lookup(const char *name);

// List all registered services (for debugging)
void service_list_all(void);
