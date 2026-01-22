# Architecture (prototype)

## Goal
Build a minimal microkernel-ish prototype on i386 that demonstrates:
- message passing (IPC)
- services split out of the “core”
- fault isolation benefits (service crash + restart)
- performance comparison (IPC vs direct call)

## Execution model
In week-1 scope, “services” run as **separately scheduled tasks** (kernel threads) with message queues.

If time permits, we can evolve to:
- ring3 user mode + syscalls
- user-mode servers
- stronger isolation via page tables

## Components

### Kernel core
- Boot + early init
- Scheduler (cooperative OK)
- Task abstraction (stack + state)
- Basic drivers: VGA/serial output

### IPC layer
- Endpoints identified by small integer IDs
- Messages are fixed-size structs or (type, len, payload)
- Each endpoint has a bounded queue

Semantics (suggested)
- `ipc_send(dst, msg)`: enqueue or yield/retry when full
- `ipc_recv(src_filter, out_msg)`: dequeue or yield when empty
- Optional: request/reply token for simple RPC

### Service registry
- Table: `name -> endpoint_id`
- Boot-time registration for built-in services

### Services
At least 3 services:
- console/log service
- echo service
- timer service
- monitor service (fault isolation)

## Fault isolation demo
- A service “crash” should not take down the whole kernel
- A monitor restarts the service task and the system continues

## Performance comparison
- direct call loop vs IPC ping/pong loop
- measure time using `rdtsc` if available, otherwise a rough tick-based approach
