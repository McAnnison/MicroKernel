#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <setjmp.h>

typedef void (*task_func_t)(void);

// Task control block
typedef struct {
    task_func_t func;
    bool active;
    const char *name;
    jmp_buf context;
    bool has_context;
} task_t;

// Scheduler API
void scheduler_init(void);
void scheduler_add_task(task_func_t func, const char *name);
void task_yield(void);
void scheduler_run(void);

#endif
