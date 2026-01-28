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

### 3.4 Fault Isolation Model
Although all tasks run in a single address space in the prototype, **fault isolation** is demonstrated at the task level. When a service crashes (e.g., triggers a panic inside its task), the **monitor** restarts the service without rebooting the kernel. This emulates the resilience benefits of microkernel service separation.

### 3.5 User Interface
The system provides an interactive **serial CLI** accessible in QEMU via `-serial stdio`. Commands include:
- `help` — list commands
- `clear` — clear VGA text output
- `echo <text>` — print text
- `about` — system info
- `halt` — halt CPU

The serial UI is chosen for reliability and speed of implementation, avoiding a keyboard driver while enabling interactive demos.

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

### 5.2 Performance Metrics
Two microbenchmarks are used:
1) **Direct call baseline:** A tight loop of function calls inside a single task.
2) **IPC ping/pong:** Two tasks send and receive messages for a fixed number of iterations.

Where possible, `rdtsc` or a tick-based counter is used to estimate cycles per iteration. The comparison illustrates the cost of message passing and context switching relative to direct calls.

**Sample metrics (replace with measured results):**
| Metric | Direct Call | IPC Ping/Pong |
|---|---:|---:|
| Iterations | N | N |
| Avg cycles/iter | TBD | TBD |
| Relative overhead | 1.0× | TBD× |

### 5.3 Fault Isolation Demonstration
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
- Services currently run as kernel tasks (no user-mode isolation yet).
- IPC performance is not fully optimized.
- Hardware drivers are minimal (serial + VGA only).

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
