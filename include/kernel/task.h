#pragma once

#include <stdint.h>

typedef void (*task_entry_t)(void *arg);

void task_init(void);

// Creates a runnable task with its own stack.
// Returns task id on success, -1 on failure.
int task_create(const char *name, task_entry_t entry, void *arg);

// Cooperative yield: switches back to the scheduler.
void task_yield(void);

// Runs the cooperative scheduler until no runnable tasks remain.
void scheduler_run(void);

// Get current task ID (-1 if not in a task)
int task_get_current(void);

// Restart a crashed task with the same entry point
int task_restart(int task_id);
