<<<<<<< HEAD
# IPC Implementation - Operating System Fundamentals

## Project Structure

```
.
├── ipc/           # IPC implementation (message passing, ring buffer)
│   ├── ipc.h      # IPC API and data structures
│   ├── ipc.c      # IPC implementation
│   └── README.md  # Detailed IPC documentation
├── scheduler/     # Simple cooperative scheduler
│   ├── scheduler.h
│   └── scheduler.c
├── demo/          # Demo programs
│   ├── main.c     # Main ping/pong demo
│   └── ping_pong.c # Original ping/pong code
├── Makefile       # Build configuration
└── README.md      # This file
```

## Building and Running

### Prerequisites
- GCC compiler
- Make (or use gcc directly)

### Build
=======
# Group 18 – Microkernel Operating System Design

## 📌 Project Overview
This project implements a **microkernel-based operating system simulation** that demonstrates minimal kernel functionality and user-space services. The system emphasizes **inter-process communication (IPC)**, **fault isolation**, and **modular OS design**, following microkernel principles.

The project is developed as a **working prototype/simulation** running on Linux, fulfilling all course requirements.

---

## 🎯 Objectives
- Design and implement a **minimal microkernel**
- Implement **IPC-based message passing**
- Move traditional kernel services to **user space**
- Create **at least three user-space services**
- Demonstrate **fault isolation**
- Compare performance with a **monolithic design**

---

## 🧠 System Architecture

### Kernel Responsibilities
- Process/service registration
- Message routing (IPC)
- Basic scheduling simulation

# microkernel-os

Minimal, bootable **freestanding C** kernel scaffold targeting **i386 (32-bit x86)** via **Multiboot2 + GRUB**.

This repo is being used by a **7-person team** to build a **microkernel-ish prototype in 7 days**:
- minimal kernel core
- IPC message passing
- at least 3 services
- a fault-isolation demo (service crash + restart)
- a performance comparison (IPC vs direct call)

Detailed team breakdown + timeline: see [docs/TEAM_PLAN.md](docs/TEAM_PLAN.md).

## Quick start (Windows + WSL2 recommended)

1) Install prerequisites inside WSL (Ubuntu):

```bash
sudo apt update
sudo apt install -y build-essential gcc-multilib grub-pc-bin xorriso qemu-system-x86
```

2) Build ISO:

>>>>>>> 8aa1da5d756dac0c5d64daea3c0d1c82d3485ae4
```bash
make
```

<<<<<<< HEAD
Or manually:
```bash
mkdir -p build
gcc -Wall -Wextra -std=c11 -g -I. -c ipc/ipc.c -o build/ipc.o
gcc -Wall -Wextra -std=c11 -g -I. -c scheduler/scheduler.c -o build/scheduler.o
gcc -Wall -Wextra -std=c11 -g -I. -c demo/main.c -o build/main.o
gcc -Wall -Wextra -std=c11 -g build/ipc.o build/scheduler.o build/main.o -o build/ipc_demo
```

### Run
=======
3) Run in QEMU:

>>>>>>> 8aa1da5d756dac0c5d64daea3c0d1c82d3485ae4
```bash
make run
```

<<<<<<< HEAD
Or:
```bash
./build/ipc_demo
```

## Features Implemented

✅ **IPC Message Passing**
- Ring buffer (bounded queue) implementation
- Blocking send/receive with cooperative yielding
- Reply token support for request/response patterns

✅ **Cooperative Scheduler**
- Round-robin task scheduling
- Task yielding support
- Context switching using setjmp/longjmp

✅ **Ping/Pong Demo**
- Two tasks communicating via IPC
- Demonstrates blocking behavior
- Shows reply token usage

✅ **Documentation**
- API documentation
- Semantics explanation
- Limits and constraints

## Key Components

### IPC Queue (Ring Buffer)
- Fixed capacity: 16 messages
- Payload size: 32 bytes per message
- Thread-safe for cooperative multitasking

### Blocking Semantics
- `ipc_send()`: Blocks (yields) when queue is full
- `ipc_recv()`: Blocks (yields) when queue is empty
- Both use `task_yield()` for cooperative blocking

### Reply Tokens
- Optional field in messages
- Enables request/response correlation
- Demonstrated in ping/pong demo

## Testing

The demo program runs a ping/pong exchange:
1. Ping task sends a PING message with a token
2. Pong task receives PING and sends PONG reply with same token
3. Ping task receives PONG and verifies token
4. Process repeats 3 times

Expected output shows:
- Task initialization
- Message sending/receiving
- Token matching
- Queue full/empty handling (if it occurs)

## Branch Information

This implementation is on the `IPC` branch as required:
```bash
git branch
# Should show: * IPC
```

## Notes

- This is a simplified implementation for educational purposes
- No memory protection or capability system
- Suitable for single-address-space systems
- Cooperative multitasking only (no preemption)

For detailed IPC semantics and API documentation, see `ipc/README.md`.
=======
PowerShell wrappers (calls WSL):

```powershell
./scripts/build.ps1
./scripts/run.ps1
```

## Repo map

Build-critical code (current):
- `src/arch/i386/boot.S`: Multiboot2 header + entry point
- `src/arch/i386/linker.ld`: kernel linker script
- `src/kernel/`: kernel C code (VGA text output, serial, panic, **IPC**, **service registry**)
- `boot/grub/grub.cfg`: GRUB menu entry

Collaboration folders (team work areas):
- `kernel/`: future home for kernel subsystems (planned)
- `ipc/`: IPC design + implementation work (**implemented**)
- `services/`: service modules (**3 services implemented: console, echo, timer**)
- `tests/`: validation steps and (optional) host-side tests
- `docs/`: architecture, team plan, contributing, perf writeups, **services demo**

Architecture overview: see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

UI overview (serial CLI + next steps): see [docs/UI.md](docs/UI.md).

**Services demo:** see [docs/SERVICES_DEMO.md](docs/SERVICES_DEMO.md) for complete guide.

## Services

Three services have been implemented:

1. **Console/Log Service** - Centralized logging via IPC
2. **Echo Service** - Request/reply pattern demonstration
3. **Timer Service** - Periodic tick message distribution

Try them in QEMU:
```bash
make run
# Type in console:
services       # List all services
log Hello      # Send log message
ipcecho Test   # Test echo service
```

See [services/README.md](services/README.md) for details.

## GitHub workflow

- Use issues from [docs/ISSUES.md](docs/ISSUES.md) as the starting backlog.
- Every PR links an issue and includes a QEMU boot check.
- See [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) for lightweight rules.

## Roadmap (7 days)

The full schedule and member responsibilities are in [docs/TEAM_PLAN.md](docs/TEAM_PLAN.md).

>>>>>>> 8aa1da5d756dac0c5d64daea3c0d1c82d3485ae4
