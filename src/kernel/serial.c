#include "kernel/serial.h"

#include <stdint.h>

#include "kernel/io.h"

#define COM1 0x3F8

static int serial_received(void) {
    return inb(COM1 + 5) & 0x01;
}

static int serial_is_transmit_empty(void) {
    return inb(COM1 + 5) & 0x20;
}

void serial_init(void) {
    outb(COM1 + 1, 0x00); // Disable interrupts
    outb(COM1 + 3, 0x80); // Enable DLAB
    outb(COM1 + 0, 0x03); // Divisor low byte (38400 baud)
    outb(COM1 + 1, 0x00); // Divisor high byte
    outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7); // Enable FIFO, clear, 14-byte threshold
    outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

static void serial_putc(char c) {
    while (!serial_is_transmit_empty()) {
        /* spin */
    }
    outb(COM1, (uint8_t)c);
}

void serial_write(const char *s) {
    if (!s) {
        return;
    }
    for (; *s; s++) {
        if (*s == '\n') {
            serial_putc('\r');
        }
        serial_putc(*s);
    }
}

char serial_read_blocking(void) {
    while (!serial_received()) {
        /* spin */
    }
    return (char)inb(COM1);
}

int serial_read_nonblocking(char *out) {
    if (!out) {
        return 0;
    }
    if (!serial_received()) {
        return 0;
    }
    *out = (char)inb(COM1);
    return 1;
}
