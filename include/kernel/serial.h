#pragma once

void serial_init(void);
void serial_write(const char *s);

// Blocks until a character is available on COM1.
char serial_read_blocking(void);

// Non-blocking read from COM1.
// Returns 1 if a character was read into *out, 0 otherwise.
int serial_read_nonblocking(char *out);
