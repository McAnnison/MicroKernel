#include "kernel/vga.h"

#include <stddef.h>
#include <stdint.h>

static volatile uint16_t *const VGA_BUFFER = (uint16_t *)0xB8000;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t cursor_row;
static size_t cursor_col;
static uint8_t vga_color;

static uint8_t make_color(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)(fg | (bg << 4));
}

static uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

static void scroll_if_needed(void) {
    if (cursor_row < VGA_HEIGHT) {
        return;
    }

    for (size_t row = 1; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            VGA_BUFFER[(row - 1) * VGA_WIDTH + col] = VGA_BUFFER[row * VGA_WIDTH + col];
        }
    }

    for (size_t col = 0; col < VGA_WIDTH; col++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = make_vga_entry(' ', vga_color);
    }

    cursor_row = VGA_HEIGHT - 1;
}

void vga_init(void) {
    cursor_row = 0;
    cursor_col = 0;
    vga_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            VGA_BUFFER[row * VGA_WIDTH + col] = make_vga_entry(' ', vga_color);
        }
    }
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_color = make_color(fg, bg);
}

void vga_putc(char c) {
    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = make_vga_entry(' ', vga_color);
        }
        return;
    }
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        scroll_if_needed();
        return;
    }

    VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = make_vga_entry(c, vga_color);
    cursor_col++;
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
        scroll_if_needed();
    }
}

void vga_puts(const char *s) {
    if (!s) {
        return;
    }
    for (; *s; s++) {
        vga_putc(*s);
    }
}

void vga_clear(void) {
    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            VGA_BUFFER[row * VGA_WIDTH + col] = make_vga_entry(' ', vga_color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}
