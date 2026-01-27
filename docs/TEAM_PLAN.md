# Team plan (7 people / 7 days)

This plan is designed to ship a **working prototype** of a microkernel-ish OS in **7 days** using the current i386 Multiboot2 + GRUB scaffold.

> Reality check (important): a “true” microkernel with user-mode servers, loading separate ELF programs, MMU-based isolation, etc. is a multi-week project.
> In 7 days, we can still meet the assignment intent by implementing **message-passing + services split** as separately scheduled components, and demonstrating **fault isolation** via controlled service failures + restart (and, if time allows, basic user-mode ring3).

## Roles (7 members)

### Member 1 — You (Core + integration) ✅
**Goal:** own the core kernel direction, integrate everyone’s work, and keep the build bootable daily.

**You will build (2-in-1 task):**
1) **Core kernel primitives**
- Boot stays stable (already works)
- Add a minimal scheduler (cooperative is OK)
- Add a “task” abstraction for services/threads
- Add basic timekeeping / tick (PIT or a simple busy-loop if needed)

2) **Integration + release**
- Merge PRs daily
- Keep `make` and `make run` working
- Run a nightly boot test in QEMU and capture screenshots/logs

**Acceptance criteria**
- QEMU boots and shows a clean boot banner
- Can start N tasks and switch between them
- Provides a stable API that other members can use (IPC + service interfaces)

---

### Member 2 — IPC implementation (simple + focused)
**Goal:** implement basic IPC message passing used by services.

**Deliverables**
- `ipc/` message definitions and API (headers + C implementation)
- `send/recv` primitives using a bounded queue (ring buffer)
- Blocking semantics (simple): if queue empty, task yields; if full, yield/retry
- Optional: reply tokens (request/response)

**Acceptance criteria**
- A demo “ping/pong” between two tasks prints output
- Handles queue full/empty without crashing
- Has a short doc explaining semantics and limits

**What NOT to do**
- No need for memory protection or capability system yet
- No need for networking/filesystems

---

### Member 3 — Services (3 small user-space-like services)
**Goal:** implement at least **3 services** as separate components (initially as kernel tasks; if ring3 happens later, this becomes the user-mode port).

**Pick any 3**
- **Console/log service**: collects log messages from other tasks; prints to VGA/serial
- **Echo service**: receives messages and replies (good for IPC demo)
- **Timer service**: sends periodic tick messages to clients
- **Health monitor**: watches heartbeats and restarts failed services

**Acceptance criteria**
- Each service has its own module/file(s)
- Each service registers with a small “service registry” (name → endpoint id)
- A demo scenario shows service interactions via IPC

---

### Member 4 — Fault isolation demo (simple “crash + restart”)
**Goal:** demonstrate “fault isolation benefits” in a way that’s achievable quickly.

**Deliverables**
- A controlled “service crash” (e.g., panic inside a service task) that does NOT kill the whole kernel
- A recovery mechanism: service task can be restarted (new stack + state)
- A demo script that shows:
  1) system runs
  2) service crashes
  3) monitor restarts it
  4) system continues

**Acceptance criteria**
- After crash, kernel is still alive and can continue other services
- Logs clearly show crash + restart

---

### Member 5 — Testing + validation procedures
**Goal:** create test cases and validation procedures the team can run daily.

**Deliverables**
- `tests/` with:
  - “boot smoke test” steps (QEMU output expectations)
  - IPC ping/pong test procedure
  - service crash/restart procedure
- Optional (nice): host-side unit tests for data structures (ring buffer) built with native compiler

**Acceptance criteria**
- Clear checklist anyone can follow
- At least 5 test cases with expected outputs

---

### Member 6 — Tooling + CI + repo hygiene
**Goal:** make it easy to build/run and keep PRs consistent.

**Deliverables**
- GitHub Actions workflow (if you want) or documented local workflow
- Add issue templates + PR template (already scaffolded, but maintain them)
- Keep a short `docs/CONTRIBUTING.md` (branch naming, PR checklist)

**Acceptance criteria**
- New contributors can build in ≤ 15 minutes
- PR template enforces “runs in QEMU” checkbox

---

### Member 7 — Performance comparison (microkernel-ish vs monolithic-ish)
**Goal:** compare performance with a “direct call” path vs “IPC message passing” path.

**Deliverables**
- A tiny benchmark:
  - baseline: direct function call loop
  - IPC: send/recv ping/pong loop
- Record timing (approx is fine): cycle counter via `rdtsc` if available, otherwise tick-based
- Write up results in `docs/PERF.md`

**Acceptance criteria**
- Has a repeatable method and documented numbers
- Explains what was measured and why it’s relevant

---

## 7-day execution schedule (what to do each day)

### Day 1 — Setup + architecture freeze
- You: define kernel task model + minimal scheduler interface
- Member 2: IPC API header draft (types, endpoints)
- Member 3: choose services + stub modules
- Member 6: issue/PR templates + basic workflow

**Exit criteria:** everyone can build and run QEMU.

### Day 2 — IPC MVP + first service
- You: task switching/yield (cooperative OK)
- Member 2: ring-buffer queue + send/recv working
- Member 3: Echo service using IPC

**Exit criteria:** ping/pong prints in QEMU.

### Day 3 — Service registry + 3 services
- You: service registry mechanism (simple table)
- Member 3: implement 2 more services
- Member 5: first test checklist

**Exit criteria:** 3 services interact over IPC.

### Day 4 — Fault isolation demo
- Member 4: crash + restart story
- You: restart plumbing / supervision hooks
- Member 5: crash/restart test procedure

**Exit criteria:** service can crash and be restarted; kernel stays alive.

### Day 5 — Performance harness
- Member 7: benchmark + writeup
- You: ensure timing hooks exist

**Exit criteria:** baseline vs IPC numbers captured.

### Day 6 — Hardening + docs
- You: fix bugs, ensure demo path is stable
- Member 6: polish build instructions
- Member 5: finalize test cases

**Exit criteria:** “one command” boot demo is reliable.

### Day 7 — Final integration + submission pack
- Everyone: run through demo script
- You: cut final release tag locally (optional)
- Member 6/Docs: finalize report structure

**Exit criteria:** README is complete, demo steps pass, perf + fault isolation clearly shown.
