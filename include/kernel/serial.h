#pragma once

void serial_init(void);
void serial_write(const char *s);

// Blocks until a character is available on COM1.
char serial_read_blocking(void);
