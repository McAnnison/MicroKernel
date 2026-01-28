/**
 * timing.h - Portable high-resolution timing utilities
 * 
 * Uses rdtsc for cycle-accurate timing on x86, falls back to
 * clock-based timing on other platforms.
 */
#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <time.h>
#endif

/**
 * Read CPU timestamp counter (x86 only)
 * Returns cycle count for high-precision timing
 */
#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || defined(_M_X64)
static inline uint64_t rdtsc(void) {
#if defined(_MSC_VER)
    return __rdtsc();
#else
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
#endif
}
#define HAS_RDTSC 1
#else
#define HAS_RDTSC 0
#endif

/**
 * Portable timing structure
 */
typedef struct {
    uint64_t start;
    uint64_t end;
} timing_t;

/**
 * Get current time value (cycles or ticks)
 */
static inline uint64_t timing_now(void) {
#if HAS_RDTSC
    return rdtsc();
#elif defined(_WIN32)
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (uint64_t)counter.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#endif
}

/**
 * Get timing unit name for display
 */
static inline const char* timing_unit(void) {
#if HAS_RDTSC
    return "cycles";
#elif defined(_WIN32)
    return "ticks";
#else
    return "ns";
#endif
}

/**
 * Start timing
 */
static inline void timing_start(timing_t *t) {
    t->start = timing_now();
}

/**
 * Stop timing
 */
static inline void timing_stop(timing_t *t) {
    t->end = timing_now();
}

/**
 * Get elapsed time
 */
static inline uint64_t timing_elapsed(timing_t *t) {
    return t->end - t->start;
}

#endif /* TIMING_H */
