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

```bash
make
```

3) Run in QEMU:

```bash
make run
```

PowerShell wrappers (calls WSL):

```powershell
./scripts/build.ps1
./scripts/run.ps1
```

## Repo map

Build-critical code (current):
- `src/arch/i386/boot.S`: Multiboot2 header + entry point
- `src/arch/i386/linker.ld`: kernel linker script
- `src/kernel/`: kernel C code (VGA text output, serial, panic)
- `boot/grub/grub.cfg`: GRUB menu entry

Collaboration folders (team work areas; some are placeholders today):
- `kernel/`: future home for kernel subsystems (planned)
- `ipc/`: IPC design + implementation work
- `services/`: service modules (echo, timer, log, monitor, ...)
- `tests/`: validation steps and (optional) host-side tests
- `docs/`: architecture, team plan, contributing, perf writeups

Architecture overview: see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

UI overview (serial CLI + next steps): see [docs/UI.md](docs/UI.md).

## GitHub workflow

- Use issues from [docs/ISSUES.md](docs/ISSUES.md) as the starting backlog.
- Every PR links an issue and includes a QEMU boot check.
- See [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) for lightweight rules.

## Roadmap (7 days)

The full schedule and member responsibilities are in [docs/TEAM_PLAN.md](docs/TEAM_PLAN.md).

