# Service Tests

This document describes validation procedures for the three implemented services.

## Test 1: Boot and Service Initialization

**Objective:** Verify all services initialize correctly during boot.

**Procedure:**
1. Run `make run`
2. Observe serial output

**Expected Output:**
```
microkernel: serial online
microkernel: IPC initialized
microkernel: Service registry initialized
console_service: initialized (endpoint 0)
echo_service: initialized (endpoint 1)
timer_service: initialized (endpoint 2)
Registered services:
  - console
  - echo
  - timer
```

**Pass Criteria:**
- All three services report initialization
- Each service has a unique endpoint ID
- Service registry lists all three services

## Test 2: Console/Log Service

**Objective:** Verify the console service receives and prints log messages.

**Procedure:**
1. Boot the system
2. Type: `log Test message 1`
3. Type: `log Another test`

**Expected Output:**
```
mk> log Test message 1
Log message sent via IPC
[LOG] Test message 1
mk> log Another test
Log message sent via IPC
[LOG] Another test
```

**Pass Criteria:**
- Log messages are sent successfully
- Messages appear with [LOG] prefix
- Messages are displayed on both VGA and serial

## Test 3: Echo Service (IPC Request/Reply)

**Objective:** Verify the echo service can receive and reply to messages.

**Procedure:**
1. Boot the system
2. Type: `ipcecho Hello World`
3. Type: `ipcecho 123456789`

**Expected Output:**
```
mk> ipcecho Hello World
Echo request sent via IPC, processing...
Echo reply received: Hello World
mk> ipcecho 123456789
Echo request sent via IPC, processing...
Echo reply received: 123456789
```

**Pass Criteria:**
- Echo requests are sent successfully
- Replies are received correctly
- Reply content matches request content

## Test 4: Timer Service

**Objective:** Verify the timer service can send tick messages.

**Procedure:**
1. Boot the system
2. Type: `timertick`
3. Type: `timertick` again

**Expected Output:**
```
mk> timertick
Triggering timer tick...
Timer tick sent to subscribers
mk> timertick
Triggering timer tick...
Timer tick sent to subscribers
```

**Pass Criteria:**
- Timer tick command executes without error
- Tick messages are sent to subscribers (if any)

## Test 5: Service Registry Lookup

**Objective:** Verify service registry functionality.

**Procedure:**
1. Boot the system
2. Type: `services`

**Expected Output:**
```
mk> services
Registered services:
  - console
  - echo
  - timer
```

**Pass Criteria:**
- All three services are listed
- Service names match expected values

## Test 6: Multiple Service Interactions

**Objective:** Verify multiple services can work together.

**Procedure:**
1. Boot the system
2. Execute the following sequence:
   ```
   services
   log Starting tests
   ipcecho test1
   timertick
   log Tests complete
   ```

**Expected Output:**
```
mk> services
Registered services:
  - console
  - echo
  - timer
mk> log Starting tests
Log message sent via IPC
[LOG] Starting tests
mk> ipcecho test1
Echo request sent via IPC, processing...
Echo reply received: test1
mk> timertick
Triggering timer tick...
Timer tick sent to subscribers
mk> log Tests complete
Log message sent via IPC
[LOG] Tests complete
```

**Pass Criteria:**
- All commands execute successfully
- Services don't interfere with each other
- Output is correct for each command

## Test 7: IPC Queue Behavior

**Objective:** Verify IPC queues handle messages correctly.

**Procedure:**
1. Send multiple log messages rapidly
2. Verify all messages are processed

**Expected Results:**
- Messages are queued and processed in order
- No messages are lost (within queue size limits)
- System remains stable

## Test 8: Help Command Integration

**Objective:** Verify new commands appear in help.

**Procedure:**
1. Boot the system
2. Type: `help`

**Expected Output:**
```
mk> help
Commands:
  help         Show this help
  clear        Clear VGA screen
  echo <text>  Print text
  about        Show build info
  services     List registered services
  log <text>   Send log message to console service
  ipcecho <text> Send echo request via IPC
  timertick    Trigger timer tick
  halt         Halt CPU
```

**Pass Criteria:**
- All new service commands are listed
- Command descriptions are clear

## Test 9: Fault Isolation (Crash + Restart)

**Objective:** Verify a service can crash without killing the kernel, and that it can be restarted.

**Procedure:**
1. Boot the system
2. Type: `ipcecho before`
3. Type: `crash`
4. Type: `ipcecho after`

**Pass Criteria:**
- Kernel remains responsive after `crash`
- Echo service responds again after restart

## Test 10: Performance Benchmark

**Objective:** Compare direct-call overhead vs IPC overhead.

**Procedure:**
1. Boot the system
2. Type: `bench`
3. (Optional) Type: `bench 5000`

**Expected Output:**
- Prints two cycle counts: `direct cycles` and `ipc cycles`
- IPC should generally be noticeably larger than direct

## Automated Testing Notes

These tests can be partially automated using expect scripts or similar tools to:
1. Send commands via serial
2. Capture output
3. Verify expected patterns

Example automation approach:
```bash
#!/bin/bash
# Run QEMU with serial port and send commands
(
  sleep 2
  echo "services"
  sleep 1
  echo "log Test"
  sleep 1
  echo "ipcecho Hello"
  sleep 1
) | qemu-system-i386 -cdrom build/microkernel.iso -nographic -monitor none -serial stdio
```

## Test Summary Checklist

- [ ] All services initialize on boot
- [ ] Console service receives and displays log messages
- [ ] Echo service responds to requests correctly
- [ ] Timer service sends tick messages
- [ ] Service registry lists all services
- [ ] Multiple services can be used in sequence
- [ ] Help command shows all service commands
- [ ] System remains stable during testing
