# Microkernel Operating System Prototype — Technical Report

**Course/Project:** Microkernel OS Design (Group 18)
**Date:** January 26, 2026
**Authors:** Group 18

---

## Abstract

This report presents a microkernel-based operating system prototype that emphasizes minimal kernel functionality, message-based inter-process communication (IPC), and service modularity. The kernel provides a small core of mechanisms—task scheduling, basic task management, and console I/O—while moving higher-level services into isolated tasks. The design demonstrates fault isolation by enabling service crash and restart without halting the kernel and evaluates overhead through an IPC vs. direct-call performance comparison. The implementation targets i386, boots via Multiboot2 and GRUB, and is exercised in QEMU with a serial command-line interface. We describe the architecture, implementation choices, testing methodology, and results, and outline limitations and future work.

---

## 1. Introduction and Background

### 1.1 Problem Statement

Traditional monolithic kernels centralize system services inside a large privileged code base, which can simplify performance but increases the risk of system-wide failure when a component faults. Microkernels aim to minimize kernel responsibilities—shifting services into user-space or isolated modules—to improve reliability, security, and modularity. The project’s objective is to build a minimal microkernel-inspired prototype that demonstrates IPC, service separation, fault isolation, and a basic performance comparison.

### 1.2 Motivation

Modern systems increasingly demand resilience, component isolation, and the ability to evolve system services independently. Microkernels provide a promising architecture, but they can incur performance overhead due to message passing and context switches. Building a small prototype lets us explore these trade-offs in a tangible, measurable way while keeping the scope achievable for a short academic timeline.

### 1.3 Objectives

The project goals are:

- Build a bootable i386 microkernel-like prototype.
- Implement IPC for message-based communication.
- Provide a task abstraction and a minimal scheduler.
- Split at least three services into separate tasks.
- Demonstrate fault isolation via crash and restart of a service.
- Compare IPC overhead with a direct-call baseline.

---

## 2. Literature Review

Microkernel research dates back to systems such as Mach, Chorus, and L4. Mach introduced message-based communication and a separation of kernel services but faced performance challenges. Later microkernels (e.g., L4 and its descendants) demonstrated that careful IPC design and low-level optimization can significantly reduce overhead. Contemporary systems like QNX and seL4 illustrate the viability of microkernels for safety- and security-critical domains.

**Key ideas from prior work:**

- **Minimal kernel core:** The kernel should only implement mechanisms that require privileged execution (e.g., scheduling, address space management). Policies belong in services.
- **IPC efficiency:** Message passing is central; its cost can dominate performance if not optimized.
- **Fault isolation:** Separating services reduces the blast radius of faults.
- **Verifiability:** A smaller kernel is easier to analyze and verify.

This prototype draws on these principles but simplifies execution to a kernel-threaded model in the initial scope to meet delivery constraints.

---

## 3. System Design and Architecture

### 3.1 Overall Architecture

The system follows a microkernel-inspired design. The **kernel core** handles boot, minimal scheduling, a task abstraction, and console/serial I/O. **Services** run as separate tasks and interact using **IPC message passing**. A **service registry** maps names to endpoints, enabling simple discovery. A **monitor/health service** restarts failed services to demonstrate fault isolation.

A high-level component view:

- **Kernel core:** Boot, scheduler, task management, VGA/serial output.
- **IPC layer:** Endpoint IDs, bounded queues, send/receive primitives.
- **Service registry:** Static mapping for built-in services.
- **Services:** Echo, timer, log/console, monitor.

### 3.2 Execution Model

Given the project’s timeline, services run as **separately scheduled tasks** within the kernel address space (kernel threads). This preserves the microkernel principle of modularity and message passing, while deferring user-mode isolation (ring 3) to future work. Tasks cooperatively yield to the scheduler.

### 3.3 IPC Design

IPC uses small endpoint IDs and bounded queues. The expected semantics are:

- `ipc_send(dst, msg)`: enqueue or yield/retry if full.
- `ipc_recv(src_filter, out_msg)`: dequeue or yield if empty.

An optional request/reply token supports simple RPC-style exchanges. The bounded queue helps control memory usage and prevents unbounded resource consumption.

**Representative IPC API (from `include/kernel/ipc.h`):**

