#include "scheduler.h"
#include <stdio.h>
#include <stddef.h>

#define MAX_TASKS 8

static task_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_task = 0;
static bool scheduler_running = false;
static jmp_buf scheduler_context;
static bool yielding = false;

void scheduler_init(void) {
    task_count = 0;
    current_task = 0;
    scheduler_running = false;
    yielding = false;
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].func = NULL;
        tasks[i].active = false;
        tasks[i].name = NULL;
        tasks[i].has_context = false;
    }
}

void scheduler_add_task(task_func_t func, const char *name) {
    if (task_count >= MAX_TASKS) {
        printf("Error: Too many tasks (max %d)\n", MAX_TASKS);
        return;
    }
    
    tasks[task_count].func = func;
    tasks[task_count].active = true;
    tasks[task_count].name = name;
    tasks[task_count].has_context = false;
    task_count++;
}

void task_yield(void) {
    if (!scheduler_running || !yielding) {
        return;
    }
    
    int task_idx = current_task;
    
    // Save task context and jump back to scheduler
    if (setjmp(tasks[task_idx].context) == 0) {
        tasks[task_idx].has_context = true;
        yielding = false;
        longjmp(scheduler_context, 1);
    }
    // Resumed by scheduler - continue execution
    yielding = false;
}

void scheduler_run(void) {
    scheduler_running = true;
    
    printf("=== Starting scheduler with %d tasks ===\n", task_count);
    
    int iterations = 0;
    const int max_iterations = 1000;
    
    while (iterations < max_iterations) {
        // Check if any tasks are active
        bool any_active = false;
        for (int i = 0; i < task_count; i++) {
            if (tasks[i].active) {
                any_active = true;
                break;
            }
        }
        
        if (!any_active) {
            printf("=== All tasks completed ===\n");
            break;
        }
        
        // Find and run next active task
        int start = current_task;
        bool task_ran = false;
        
        do {
            if (tasks[current_task].active && tasks[current_task].func) {
                task_ran = true;
                yielding = true;
                
                // Save scheduler context before jumping to task
                if (setjmp(scheduler_context) == 0) {
                    // Jump to task
                    if (tasks[current_task].has_context) {
                        // Resume from previous yield
                        longjmp(tasks[current_task].context, 1);
                        // If we reach here, task completed after resuming
                        tasks[current_task].has_context = false;
                        tasks[current_task].active = false;
                    } else {
                        // Start task for first time
                        tasks[current_task].func();
                        // Task completed (returned normally without yielding)
                        tasks[current_task].active = false;
                    }
                } else {
                    // Returned from task_yield() - task yielded, keep it active
                    // has_context is already set by task_yield()
                }
                break;
            }
            current_task = (current_task + 1) % task_count;
        } while (current_task != start);
        
        if (!task_ran) {
            break;
        }
        
        // Move to next task
        current_task = (current_task + 1) % task_count;
        iterations++;
    }
    
    if (iterations >= max_iterations) {
        printf("Warning: Reached max iterations limit\n");
    }
    
    scheduler_running = false;
}
