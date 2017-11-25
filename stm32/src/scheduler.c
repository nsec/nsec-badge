// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)

#include <string.h>

#include "scheduler.h"

#define MAX_NBR_SCHEDULER_TASK 256

struct scheduler {
    int current_task;
    int last_task;
    struct task task_list[MAX_NBR_SCHEDULER_TASK];
};

static struct scheduler scheduler;

void scheduler_init(void) {
    scheduler.current_task = 0;
    scheduler.last_task = 0;
}

void scheduler_add_task(struct task *t) {
    memcpy(&(scheduler.task_list[scheduler.last_task]), t, sizeof(struct task));

    scheduler.last_task = (scheduler.last_task + 1) % MAX_NBR_SCHEDULER_TASK;
}

void scheduler_execute(void) {
    if (scheduler.current_task == scheduler.last_task) {
        // Nothing to run
        return ;
    }

    struct task *current = &scheduler.task_list[scheduler.current_task];

    current->callback(current->args);

    scheduler.current_task = (scheduler.current_task + 1) % MAX_NBR_SCHEDULER_TASK;
}
