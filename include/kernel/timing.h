#pragma once

#include <stdint.h>

// i386 TSC timestamp (EDX:EAX) stored as two 32-bit halves.
typedef struct {
    uint32_t lo;
    uint32_t hi;
} tsc_t;

static inline tsc_t tsc_now(void) {
    tsc_t t;
    __asm__ volatile("rdtsc" : "=a"(t.lo), "=d"(t.hi));
    return t;
}

static inline tsc_t tsc_sub(tsc_t end, tsc_t start) {
    tsc_t d;
    d.lo = end.lo - start.lo;
    d.hi = end.hi - start.hi - (end.lo < start.lo ? 1u : 0u);
    return d;
}
