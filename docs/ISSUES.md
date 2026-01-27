# GitHub issue list (copy/paste into GitHub)

Use these labels (recommended):
- `core`, `ipc`, `services`, `docs`, `tests`, `tooling`, `perf`, `good first issue`, `blocked`

## Milestone: Day 1–2 (bootable + IPC MVP)

### 1) Define task model + yield
- **Labels:** `core`
- **Owner:** Core (You)
- **Description:** Define a minimal task structure and a cooperative `yield()` mechanism.
- **Acceptance:** At least 2 tasks can run and alternate output.

### 2) IPC API spec (header-only first)
- **Labels:** `ipc`
- **Owner:** Member 2
- **Description:** Draft `ipc` interface (message type, endpoint id, send/recv signatures).
- **Acceptance:** Header reviewed and agreed by core.

### 3) Implement IPC queues (ring buffer)
- **Labels:** `ipc`
- **Owner:** Member 2
- **Depends on:** #2
- **Description:** Bounded queue per endpoint, safe enqueue/dequeue.
- **Acceptance:** Ping/pong between two tasks works.

### 4) Echo service MVP
- **Labels:** `services`
- **Owner:** Member 3
- **Depends on:** #3
- **Description:** A task that receives a message and replies.
- **Acceptance:** Echo round-trip visible in VGA/serial logs.

## Milestone: Day 3 (3 services + registry)

### 5) Service registry (name → endpoint)
- **Labels:** `core`, `services`
- **Owner:** Core (You)
- **Description:** Simple registry used by services to find each other.
- **Acceptance:** Services can lookup endpoints by name.

### 6) Console/log service
- **Labels:** `services`
- **Owner:** Member 3
- **Description:** Central logging endpoint that prints incoming log messages.
- **Acceptance:** Other tasks can send logs without directly touching VGA.

### 7) Timer service
- **Labels:** `services`
- **Owner:** Member 3
- **Description:** Periodically sends tick messages to subscribers.
- **Acceptance:** Client receives periodic tick messages.

### 8) Health monitor service
- **Labels:** `services`, `core`
- **Owner:** Member 4
- **Description:** Watches heartbeats and can restart crashed services.
- **Acceptance:** Monitor detects failure and triggers restart.

## Milestone: Day 4 (fault isolation)

### 9) Controlled service crash demo
- **Labels:** `services`, `tests`
- **Owner:** Member 4
- **Description:** Add a deterministic way to crash a service (command/message).
- **Acceptance:** Crash is reproducible and logged.

### 10) Service restart plumbing
- **Labels:** `core`
- **Owner:** Core (You)
- **Depends on:** #9
- **Description:** Allow monitor to spawn a fresh instance of a service task.
- **Acceptance:** After crash, service comes back and responds to messages.

## Milestone: Day 5 (performance)

### 11) Benchmark harness
- **Labels:** `perf`
- **Owner:** Member 7
- **Description:** Compare direct call loop vs IPC ping/pong loop.
- **Acceptance:** Results documented with method.

### 12) Timing primitive
- **Labels:** `core`, `perf`
- **Owner:** Core (You)
- **Description:** Expose timing API (`rdtsc` or tick count).
- **Acceptance:** Benchmark can measure durations consistently.

## Milestone: Day 6–7 (docs/tests/final polish)

### 13) Test plan + validation checklist
- **Labels:** `tests`, `docs`
- **Owner:** Member 5
- **Description:** Boot smoke tests + IPC tests + crash/restart procedure.
- **Acceptance:** Anyone can run validation in ≤ 10 minutes.

### 14) Add GitHub Actions CI (optional)
- **Labels:** `tooling`
- **Owner:** Member 6
- **Description:** CI workflow running build in Ubuntu.
- **Acceptance:** PRs show build status.

### 15) Documentation pack
- **Labels:** `docs`
- **Owner:** Member 6
- **Description:** Contributing guide + architecture docs + final README polish.
- **Acceptance:** Repo is understandable to a new reader.

### 16) Demo script and expected output
- **Labels:** `docs`, `tests`
- **Owner:** Member 5
- **Description:** Step-by-step demo with expected VGA/serial lines.
- **Acceptance:** Demo works on a clean machine with WSL.
