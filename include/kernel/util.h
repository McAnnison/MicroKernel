#pragma once

#include <stddef.h>
#include <stdint.h>

// Convert unsigned integer to string
// Returns the length of the string (not including null terminator)
size_t uint_to_str(uint32_t value, char *buf, size_t buf_size);

// Convert unsigned 32-bit integer to fixed-width hex string.
// Writes exactly 8 hex digits plus a null terminator (requires buf_size >= 9).
// Returns the length (8) on success, 0 on failure.
size_t u32_to_hex(uint32_t value, char *buf, size_t buf_size);

// Safe string length
size_t str_len(const char *s);

// Safe string copy with bounds checking
void str_copy_safe(char *dst, const char *src, size_t dst_size);