```
#define IPC_MAX_ENDPOINTS 32
#define IPC_QUEUE_SIZE 16
#define IPC_MAX_PAYLOAD 64

typedef uint32_t endpoint_id_t;
#define ENDPOINT_INVALID ((endpoint_id_t)-1)

typedef enum {
	MSG_NONE = 0,
	MSG_LOG,
	MSG_ECHO,
	MSG_ECHO_REPLY,
	MSG_TIMER_TICK,
	MSG_HEARTBEAT,
	MSG_CRASH,
	MSG_MAX
} msg_type_t;

typedef struct {
	msg_type_t type;
	endpoint_id_t sender;
	uint32_t payload_len;
	uint8_t payload[IPC_MAX_PAYLOAD];
} ipc_msg_t;

typedef enum {
	IPC_SUCCESS = 0,
	IPC_ERR_INVALID_ENDPOINT = -1,
	IPC_ERR_QUEUE_FULL = -2,
	IPC_ERR_QUEUE_EMPTY = -3,
	IPC_ERR_INVALID_MSG = -4,
} ipc_error_t;

ipc_error_t ipc_send(endpoint_id_t dst, const ipc_msg_t *msg);
ipc_error_t ipc_recv(endpoint_id_t src, ipc_msg_t *out_msg);
```

**Bounded ring-buffer enqueue/dequeue (from `src/kernel/ipc.c`):**

```c
if (q->count >= IPC_QUEUE_SIZE) {
	return IPC_ERR_QUEUE_FULL;
}
q->messages[q->tail] = *msg;
q->tail = (q->tail + 1) % IPC_QUEUE_SIZE;
q->count++;

if (q->count == 0) {
	return IPC_ERR_QUEUE_EMPTY;
}
*out_msg = q->messages[q->head];
q->head = (q->head + 1) % IPC_QUEUE_SIZE;
q->count--;
```

**Cooperative “blocking” at the call site:** because `ipc_recv()` is non-blocking, callers implement waiting by yielding the CPU until a message arrives (example from the CLI echo path in `src/kernel/cli.c`).

```c
for (;;) {
	err = ipc_recv(cli_ep, &reply);
	if (err == IPC_SUCCESS) {
		break;
	}
	task_yield();
}
```

### 3.4 Fault Isolation Model

Although all tasks run in a single address space in the prototype, **fault isolation** is demonstrated at the task level. When a service crashes (e.g., triggers a panic inside its task), the **monitor** restarts the service without rebooting the kernel. This emulates the resilience benefits of microkernel service separation.

**Crash trigger + reporting (echo service, `src/services/echo_service.c`):**

```c
while (ipc_recv(echo_endpoint, &msg) == IPC_SUCCESS) {
	if (msg.type == MSG_CRASH) {
		serial_write("echo_service: CRASH MESSAGE RECEIVED - simulating crash!\n");
		monitor_report_crash(echo_endpoint);
		panic("echo_service: intentional crash for demo");
	}
}
```

**Restart loop (monitor service, `src/services/monitor_service.c`):**

```c
for (int i = 0; i < MAX_MONITORED_SERVICES; i++) {
	if (monitored[i].active && monitored[i].crashed) {
		serial_write("[MONITOR] Restarting crashed service: ");
		serial_write(monitored[i].name);
		serial_write("\n");

		if (task_restart(monitored[i].task_id) == 0) {
			monitored[i].crashed = 0;
			serial_write("[MONITOR] Service restarted successfully\n");
		}
	}
}
```

**Task restart primitive (kernel task subsystem, `src/kernel/task.c`):**

```c
// Can only restart if we have the original entry point
if (t->entry == NULL) {
	return -1;
}

// Reset the task state and rebuild its initial stack
t->state = TASK_RUNNABLE;
uint32_t *stack_top = (uint32_t *)(g_stacks[task_id] + STACK_SIZE);
stack_top = (uint32_t *)((uintptr_t)stack_top & ~((uintptr_t)0xF));
*(--stack_top) = (uint32_t)(uintptr_t)task_trampoline;
t->sp = stack_top;
```

### 3.5 User Interface

The system provides an interactive **serial CLI** accessible in QEMU via `-serial stdio`. Commands include:

- `help` — list commands
- `clear` — clear VGA text output
- `echo <text>` — print text
- `about` — system info
- `halt` — halt CPU

The serial UI is chosen for reliability and speed of implementation, avoiding a keyboard driver while enabling interactive demos.

### 3.6 User Space (Current vs. Planned)

