#include "kernel/cli.h"

#include <stddef.h>
#include <stdint.h>

#include "kernel/serial.h"
#include "kernel/vga.h"
#include "kernel/ipc.h"
#include "kernel/service_registry.h"
#include "kernel/util.h"
#include "kernel/timing.h"
#include "services/console_service.h"
#include "services/echo_service.h"
#include "services/timer_service.h"
#include "services/monitor_service.h"

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
    puts_both("  services     List registered services\n");
    puts_both("  log <text>   Send log message to console service\n");
    puts_both("  ipcecho <text> Send echo request via IPC\n");
    puts_both("  timertick    Trigger timer tick\n");
    puts_both("  bench [n]    Benchmark direct vs IPC\n");
    puts_both("  crash        Crash echo service (fault isolation demo)\n");
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

static void cmd_services(void) {
    service_list_all();
}

static void cmd_log(const char *text) {
    endpoint_id_t console_ep = service_lookup(CONSOLE_SERVICE_NAME);
    if (console_ep == ENDPOINT_INVALID) {
        puts_both("Error: console service not found\n");
        return;
    }
    
    // Create and send log message
    ipc_msg_t msg;
    msg.type = MSG_LOG;
    msg.sender = ENDPOINT_INVALID; // CLI has no endpoint
    
    // Copy text to payload
    size_t len = 0;
    while (text[len] != '\0' && len < IPC_MAX_PAYLOAD - 1) {
        msg.payload[len] = text[len];
        len++;
    }
    msg.payload[len] = '\0';
    msg.payload_len = len;
    
    ipc_error_t err = ipc_send(console_ep, &msg);
    if (err == IPC_SUCCESS) {
        puts_both("Log message sent via IPC\n");
        // Process console service to print the message
        console_service_process();
    } else {
        puts_both("Error: failed to send log message\n");
    }
}

static void cmd_ipcecho(const char *text) {
    endpoint_id_t echo_ep = service_lookup(ECHO_SERVICE_NAME);
    if (echo_ep == ENDPOINT_INVALID) {
        puts_both("Error: echo service not found\n");
        return;
    }
    
    // Create a temporary endpoint for receiving the reply
    endpoint_id_t reply_ep = ipc_endpoint_create();
    if (reply_ep == ENDPOINT_INVALID) {
        puts_both("Error: failed to create reply endpoint\n");
        return;
    }
    
    // Create and send echo request
    ipc_msg_t msg;
    msg.type = MSG_ECHO;
    msg.sender = reply_ep;
    
    // Copy text to payload
    size_t len = 0;
    while (text[len] != '\0' && len < IPC_MAX_PAYLOAD - 1) {
        msg.payload[len] = text[len];
        len++;
    }
    msg.payload[len] = '\0';
    msg.payload_len = len;
    
    ipc_error_t err = ipc_send(echo_ep, &msg);
    if (err != IPC_SUCCESS) {
        puts_both("Error: failed to send echo request\n");
        return;
    }
    
    puts_both("Echo request sent via IPC, processing...\n");
    
    // Process echo service
    echo_service_process();
    
    // Try to receive reply
    ipc_msg_t reply;
    err = ipc_recv(reply_ep, &reply);
    if (err == IPC_SUCCESS && reply.type == MSG_ECHO_REPLY) {
        // Ensure null termination with proper bounds checking
        size_t safe_len = reply.payload_len;
        if (safe_len >= IPC_MAX_PAYLOAD) {
            safe_len = IPC_MAX_PAYLOAD - 1;
        }
        reply.payload[safe_len] = '\0';
        puts_both("Echo reply received: ");
        puts_both((const char *)reply.payload);
        puts_both("\n");
    } else {
        puts_both("Error: no reply received\n");
    }
}

static void cmd_timertick(void) {
    puts_both("Triggering timer tick...\n");
    timer_service_tick();
    puts_both("Timer tick sent to subscribers\n");
}

static uint32_t parse_u32_or_default(const char *s, uint32_t def) {
    s = skip_spaces(s);
    if (!s || *s == '\0') {
        return def;
    }
    uint32_t v = 0;
    int any = 0;
    while (*s >= '0' && *s <= '9') {
        any = 1;
        uint32_t digit = (uint32_t)(*s - '0');
        // very simple overflow clamp
        if (v > 400000000u) {
            return def;
        }
        v = v * 10u + digit;
        s++;
    }
    return any ? v : def;
}

static void print_tsc_delta(const char *label, tsc_t d) {
    char hi[9];
    char lo[9];
    u32_to_hex(d.hi, hi, sizeof(hi));
    u32_to_hex(d.lo, lo, sizeof(lo));

    puts_both(label);
    puts_both("0x");
    puts_both(hi);
    puts_both(lo);
    puts_both("\n");
}

static void direct_echo_copy(const uint8_t *in, uint32_t len, uint8_t *out) {
    if (!in || !out) {
        return;
    }
    if (len > IPC_MAX_PAYLOAD) {
        len = IPC_MAX_PAYLOAD;
    }
    for (uint32_t i = 0; i < len; i++) {
        out[i] = in[i];
    }
}

