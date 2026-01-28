# Performance Comparison: Microkernel IPC vs Monolithic Direct Calls

This document presents benchmark results comparing microkernel-style inter-process communication (IPC) message passing against monolithic-style direct function calls.

## Methodology

### What Was Measured

| Benchmark | Description |
|-----------|-------------|
| **Direct Call Baseline** | Tight loop of direct function calls (`ping()` → `pong()`), simulating how a monolithic kernel invokes internal services |
| **IPC Message Passing** | Uses ring buffer queue with `ipc_send()` / `ipc_recv()` operations, simulating microkernel service communication |

### Test Configuration

- **Iterations**: 10,000 ping/pong cycles per benchmark
- **Timing Method**: `rdtsc` CPU cycle counter (x86)
- **Optimization**: Compiled with `-O2` to simulate production performance
- **Warm-up**: 1,000 iterations before measurement to stabilize CPU caches
- **Platform**: Windows x86-64, GCC

### Test Procedure

```bash
# Build benchmark
mkdir -p build
gcc -Wall -Wextra -std=c11 -O2 -I. -c ipc/ipc.c -o build/ipc_opt.o
gcc -Wall -Wextra -std=c11 -O2 -I. -c benchmark/perf_benchmark.c -o build/perf_benchmark.o
gcc -Wall -Wextra -std=c11 -O2 build/ipc_opt.o build/perf_benchmark.o -o build/perf_benchmark

# Run benchmark
./build/perf_benchmark
```

## Results

| Metric | Direct Call | IPC Message Passing |
|--------|-------------|---------------------|
| **Total cycles** | ~20,000 | ~440,000 |
| **Avg cycles/operation** | ~2-4 | ~40-44 |
| **Overhead factor** | 1.0x (baseline) | **10-20x slower** |

### Key Observations

1. **IPC adds ~40 cycles per operation** compared to direct calls
2. **Overhead factor varies**: 10-20x depending on CPU cache state and scheduler
3. **Ring buffer operations dominate**: Memory copies and pointer arithmetic add latency

## Why This Matters

### Microkernel Trade-offs

| Aspect | Microkernel (IPC) | Monolithic (Direct) |
|--------|-------------------|---------------------|
| **Performance** | Slower due to message copying | Fastest possible path |
| **Isolation** | Services run in separate address spaces | All code shares kernel memory |
| **Reliability** | Faulty service can be restarted | Faulty driver can crash entire kernel |
| **Security** | Reduced attack surface per component | Single large attack surface |

### What the Benchmark Shows

This benchmark measures the **pure IPC overhead** without:
- Context switches (same thread/process)
- Memory protection (no MMU involvement)
- System call transitions

In a real microkernel, the overhead would be **significantly higher** due to:
1. **Context switches** between processes (1000+ cycles)
2. **TLB flushes** when switching address spaces
3. **System call entry/exit** (hundreds of cycles)

### Relevance to MicroKernel Project

Our implementation uses a **cooperative scheduler** with `setjmp`/`longjmp` for context saving, which is much faster than true preemptive multitasking. The measured 10-20x overhead represents the **minimum cost** of the message-passing abstraction.

## Conclusion

The benchmark confirms that **IPC message passing is inherently slower than direct function calls**, but provides better isolation and modularity. For this educational microkernel project, the overhead is acceptable and demonstrates the fundamental trade-off between performance and design flexibility.

---

*Benchmark code: [`benchmark/perf_benchmark.c`](../benchmark/perf_benchmark.c)*  
*Member 7 deliverable - Performance Comparison*
