# Services

This directory contains the implementation of user-space-like services for the microkernel.

## Implemented Services

### 1. Console/Log Service
**Location:** `src/services/console_service.c`

**Purpose:** Centralized logging service that collects log messages from other components via IPC.

**Features:**
- Receives MSG_LOG messages
- Prints messages to both VGA and serial output
- Adds `[LOG]` prefix to all messages
- Non-blocking message processing

**Usage from CLI:**
```
log Hello World
```

### 2. Echo Service
**Location:** `src/services/echo_service.c`

**Purpose:** Demonstrates IPC request/reply pattern by echoing back received messages.

**Features:**
- Receives MSG_ECHO messages
- Sends MSG_ECHO_REPLY with same payload
- Useful for testing IPC functionality
- Shows bidirectional communication

**Usage from CLI:**
```
ipcecho Test message
```

### 3. Timer Service
**Location:** `src/services/timer_service.c`

**Purpose:** Sends periodic tick messages to subscribed endpoints.

**Features:**
- Maintains list of subscribers
- Sends MSG_TIMER_TICK to all subscribers
- Includes tick counter in payload
- Supports up to 8 subscribers

**Usage from CLI:**
```
timertick
```

## Service Architecture

### Service Registration
Each service:
1. Creates an endpoint via `ipc_endpoint_create()`
2. Registers with the service registry using `service_register(name, endpoint)`
3. Logs initialization status to serial

### Service Discovery
Clients can find services using:
```c
endpoint_id_t ep = service_lookup("service_name");
```

### Message Processing
Services process messages by:
1. Calling `ipc_recv()` on their endpoint
2. Handling messages based on type
3. Optionally sending replies via `ipc_send()`

## Adding New Services

To add a new service:

1. **Create header file** in `include/services/`:
   ```c
   #pragma once
   #include "kernel/ipc.h"
   
   #define MYSERVICE_NAME "myservice"
   
   void myservice_init(void);
   endpoint_id_t myservice_get_endpoint(void);
   void myservice_process(void);
   ```

2. **Create implementation** in `src/services/`:
   ```c
   #include "services/myservice.h"
   #include "kernel/service_registry.h"
   
   static endpoint_id_t myservice_endpoint = ENDPOINT_INVALID;
   
   void myservice_init(void) {
       myservice_endpoint = ipc_endpoint_create();
       service_register(MYSERVICE_NAME, myservice_endpoint);
   }
   
   void myservice_process(void) {
       ipc_msg_t msg;
       while (ipc_recv(myservice_endpoint, &msg) == IPC_SUCCESS) {
           // Handle message
       }
   }
   ```

3. **Update Makefile** to include the new source files

4. **Initialize in kmain.c**:
   ```c
   #include "services/myservice.h"
   // ...
   myservice_init();
   ```

## Service Communication Patterns

### One-Way Messaging (Console Service)
```
Client -> IPC Send -> Service -> Process
```

### Request/Reply (Echo Service)
```
Client -> IPC Send (with sender ID) -> Service
Service -> IPC Send (to sender) -> Client
```

### Publish/Subscribe (Timer Service)
```
Service -> IPC Send -> Subscriber 1
        -> IPC Send -> Subscriber 2
        -> IPC Send -> Subscriber N
```

## Testing Services

See `tests/SERVICES_TESTS.md` for detailed test procedures.

Quick test:
```bash
make run
# In QEMU:
services
log Test
ipcecho Hello
timertick
```

## Documentation

- **Demo Guide:** `docs/SERVICES_DEMO.md` - Complete usage guide with examples
- **Test Plan:** `tests/SERVICES_TESTS.md` - Validation procedures
- **Architecture:** `docs/ARCHITECTURE.md` - Overall system design

## Design Principles

1. **Isolation:** Each service operates independently
2. **Simplicity:** Minimal, focused functionality per service
3. **IPC-based:** All inter-service communication via message passing
4. **Registry-based:** Services discoverable by name
5. **Non-blocking:** Services don't block on message operations
