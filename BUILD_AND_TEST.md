# Building and Testing the IPC Implementation

## Quick Start

### Option 1: Using Make (if available)
```bash
make clean
make
make run
```

### Option 2: Manual Build
```bash
# Create build directory
mkdir -p build

# Compile all components
gcc -Wall -Wextra -std=c11 -g -I. -c ipc/ipc.c -o build/ipc.o
gcc -Wall -Wextra -std=c11 -g -I. -c scheduler/scheduler.c -o build/scheduler.o
gcc -Wall -Wextra -std=c11 -g -I. -c demo/main.c -o build/main.o

# Link executable
gcc -Wall -Wextra -std=c11 -g build/ipc.o build/scheduler.o build/main.o -o build/ipc_demo

# Run
./build/ipc_demo
```

### Option 3: Using the Test Script
```bash
chmod +x test_build.sh
./test_build.sh
```

## Expected Output

When the program runs successfully, you should see:

```
=== IPC Ping/Pong Demo ===

Initialized IPC queues (max 16 messages each)

=== Starting scheduler with 2 tasks ===
[Ping] Task started
[Pong] Task started
[Ping] Sending PING (token: 100)
[Pong] Received PING (token: 100, payload: PING #0)
[Pong] Sending PONG (token: 100)
[Ping] Received PONG (token: 100, payload: PONG reply to token 100)
[Ping] Sending PING (token: 101)
[Pong] Received PING (token: 101, payload: PING #1)
[Pong] Sending PONG (token: 101)
[Ping] Received PONG (token: 101, payload: PONG reply to token 101)
[Ping] Sending PING (token: 102)
[Pong] Received PING (token: 102, payload: PING #2)
[Pong] Sending PONG (token: 102)
[Ping] Received PONG (token: 102, payload: PONG reply to token 102)
[Ping] Task completed
[Pong] Task completed
=== All tasks completed ===

=== Demo completed ===
```

## What to Verify

✅ **3 complete ping/pong cycles** - Should see 3 PING messages and 3 PONG replies

✅ **Reply tokens match** - Each PONG should have the same token as the corresponding PING (100, 101, 102)

✅ **No crashes or hangs** - Program should complete cleanly

✅ **Proper task switching** - Messages should alternate between ping and pong tasks

✅ **Queue handling** - If queue becomes full or empty, tasks should yield without crashing

## Troubleshooting

### Compilation Errors

**Error: "gcc: command not found"**
- Install GCC/MinGW on Windows
- Or use MSVC: `cl /std:c11 /I. ipc/ipc.c scheduler/scheduler.c demo/main.c /Fe:ipc_demo.exe`

**Error: "setjmp.h: No such file"**
- Ensure you're using a C compiler (not C++)
- Check that standard C library headers are available

**Error: "undefined reference to task_yield"**
- Ensure all object files are linked together
- Check that scheduler.c is compiled and linked

### Runtime Issues

**Program hangs/infinite loop**
- Check that tasks are yielding properly
- Verify scheduler is switching between tasks
- Look for deadlocks in message flow

**Messages lost or out of order**
- Verify ring buffer implementation
- Check queue initialization
- Ensure send/recv operations are atomic

**Tasks don't complete**
- Check loop conditions in tasks
- Verify task completion flags
- Ensure scheduler detects completed tasks

## Code Structure

```
.
├── ipc/
│   ├── ipc.h          # IPC API definitions
│   ├── ipc.c          # Ring buffer implementation
│   └── README.md       # Detailed IPC documentation
├── scheduler/
│   ├── scheduler.h     # Scheduler API
│   └── scheduler.c     # Cooperative scheduler with setjmp/longjmp
├── demo/
│   ├── main.c         # Ping/pong demo (updated with loops)
│   └── ping_pong.c    # Original code
├── Makefile           # Build configuration
├── test_build.sh      # Build and test script
└── README.md          # Project overview
```

## Key Features Tested

1. **Ring Buffer**: Bounded queue with 16 message capacity
2. **Blocking Semantics**: Tasks yield when queue is full/empty
3. **Reply Tokens**: Request/response correlation works correctly
4. **Cooperative Multitasking**: Tasks yield and resume properly
5. **Message Passing**: Ping/pong communication works end-to-end

## Next Steps

After successful testing:
1. Review the output to ensure all 3 cycles complete
2. Verify token matching is correct
3. Check that no errors or warnings occur
4. Document any issues found
5. Commit your work to the IPC branch
