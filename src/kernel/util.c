#include "kernel/util.h"

size_t uint_to_str(uint32_t value, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) {
        return 0;
    }
    
    // Handle zero specially
    if (value == 0) {
        if (buf_size >= 2) {
            buf[0] = '0';
            buf[1] = '\0';
            return 1;
        }
        return 0;
    }
    
    // Convert to string (reversed)
    size_t i = 0;
    uint32_t temp = value;
    while (temp > 0 && i < buf_size - 1) {
        buf[i++] = '0' + (temp % 10);
        temp /= 10;
    }
    
    if (temp > 0) {
        // Number too large for buffer
        buf[0] = '\0';
        return 0;
    }
    
    buf[i] = '\0';
    
    // Reverse the string
    for (size_t j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
    
    return i;
}

size_t str_len(const char *s) {
    if (!s) {
        return 0;
    }
    size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

void str_copy_safe(char *dst, const char *src, size_t dst_size) {
    if (!dst || dst_size == 0) {
        return;
    }
    
    if (!src) {
        dst[0] = '\0';
        return;
    }
    
    size_t i = 0;
    while (i < dst_size - 1 && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}
