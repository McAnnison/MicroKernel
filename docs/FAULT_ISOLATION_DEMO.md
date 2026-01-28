# Fault Isolation Demo

This demonstrates the microkernel's ability to isolate service failures and restart crashed services.

## Features Implemented

1. **Service Crash Detection**: Services can crash without bringing down the kernel
2. **Monitor Service**: Tracks registered services and restarts them on failure
3. **Graceful Task Exit**: Panic in task context exits the task instead of halting the kernel
4. **Service Restart**: Crashed services are reinitialized with fresh state

## How It Works

### Architecture

```
┌─────────────────────────────────────┐
│         Kernel Core                 │
│  ┌──────────┐      ┌──────────┐    │
│  │   IPC    │      │  Tasks   │    │
│  └──────────┘      └──────────┘    │
└─────────────────────────────────────┘
         │                  │
         ▼                  ▼
┌─────────────────────────────────────┐
│           Services                  │
│  ┌──────┐  ┌──────┐  ┌──────────┐  │
│  │ Echo │  │Timer │  │ Monitor  │  │
│  └──────┘  └──────┘  └──────────┘  │
└─────────────────────────────────────┘
```

### Crash & Restart Flow

1. User types `crash` command
2. CLI sends MSG_CRASH to Echo service
3. Echo service receives MSG_CRASH and calls `panic()`
4. `panic()` detects it's in task context and yields instead of halting
5. Task is marked FINISHED and removed from scheduler
6. Monitor service is notified of the crash
7. Monitor calls `task_restart()` to reinitialize the echo service
8. Echo service is back online with fresh state

## Demo Instructions

### In QEMU Console

```bash
# 1. Boot the kernel
make run

# 2. Check services are running
mk> services
# Output shows: console, echo, timer, monitor

# 3. Test echo service (pre-crash)
mk> ipcecho Hello
# Should reply: "Echo reply received: Hello"

# 4. Trigger controlled crash
mk> crash
# Output:
#   [CRASH DEMO] Sending crash message to echo service...
#   echo_service: CRASH MESSAGE RECEIVED - simulating crash!
#   PANIC: echo_service: intentional crash for demo
#   PANIC: Task context detected - yielding to scheduler
#   [MONITOR] CRASH DETECTED: echo
#   [MONITOR] Restarting crashed service: echo
#   [MONITOR] Service restarted successfully
#   [CRASH DEMO] Test the echo service again with 'ipcecho test'

# 5. Verify service is back online
mk> ipcecho Recovery test
# Should reply: "Echo reply received: Recovery test"
```

## Key Implementation Details

### Modified Components

**1. `task.c` - Added restart capability**
- `task_get_current()`: Returns current task ID or -1
- `task_restart(task_id)`: Reinitializes task with fresh stack

**2. `panic.c` - Context-aware panic**
- Checks if panic occurred in task vs kernel context
- Task panics: yield and mark FINISHED
- Kernel panics: halt system (unchanged)

**3. `monitor_service.c` - Crash monitoring**
- Maintains registry of services to monitor
- Detects and reports crashes
- Triggers restart via `task_restart()`

**4. `echo_service.c` - Crash handling**
- Processes MSG_CRASH message type
- Intentionally panics for demo

### Message Types
- `MSG_CRASH`: Trigger intentional crash (demo only)

## Acceptance Criteria ✅

- [x] Service can crash without killing kernel
- [x] Monitor detects crash
- [x] Service is restarted automatically
- [x] Restarted service responds to requests
- [x] Kernel remains stable throughout process

## Limitations

This is a simplified fault isolation demo suitable for a cooperative scheduler:
- Services run as kernel tasks (not separate protection domains)
- No memory protection or page tables
- Crash detection requires explicit panic() calls
- Services must be restartable (stateless or state recreatable)

For production microkernel:
- Services would run in user mode (ring 3)
- MMU-based memory protection
- Page fault handler for crash detection
- Capability-based security
