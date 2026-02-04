#include "kernel/keyboard.h"
#include "kernel/io.h"

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

static uint8_t kbd_scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
    // ... rest are 0
};

void keyboard_init(void) {
    // No initialization needed for basic polling
}

int keyboard_read_nonblocking(char *out) {
    if (!out) return 0;
    if ((inb(KBD_STATUS_PORT) & 0x01) == 0) return 0;
    uint8_t scancode = inb(KBD_DATA_PORT);
    if (scancode & 0x80) return 0; // ignore key releases
    char c = kbd_scancode_to_ascii[scancode];
    if (c == 0) return 0;
    // Map Enter key to '\r' for CLI compatibility
    if (scancode == 0x1C) c = '\r';
    *out = c;
    return 1;
}
