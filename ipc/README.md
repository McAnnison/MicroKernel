# IPC (Inter-Process Communication) Implementation

## Overview

This IPC implementation provides basic message passing between tasks using bounded queues (ring buffers) with cooperative blocking semantics. It's designed for a simple operating system environment with cooperative multitasking.

## Architecture

### Message Structure

Messages are defined by the `ipc_message_t` structure:
- **type**: Message type identifier (e.g., `IPC_MSG_PING`, `IPC_MSG_PONG`)
- **sender**: Task ID of the sender
- **reply_token**: Optional token for request/response correlation
- **payload**: Fixed-size data payload (32 bytes)

### Queue Structure

Each IPC queue is a ring buffer (circular queue) with:
- **buffer**: Array of messages (capacity: `IPC_MAX_MSG = 16`)
- **head**: Index of the oldest message (read position)
- **tail**: Index where next message will be written
- **count**: Current number of messages in the queue

## API

### `void ipc_init(ipc_queue_t *q)`
Initializes an IPC queue. Must be called before using a queue.

### `bool ipc_send(ipc_queue_t *q, ipc_message_t *msg)`
Sends a message to the queue. 
- **Blocking behavior**: If the queue is full, the function yields to the scheduler and retries until space is available.
- **Returns**: `true` on success, `false` on invalid parameters.

### `bool ipc_recv(ipc_queue_t *q, ipc_message_t *msg)`
Receives a message from the queue.
- **Blocking behavior**: If the queue is empty, the function yields to the scheduler and retries until a message is available.
- **Returns**: `true` on success, `false` on invalid parameters.

## Semantics

### Blocking Behavior

Both `ipc_send()` and `ipc_recv()` use **cooperative blocking**:
- When a queue is full (send) or empty (receive), the function calls `task_yield()`
- This allows other tasks to run and potentially unblock the waiting task
- The function retries after yielding, creating a busy-wait loop that cooperatively yields

### Ring Buffer Implementation

The queue uses a ring buffer (circular array) pattern:
- When `tail` reaches the end of the buffer, it wraps to index 0
- When `head` reaches the end of the buffer, it wraps to index 0
- The `count` field tracks the number of messages, preventing buffer overflow/underflow

### Reply Tokens

The `reply_token` field enables request/response patterns:
- A sender can include a unique token in the request
- The receiver echoes this token in the response
- The original sender can match responses to requests using the token

## Limits and Constraints

### Queue Capacity
- **Maximum messages per queue**: 16 (`IPC_MAX_MSG`)
- **Payload size**: 32 bytes per message (`IPC_PAYLOAD_SIZE`)
- **Total queue size**: ~1 KB per queue (16 messages × ~64 bytes per message)

### Thread Safety
- **Not thread-safe**: This implementation assumes cooperative multitasking
- Tasks must yield voluntarily; no preemption protection
- In a preemptive system, additional synchronization would be required

### Memory Protection
- **No memory protection**: Tasks can access any queue
- **No capability system**: No access control or permissions
- Suitable only for trusted tasks in a single address space

### Deadlock Prevention
- **No built-in deadlock detection**: Tasks can deadlock if circular dependencies exist
- **Cooperative design**: Deadlocks are less likely due to voluntary yielding
- **Best practice**: Design message flows to avoid circular waiting

## Usage Example

```c
// Initialize queues
ipc_queue_t task1_queue, task2_queue;
ipc_init(&task1_queue);
ipc_init(&task2_queue);

// Task 1: Send a message
ipc_message_t msg = {
    .type = IPC_MSG_PING,
    .sender = 1,
    .reply_token = 42
};
ipc_send(&task2_queue, &msg);

// Task 2: Receive and reply
ipc_message_t received;
ipc_recv(&task2_queue, &received);

ipc_message_t reply = {
    .type = IPC_MSG_PONG,
    .sender = 2,
    .reply_token = received.reply_token
};
ipc_send(&task1_queue, &reply);
```

## Design Decisions

### Why Ring Buffer?
- **Efficient**: O(1) enqueue/dequeue operations
- **Fixed memory**: Predictable memory usage
- **Cache-friendly**: Contiguous memory layout

### Why Cooperative Blocking?
- **Simple**: No need for complex synchronization primitives
- **Low overhead**: Yielding is cheap compared to context switches
- **Deterministic**: Easier to reason about task behavior

### Why Bounded Queues?
- **Memory safety**: Prevents unbounded memory growth
- **Backpressure**: Full queues naturally throttle senders
- **Predictable**: System behavior is bounded and analyzable

## Future Enhancements (Not Implemented)

- Memory protection and capability system
- Network IPC (currently local only)
- File system integration
- Priority-based message delivery
- Timeout mechanisms for blocking operations
- Non-blocking send/receive variants
