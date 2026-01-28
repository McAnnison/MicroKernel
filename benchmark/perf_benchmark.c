/**
 * perf_benchmark.c - Performance comparison benchmark
 * 
 * Compares microkernel-style IPC message passing vs
 * monolithic-style direct function calls.
 * 
 * Member 7 - Performance comparison deliverable
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "timing.h"
#include "ipc/ipc.h"

/* Number of ping/pong iterations for benchmark */
#define BENCHMARK_ITERATIONS 10000

/* Shared state for direct call benchmark */
static volatile uint32_t direct_ping_value;
static volatile uint32_t direct_pong_value;

/**
 * Simple "ping" function for direct call baseline
 * Simulates the work a service would do in a monolithic kernel
 */
static void direct_ping(uint32_t token) {
    direct_ping_value = token;
}

/**
 * Simple "pong" function for direct call baseline
 * Returns token to simulate response
 */
static uint32_t direct_pong(void) {
    direct_pong_value = direct_ping_value;
    return direct_pong_value;
}

/**
 * Run direct function call benchmark (monolithic-style)
 * This represents the fastest possible path - no IPC overhead
 */
static uint64_t run_direct_call_benchmark(int iterations) {
    timing_t t;
    uint32_t result = 0;
    
    timing_start(&t);
    
    for (int i = 0; i < iterations; i++) {
        /* Ping: caller invokes service directly */
        direct_ping(i);
        /* Pong: service returns result directly */
        result = direct_pong();
    }
    
    timing_stop(&t);
    
    /* Prevent compiler from optimizing away the loop */
    (void)result;
    
    return timing_elapsed(&t);
}

/**
 * Stub for task_yield - not used in non-blocking benchmark
 * The IPC code expects this to be available
 */
void task_yield(void) {
    /* No-op: benchmark runs single-threaded without blocking */
}

/**
 * Run IPC message passing benchmark (microkernel-style)
 * Uses the actual IPC ring buffer send/recv implementation
 */
static uint64_t run_ipc_benchmark(int iterations) {
    timing_t t;
    ipc_queue_t queue;
    ipc_message_t msg;
    ipc_message_t reply;
    
    /* Initialize IPC queue */
    ipc_init(&queue);
    
    timing_start(&t);
    
    for (int i = 0; i < iterations; i++) {
        /* Ping: send message */
        msg.type = IPC_MSG_PING;
        msg.sender = 1;
        msg.reply_token = i;
        ipc_send(&queue, &msg);
        
        /* Pong: receive message and send reply */
        ipc_recv(&queue, &reply);
        
        /* Note: In real microkernel, this would involve
         * context switch to another process */
    }
    
    timing_stop(&t);
    
    return timing_elapsed(&t);
}

/**
 * Print benchmark results
 */
static void print_results(uint64_t direct_time, uint64_t ipc_time, int iterations) {
    const char *unit = timing_unit();
    double direct_avg = (double)direct_time / iterations;
    double ipc_avg = (double)ipc_time / iterations;
    double overhead = (ipc_time > 0 && direct_time > 0) 
                     ? (double)ipc_time / direct_time 
                     : 0.0;
    
    printf("\n=== Performance Benchmark Results ===\n");
    printf("Iterations: %d\n\n", iterations);
    
    printf("[Direct Call Baseline] (monolithic-style)\n");
    printf("  Total: %llu %s\n", (unsigned long long)direct_time, unit);
    printf("  Avg per call: %.2f %s\n\n", direct_avg, unit);
    
    printf("[IPC Message Passing] (microkernel-style)\n");
    printf("  Total: %llu %s\n", (unsigned long long)ipc_time, unit);
    printf("  Avg per call: %.2f %s\n\n", ipc_avg, unit);
    
    printf("[Comparison]\n");
    printf("  IPC overhead: %.2fx slower than direct calls\n", overhead);
    printf("  Additional cost per operation: %.2f %s\n\n", ipc_avg - direct_avg, unit);
}

int main(void) {
    printf("=== Microkernel IPC vs Monolithic Direct Calls ===\n");
    printf("Performance Comparison Benchmark\n");
    printf("Timing method: %s\n", timing_unit());
    
    /* Warm-up run to stabilize caches */
    printf("\nWarm-up run...\n");
    run_direct_call_benchmark(1000);
    run_ipc_benchmark(1000);
    
    /* Actual benchmark */
    printf("Running benchmark with %d iterations...\n", BENCHMARK_ITERATIONS);
    
    uint64_t direct_time = run_direct_call_benchmark(BENCHMARK_ITERATIONS);
    uint64_t ipc_time = run_ipc_benchmark(BENCHMARK_ITERATIONS);
    
    print_results(direct_time, ipc_time, BENCHMARK_ITERATIONS);
    
    printf("Benchmark complete.\n");
    return 0;
}