In the current prototype, “services” are **kernel tasks** (kernel threads) rather than ring 3 processes. This keeps the project scope focused on message passing and modularity while still enabling a clean path toward a user/kernel boundary.

**Current model: services run as cooperative kernel tasks (from `src/kernel/kmain.c`):**

```c
task_init();
int console_tid = task_create("console", console_task, NULL);
int echo_tid = task_create("echo", echo_task, NULL);
(void)task_create("monitor", monitor_task, NULL);
(void)task_create("cli", cli_task, NULL);

// Register services for restart
if (echo_tid >= 0) {
	monitor_register_service(echo_tid, echo_service_get_endpoint(), ECHO_SERVICE_NAME);
}
if (console_tid >= 0) {
	monitor_register_service(console_tid, console_service_get_endpoint(), CONSOLE_SERVICE_NAME);
}
```

**Planned user-mode direction (ring 3):** introduce a syscall ABI for IPC and task primitives, move service code into user-mode binaries, and keep only mechanism in the kernel. A minimal syscall wrapper could look like:

```c
// illustrative (planned): syscall numbers + a generic trap wrapper
enum {
	SYS_IPC_SEND = 1,
	SYS_IPC_RECV = 2,
};

static inline int sys_call(int num, int a1, int a2, int a3) {
	int ret;
	__asm__ volatile(
		"int $0x80"
		: "=a"(ret)
		: "a"(num), "b"(a1), "c"(a2), "d"(a3)
		: "memory");
	return ret;
}
```

---

## 4. Implementation Details

### 4.1 Build and Boot

The kernel is a **freestanding C** binary targeting i386 and is booted by **Multiboot2 + GRUB**. The build process uses `make` and creates a bootable ISO that can run under QEMU.

Key build artifacts:

- Multiboot2 entry and boot header: `src/arch/i386/boot.S`
- Linker script: `src/arch/i386/linker.ld`
- GRUB config: `boot/grub/grub.cfg`

### 4.2 Task Abstraction and Scheduler

The kernel implements a minimal task system and a cooperative scheduler. Each task has:

- A name
- Entry function and argument
- Stack pointer
- State: `RUNNABLE`, `FINISHED`, or `UNUSED`

The scheduler loops over runnable tasks in round-robin order. Context switching is performed using a small assembly routine (`context_switch.S`) that swaps stack pointers. When a task completes, it transitions to `FINISHED` and is reclaimed.

**Key behavior:**

- `task_create()` allocates a task slot and initializes its stack to enter a trampoline.
- `task_trampoline()` runs the task entry and then calls `task_exit()`.
- `task_yield()` cooperatively yields control to the scheduler.

### 4.3 IPC Layer (Planned/Integrated Modules)

IPC is implemented as a bounded ring buffer per endpoint. Messages are fixed-size structs that include source/destination IDs, type, and payload. Send/receive semantics yield on full or empty queues to preserve kernel responsiveness. (Exact message struct and queue size are defined in the IPC module.)

### 4.4 Service Registry

A simple registry maps service names to endpoint IDs, enabling services and clients to discover each other. The registry is populated during boot for built-in services. This supports a minimal directory-service capability without a full name server.

### 4.5 Services

At least three services are implemented or planned:

- **Log/console service:** Receives log messages and prints them to VGA/serial.
- **Echo service:** Demonstrates IPC by replying with the same payload.
- **Timer service:** Periodically sends tick messages to clients.
- **Monitor service:** Watches services and restarts those that crash.

**Common service pattern (endpoint + registry):** services create a private endpoint and register it by name.

```c
console_endpoint = ipc_endpoint_create();
if (console_endpoint == ENDPOINT_INVALID) {
	serial_write("console_service: failed to create endpoint\n");
	return;
}
if (service_register(CONSOLE_SERVICE_NAME, console_endpoint) != 0) {
	serial_write("console_service: failed to register\n");
	return;
}
```

**Console/log handling (from `src/services/console_service.c`):**

```c
ipc_msg_t msg;
while (ipc_recv(console_endpoint, &msg) == IPC_SUCCESS) {
	if (msg.type == MSG_LOG) {
		vga_puts("[LOG] ");
		serial_write("[LOG] ");
		/* bounded print of msg.payload */
	}
}
```

**Timer broadcast to subscribers (from `src/services/timer_service.c`):**

