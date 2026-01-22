#include "kernel/cli.h"

#include <stddef.h>
#include <stdint.h>

#include "kernel/serial.h"
#include "kernel/vga.h"

static void puts_both(const char *s) {
    vga_puts(s);
    serial_write(s);
}

static int str_eq(const char *a, const char *b) {
    if (!a || !b) {
        return 0;
    }
    while (*a && *b) {
        if (*a != *b) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static const char *skip_spaces(const char *s) {
    if (!s) {
        return "";
    }
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    return s;
}

static void prompt(void) {
    puts_both("mk> ");
}

static void cmd_help(void) {
    puts_both("Commands:\n");
    puts_both("  help         Show this help\n");
    puts_both("  clear        Clear VGA screen\n");
    puts_both("  echo <text>  Print text\n");
    puts_both("  about        Show build info\n");
    puts_both("  halt         Halt CPU\n");
}

static void cmd_about(void) {
    puts_both("microkernel-os: serial CLI (i386)\n");
    puts_both("Tip: run with `make run` and type here.\n");
}

static void cmd_halt(void) {
    puts_both("Halting...\n");
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}

static void exec_line(const char *line) {
    line = skip_spaces(line);
    if (*line == '\0') {
        return;
    }

    if (str_eq(line, "help")) {
        cmd_help();
        return;
    }
    if (str_eq(line, "clear")) {
        vga_clear();
        return;
    }
    if (str_eq(line, "about")) {
        cmd_about();
        return;
    }
    if (str_eq(line, "halt")) {
        cmd_halt();
        return;
    }

    // echo <text>
    static const char echo_prefix[] = "echo";
    const char *p = line;
    for (size_t i = 0; i < sizeof(echo_prefix) - 1; i++) {
        if (p[i] != echo_prefix[i]) {
            p = NULL;
            break;
        }
    }
    if (p) {
        p += sizeof(echo_prefix) - 1;
        p = skip_spaces(p);
        puts_both(p);
        puts_both("\n");
        return;
    }

    puts_both("Unknown command. Type `help`.\n");
}

void cli_run(void) {
    puts_both("\nserial CLI ready. Type `help`.\n");

    char line[128];
    size_t len = 0;

    prompt();

    for (;;) {
        char c = serial_read_blocking();

        if (c == '\r' || c == '\n') {
            puts_both("\n");
            line[len] = '\0';
            exec_line(line);
            len = 0;
            prompt();
            continue;
        }

        if (c == '\b' || c == 0x7F) {
            if (len > 0) {
                len--;
                // Erase last char visually: backspace, space, backspace
                serial_write("\b \b");
                vga_putc('\b');
            }
            continue;
        }

        if ((uint8_t)c < 0x20) {
            // ignore other control characters
            continue;
        }

        if (len + 1 < sizeof(line)) {
            line[len++] = c;
            // local echo
            vga_putc(c);
            serial_write((char[]){c, 0});
        }
    }
}
