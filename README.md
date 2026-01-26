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
```bash
make
```

Or manually:
```bash
mkdir -p build
gcc -Wall -Wextra -std=c11 -g -I. -c ipc/ipc.c -o build/ipc.o
gcc -Wall -Wextra -std=c11 -g -I. -c scheduler/scheduler.c -o build/scheduler.o
gcc -Wall -Wextra -std=c11 -g -I. -c demo/main.c -o build/main.o
gcc -Wall -Wextra -std=c11 -g build/ipc.o build/scheduler.o build/main.o -o build/ipc_demo
```

### Run
```bash
make run
```

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
