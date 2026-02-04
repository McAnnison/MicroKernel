#ifndef KERNEL_KEYBOARD_H
#define KERNEL_KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
int keyboard_read_nonblocking(char *out);

#endif // KERNEL_KEYBOARD_H