```c
tick_msg.type = MSG_TIMER_TICK;
tick_msg.sender = timer_endpoint;
tick_msg.payload_len = sizeof(uint32_t);
*((uint32_t *)tick_msg.payload) = tick_counter;
ipc_send(subscribers[i], &tick_msg);
```

**Client-side usage via the registry (CLI example from `src/kernel/cli.c`):**

```c
endpoint_id_t echo_ep = service_lookup(ECHO_SERVICE_NAME);
ipc_error_t err = ipc_send(echo_ep, &msg);
```

### 4.6 CLI and Output

The CLI is driven by the serial port (COM1) and provides a predictable UI for demonstrations and tests. VGA output is used for display, while serial output is used for interactive commands and logging.

---

## 5. Results and Evaluation

### 5.1 Testing Methodology

The test plan includes:

- **Boot smoke test:** Kernel boots, displays banner, CLI is responsive.
- **IPC ping/pong:** Two tasks exchange messages and print results.
- **Service interaction:** Client service uses registry + IPC to call a server.
- **Crash/restart demo:** Service is intentionally crashed and restarted.
- **Performance benchmark:** Direct call loop vs. IPC ping/pong loop.

All tests are performed in QEMU using the same ISO build.

### 5.2 Fault Isolation Demonstration

The monitor service restarts a crashed service without rebooting the kernel. The demo steps are:

1) Start system and services.
2) Trigger a service crash.
3) Monitor restarts the service.
4) System continues handling requests.

This validates that service failures can be isolated and recovered at the task level.

### 5.4 Discussion

The prototype achieves modularity and clear service boundaries. IPC overhead is expected to be higher than direct calls, but remains acceptable for the demonstrated scope. The lack of hardware-enforced isolation is a known limitation of the current execution model; however, the control-plane design aligns with microkernel principles and can be extended with user-mode servers.

---

## 6. Conclusion and Future Work

### 6.1 Summary of Achievements

- Bootable i386 kernel prototype using Multiboot2 + GRUB.
- Minimal scheduler and task abstraction.
- Message-based IPC foundation.
- Multi-service architecture with registry support.
- Fault isolation demo via service restart.
- Performance benchmark plan and initial results placeholder.

### 6.2 Limitations

- No hardware-enforced isolation yet: services run as cooperative kernel tasks in a single address space (ring 0), so memory corruption in one task can still affect the whole system.
- Cooperative scheduler only: tasks must yield explicitly (`task_yield()`), so a misbehaving task can monopolize CPU time (no preemption/time slicing).
- IPC is intentionally minimal: fixed-size messages with bounded queues (`IPC_MAX_PAYLOAD`, `IPC_QUEUE_SIZE`), simple endpoints (`IPC_MAX_ENDPOINTS`), and non-blocking primitives that require yield-based waiting at call sites.
- Fault handling is demo-oriented: the crash demo uses an intentional `panic()` path; general fault recovery (e.g., invalid memory access) is not robust in the current execution model.
- Restart semantics are limited: `task_restart()` restarts a task entry point, but full service re-initialization, state reconciliation, and endpoint/registry lifecycle management are simplified.
- Driver and subsystem scope is small: serial + VGA output only; no keyboard input, filesystem, networking, or broader device abstraction layer.
- No memory management or security model: no paging-based protection domains, capabilities, or access-control enforcement between services.
- Benchmark results are approximate: the `bench` command reports TSC deltas under QEMU for a specific workload, which is useful for relative comparison but not a definitive real-hardware performance evaluation.

### 6.3 Future Work

- Move services to user-mode (ring 3) with syscalls.
- Add stronger isolation with page tables and capabilities.
- Optimize IPC path (fast-path, zero-copy buffers).
- Expand services (filesystem, device abstraction, network).
- Improve tooling and CI with automated tests.

---

## References (Suggested)

- A. Tanenbaum, *Modern Operating Systems*.
- J. Liedtke, “On µ-Kernel Construction.”
- G. Heiser et al., L4 microkernel literature.
- seL4 documentation and formal verification materials.

---

## Appendix A — Repository Map (for readers)

- `src/arch/i386/boot.S` — Multiboot2 entry point
- `src/arch/i386/linker.ld` — Linker script
- `src/kernel/` — Kernel core (task, CLI, VGA, serial)
- `docs/ARCHITECTURE.md` — Architecture overview
- `docs/UI.md` — UI plan and CLI commands
- `scripts/build.ps1` — Windows build wrapper
- `scripts/run.ps1` — Windows run wrapper
