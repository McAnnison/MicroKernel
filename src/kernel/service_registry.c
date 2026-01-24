#include "kernel/service_registry.h"
#include "kernel/serial.h"
#include <stddef.h>

// Global service registry
static service_entry_t services[SERVICE_MAX_ENTRIES];

// String comparison helper
static int str_cmp(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

// String copy helper
static void str_copy(char *dst, const char *src, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void service_registry_init(void) {
    for (int i = 0; i < SERVICE_MAX_ENTRIES; i++) {
        services[i].active = 0;
        services[i].name[0] = '\0';
        services[i].endpoint = ENDPOINT_INVALID;
    }
}

int service_register(const char *name, endpoint_id_t endpoint) {
    if (!name || endpoint == ENDPOINT_INVALID) {
        return -1;
    }
    
    // Find free slot
    for (int i = 0; i < SERVICE_MAX_ENTRIES; i++) {
        if (!services[i].active) {
            str_copy(services[i].name, name, SERVICE_MAX_NAME_LEN);
            services[i].endpoint = endpoint;
            services[i].active = 1;
            return 0;
        }
    }
    
    return -1; // No free slots
}

endpoint_id_t service_lookup(const char *name) {
    if (!name) {
        return ENDPOINT_INVALID;
    }
    
    for (int i = 0; i < SERVICE_MAX_ENTRIES; i++) {
        if (services[i].active && str_cmp(services[i].name, name)) {
            return services[i].endpoint;
        }
    }
    
    return ENDPOINT_INVALID;
}

void service_list_all(void) {
    serial_write("Registered services:\n");
    int count = 0;
    for (int i = 0; i < SERVICE_MAX_ENTRIES; i++) {
        if (services[i].active) {
            serial_write("  - ");
            serial_write(services[i].name);
            serial_write("\n");
            count++;
        }
    }
    if (count == 0) {
        serial_write("  (none)\n");
    }
}
