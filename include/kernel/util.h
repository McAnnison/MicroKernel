#pragma once

#include <stddef.h>
#include <stdint.h>

// Convert unsigned integer to string
// Returns the length of the string (not including null terminator)
size_t uint_to_str(uint32_t value, char *buf, size_t buf_size);

// Safe string length
size_t str_len(const char *s);

// Safe string copy with bounds checking
void str_copy_safe(char *dst, const char *src, size_t dst_size);
