# IPC Implementation Test Plan

## Build Test

1. **Compile all components**
   ```bash
   make
   # OR manually:
   gcc -Wall -Wextra -std=c11 -g -I. -c ipc/ipc.c -o build/ipc.o
   gcc -Wall -Wextra -std=c11 -g -I. -c scheduler/scheduler.c -o build/scheduler.o
   gcc -Wall -Wextra -std=c11 -g -I. -c demo/main.c -o build/main.o
   gcc -Wall -Wextra -std=c11 -g build/*.o -o build/ipc_demo
   ```

2. **Expected**: No compilation errors or warnings

## Runtime Tests

### Test 1: Basic Ping/Pong Exchange
**Expected Output:**
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

**Verification:**
- ✅ 3 ping/pong exchanges occur
- ✅ Reply tokens match (100, 101, 102)
- ✅ Messages are received in correct order
- ✅ Tasks complete properly

### Test 2: Queue Full Handling
**Test**: Send messages faster than they can be consumed

**Expected Behavior:**
- When queue is full, `ipc_send()` should yield
- No crashes or infinite loops
- Messages eventually get through

### Test 3: Queue Empty Handling
**Test**: Try to receive when queue is empty

**Expected Behavior:**
- When queue is empty, `ipc_recv()` should yield
- No crashes or infinite loops
- Task resumes when message arrives

### Test 4: Reply Token Verification
**Verification:**
- Each PING has a unique token (100, 101, 102)
- Each PONG echoes back the same token
- Token matching is correct

## Code Quality Checks

1. **No memory leaks** (if using valgrind or similar)
2. **No buffer overflows** (ring buffer logic correct)
3. **Proper initialization** (queues initialized before use)
4. **Thread safety** (for cooperative multitasking)

## Known Limitations

- No preemption protection (cooperative only)
- No memory protection between tasks
- Fixed queue size (16 messages)
- No timeout mechanisms

## Manual Testing Steps

1. Build the project: `make` or use `test_build.sh`
2. Run the demo: `./build/ipc_demo` or `make run`
3. Verify output matches expected pattern
4. Check that all 3 ping/pong cycles complete
5. Verify no crashes or hangs

## Troubleshooting

**Issue**: Program hangs
- Check if tasks are yielding properly
- Verify scheduler is switching between tasks
- Check for deadlocks in message flow

**Issue**: Messages lost or out of order
- Verify ring buffer logic (head/tail/count)
- Check queue initialization
- Verify send/recv operations

**Issue**: Compilation errors
- Check all header files are included
- Verify compiler supports C11 standard
- Check include paths are correct