static void cmd_bench(const char *args) {
    uint32_t n = parse_u32_or_default(args, 2000u);
    if (n == 0) {
        n = 1;
    }

    endpoint_id_t echo_ep = service_lookup(ECHO_SERVICE_NAME);
    if (echo_ep == ENDPOINT_INVALID) {
        puts_both("bench: echo service not found\n");
        return;
    }

    endpoint_id_t client_ep = ipc_endpoint_create();
    if (client_ep == ENDPOINT_INVALID) {
        puts_both("bench: failed to create client endpoint\n");
        return;
    }

    uint8_t payload[IPC_MAX_PAYLOAD];
    uint8_t out[IPC_MAX_PAYLOAD];
    for (uint32_t i = 0; i < IPC_MAX_PAYLOAD; i++) {
        payload[i] = (uint8_t)('A' + (i % 26));
        out[i] = 0;
    }

    uint32_t payload_len = 32;

    puts_both("bench: iterations=");
    char nbuf[16];
    uint_to_str(n, nbuf, sizeof(nbuf));
    puts_both(nbuf);
    puts_both("\n");

    // Direct-call benchmark
    tsc_t t0 = tsc_now();
    for (uint32_t i = 0; i < n; i++) {
        direct_echo_copy(payload, payload_len, out);
    }
    tsc_t t1 = tsc_now();
    tsc_t d_direct = tsc_sub(t1, t0);

    // IPC benchmark (client -> echo service -> client)
    ipc_msg_t msg;
    msg.type = MSG_ECHO;
    msg.sender = client_ep;
    msg.payload_len = payload_len;
    for (uint32_t i = 0; i < payload_len; i++) {
        msg.payload[i] = payload[i];
    }

    tsc_t t2 = tsc_now();
    for (uint32_t i = 0; i < n; i++) {
        if (ipc_send(echo_ep, &msg) != IPC_SUCCESS) {
            puts_both("bench: ipc_send failed\n");
            break;
        }

        // Let the service handle the request
        echo_service_process();

        ipc_msg_t reply;
        if (ipc_recv(client_ep, &reply) != IPC_SUCCESS || reply.type != MSG_ECHO_REPLY) {
            puts_both("bench: missing/invalid reply\n");
            break;
        }
    }
    tsc_t t3 = tsc_now();
    tsc_t d_ipc = tsc_sub(t3, t2);

    print_tsc_delta("bench: direct cycles = ", d_direct);
    print_tsc_delta("bench: ipc cycles    = ", d_ipc);
    puts_both("bench: (counts are TSC delta; compare magnitudes)\n");
}

static void cmd_crash(void) {
    puts_both("[CRASH DEMO] Sending crash message to echo service...\n");
    
    endpoint_id_t echo_ep = service_lookup(ECHO_SERVICE_NAME);
    if (echo_ep == ENDPOINT_INVALID) {
        puts_both("Error: echo service not found\n");
        return;
    }
    
    ipc_msg_t crash_msg;
    crash_msg.type = MSG_CRASH;
    crash_msg.sender = ENDPOINT_INVALID;
    crash_msg.payload_len = 0;
    
    ipc_error_t err = ipc_send(echo_ep, &crash_msg);
    if (err != IPC_SUCCESS) {
        puts_both("Error: failed to send crash message\n");
        return;
    }
    
    puts_both("[CRASH DEMO] Crash message sent. Processing...\n");
    
    // Process the message (this should trigger the crash)
    echo_service_process();
    
    // If we get here, the service crashed and we caught it
    puts_both("[CRASH DEMO] Service crashed! Monitor will restart it.\n");
    
    // Report crash to monitor
    extern void monitor_report_crash(endpoint_id_t);
    monitor_report_crash(echo_ep);
    
    // Run monitor to restart service
    monitor_service_process();
    
    puts_both("[CRASH DEMO] Test the echo service again with 'ipcecho test'\n");
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
    if (str_eq(line, "services")) {
        cmd_services();
        return;
    }
    if (str_eq(line, "timertick")) {
        cmd_timertick();
        return;
    }
    if (line[0] == 'b' && line[1] == 'e' && line[2] == 'n' && line[3] == 'c' && line[4] == 'h' && (line[5] == '\0' || line[5] == ' ' || line[5] == '\t')) {
        cmd_bench(line + 5);
        return;
    }
    if (str_eq(line, "crash")) {
        cmd_crash();
        return;
    }
    if (str_eq(line, "halt")) {
        cmd_halt();
        return;
    }

    // log <text>
    static const char log_prefix[] = "log";
    const char *p = line;
    for (size_t i = 0; i < sizeof(log_prefix) - 1; i++) {
        if (p[i] != log_prefix[i]) {
            p = NULL;
            break;
        }
    }
    if (p && (p[sizeof(log_prefix) - 1] == ' ' || p[sizeof(log_prefix) - 1] == '\t' || p[sizeof(log_prefix) - 1] == '\0')) {
        p += sizeof(log_prefix) - 1;
        p = skip_spaces(p);
        cmd_log(p);
        return;
    }
    
    // ipcecho <text>
    static const char ipcecho_prefix[] = "ipcecho";
    p = line;
    for (size_t i = 0; i < sizeof(ipcecho_prefix) - 1; i++) {
        if (p[i] != ipcecho_prefix[i]) {
            p = NULL;
            break;
        }
    }
    if (p && (p[sizeof(ipcecho_prefix) - 1] == ' ' || p[sizeof(ipcecho_prefix) - 1] == '\t' || p[sizeof(ipcecho_prefix) - 1] == '\0')) {
        p += sizeof(ipcecho_prefix) - 1;
        p = skip_spaces(p);
        cmd_ipcecho(p);
        return;
    }

    // echo <text>
    static const char echo_prefix[] = "echo";
    p = line;
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
