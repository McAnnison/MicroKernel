#include "kernel/task.h"

#include <stddef.h>

#include "kernel/panic.h"

#define MAX_TASKS 8
#define STACK_SIZE 4096

typedef enum {
    TASK_UNUSED = 0,
    TASK_RUNNABLE,
    TASK_FINISHED,
} task_state_t;

typedef struct {
    const char *name;
    task_entry_t entry;
    void *arg;
    uint32_t *sp;
    task_state_t state;
} task_t;

extern void ctx_switch(uint32_t **old_sp, uint32_t *new_sp);

static task_t g_tasks[MAX_TASKS];
static uint8_t g_stacks[MAX_TASKS][STACK_SIZE];

static int g_current = -1;
static uint32_t *g_scheduler_sp = NULL;

__attribute__((noreturn)) static void task_exit(void) {
    if (g_current >= 0 && g_current < MAX_TASKS) {
        g_tasks[g_current].state = TASK_FINISHED;
    }

    // Return control to the scheduler.
    task_yield();

    // Should never return here.
    for (;;) {
        __asm__ volatile("hlt");
    }
}

__attribute__((noreturn)) static void task_trampoline(void) {
    int idx = g_current;
    if (idx < 0 || idx >= MAX_TASKS) {
        panic("task_trampoline: invalid current task");
    }

    task_t *t = &g_tasks[idx];
    if (t->entry == NULL) {
        panic("task_trampoline: null entry");
    }

    t->entry(t->arg);
    task_exit();
}

void task_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        g_tasks[i].name = NULL;
        g_tasks[i].entry = NULL;
        g_tasks[i].arg = NULL;
        g_tasks[i].sp = NULL;
        g_tasks[i].state = TASK_UNUSED;
    }

    g_current = -1;
    g_scheduler_sp = NULL;
}

static int alloc_task_slot(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (g_tasks[i].state == TASK_UNUSED) {
            return i;
        }
    }
    return -1;
}

int task_create(const char *name, task_entry_t entry, void *arg) {
    int id = alloc_task_slot();
    if (id < 0) {
        return -1;
    }

    g_tasks[id].name = name;
    g_tasks[id].entry = entry;
    g_tasks[id].arg = arg;
    g_tasks[id].state = TASK_RUNNABLE;

    // Prepare initial stack so the first context switch "returns" into task_trampoline.
    uint32_t *stack_top = (uint32_t *)(g_stacks[id] + STACK_SIZE);

    // Align to 16 bytes for good measure.
    stack_top = (uint32_t *)((uintptr_t)stack_top & ~((uintptr_t)0xF));

    *(--stack_top) = (uint32_t)(uintptr_t)task_trampoline;
    g_tasks[id].sp = stack_top;

    return id;
}

void task_yield(void) {
    if (g_current < 0) {
        return;
    }

    ctx_switch(&g_tasks[g_current].sp, g_scheduler_sp);
}

static int pick_next_runnable(int start_after) {
    for (int off = 1; off <= MAX_TASKS; off++) {
        int idx = (start_after + off) % MAX_TASKS;
        if (g_tasks[idx].state == TASK_RUNNABLE) {
            return idx;
        }
    }
    return -1;
}

void scheduler_run(void) {
    int last = -1;

    for (;;) {
        int next = pick_next_runnable(last);
        if (next < 0) {
            break;
        }

        last = next;
        g_current = next;

        // Save scheduler SP and switch to task.
        ctx_switch(&g_scheduler_sp, g_tasks[next].sp);

        // When the task yields, we resume here.
        if (g_tasks[next].state == TASK_FINISHED) {
            g_tasks[next].state = TASK_UNUSED;
            g_tasks[next].sp = NULL;
            g_tasks[next].entry = NULL;
            g_tasks[next].arg = NULL;
            g_tasks[next].name = NULL;
        }
    }

    g_current = -1;
}

int task_get_current(void) {
    return g_current;
}

int task_restart(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) {
        return -1;
    }

    task_t *t = &g_tasks[task_id];
    
    // Can only restart if we have the original entry point
    if (t->entry == NULL) {
        return -1;
    }

    // Reset the task state
    t->state = TASK_RUNNABLE;

    // Prepare new stack (same as task_create)
    uint32_t *stack_top = (uint32_t *)(g_stacks[task_id] + STACK_SIZE);
    stack_top = (uint32_t *)((uintptr_t)stack_top & ~((uintptr_t)0xF));
    *(--stack_top) = (uint32_t)(uintptr_t)task_trampoline;
    t->sp = stack_top;

    return 0;
}
