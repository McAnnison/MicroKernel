# Services Demo Guide

This document demonstrates the three implemented services and their IPC interactions.

## Overview

The microkernel now includes three services:

1. **Console/Log Service** - Collects and prints log messages from other components
2. **Echo Service** - Receives messages and replies (IPC demonstration)
3. **Timer Service** - Sends periodic tick messages to subscribers

## Services Architecture

### IPC (Inter-Process Communication)
- Each service has its own endpoint ID
- Messages are passed through bounded queues (ring buffers)
- Message types: LOG, ECHO, ECHO_REPLY, TIMER_TICK

### Service Registry
- Maps service names to endpoint IDs
- Services register themselves on initialization
- Clients can lookup services by name

## Available CLI Commands

### Basic Commands
- `help` - Show all available commands
- `about` - Show build information
- `clear` - Clear VGA screen
- `services` - List all registered services
- `halt` - Halt the CPU

### Service Interaction Commands

#### Console/Log Service
```
log <text>
```
Sends a log message to the console service via IPC. The console service receives the message and prints it with a `[LOG]` prefix to both VGA and serial output.

**Example:**
```
mk> log Hello from CLI!
Log message sent via IPC
[LOG] Hello from CLI!
```

#### Echo Service
```
ipcecho <text>
```
Sends an echo request to the echo service via IPC. The service receives the message, creates a reply, and sends it back. The CLI displays the received reply.

**Example:**
```
mk> ipcecho Test message
Echo request sent via IPC, processing...
Echo reply received: Test message
```

#### Timer Service
```
timertick
```
Manually triggers a timer tick. The timer service sends tick messages to all subscribed endpoints.

**Example:**
```
mk> timertick
Triggering timer tick...
Timer tick sent to subscribers
```

## Demo Scenario

Here's a complete demo showing all service interactions:

1. **Boot the system:**
   ```bash
   make run
   ```

2. **Check registered services:**
   ```
   mk> services
   Registered services:
     - console
     - echo
     - timer
   ```

3. **Test Console Service (IPC logging):**
   ```
   mk> log Service test starting
   Log message sent via IPC
   [LOG] Service test starting
   ```

4. **Test Echo Service (request/reply):**
   ```
   mk> ipcecho Hello microkernel!
   Echo request sent via IPC, processing...
   Echo reply received: Hello microkernel!
   ```

5. **Test Timer Service:**
   ```
   mk> timertick
   Triggering timer tick...
   Timer tick sent to subscribers
   ```

6. **Multiple interactions:**
   ```
   mk> log First message
   [LOG] First message
   mk> log Second message
   [LOG] Second message
   mk> ipcecho Testing echo
   Echo reply received: Testing echo
   ```

## Technical Details

### Service Initialization Sequence

1. Kernel initializes IPC subsystem
2. Service registry is initialized
3. Each service:
   - Creates an endpoint via `ipc_endpoint_create()`
   - Registers itself via `service_register(name, endpoint)`
   - Logs initialization status

### Message Flow Example (Echo Service)

1. CLI creates temporary reply endpoint
2. CLI sends MSG_ECHO to echo service endpoint
3. Echo service processes message via `echo_service_process()`
4. Echo service sends MSG_ECHO_REPLY back to sender
5. CLI receives reply and displays result

### IPC Queue Characteristics

- Queue size: 16 messages per endpoint
- Payload size: 64 bytes maximum
- Non-blocking send/recv operations
- Returns error codes for queue full/empty conditions

## Implementation Files

### IPC Infrastructure
- `include/kernel/ipc.h` - IPC types and API
- `src/kernel/ipc.c` - IPC implementation

### Service Registry
- `include/kernel/service_registry.h` - Registry API
- `src/kernel/service_registry.c` - Registry implementation

### Services
- `include/services/console_service.h` + `src/services/console_service.c`
- `include/services/echo_service.h` + `src/services/echo_service.c`
- `include/services/timer_service.h` + `src/services/timer_service.c`

## Testing the Services

### Quick Test Script

```bash
# Build and run
make clean && make && make run

# In QEMU console, type:
services
log Test 1
ipcecho Hello
timertick
```

### Expected Output

```
microkernel: booted (i386)
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

serial CLI ready. Type `help`.
mk> services
Registered services:
  - console
  - echo
  - timer
mk> log Test 1
Log message sent via IPC
[LOG] Test 1
mk> ipcecho Hello
Echo request sent via IPC, processing...
Echo reply received: Hello
mk> timertick
Triggering timer tick...
Timer tick sent to subscribers
```

## Future Enhancements

Potential improvements for the services:

1. **Console Service**
   - Add log levels (INFO, WARN, ERROR)
   - Implement log filtering
   - Add timestamps

2. **Echo Service**
   - Support for transformation operations
   - Statistics tracking

3. **Timer Service**
   - Automatic periodic ticking
   - Configurable tick intervals
   - Timer subscription management commands

4. **General IPC**
   - Asynchronous message handling
   - Priority queues
   - Larger payload support
