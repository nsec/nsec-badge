// Copyright (c) 2017
// Benjamin Vanheuverzwijn <bvanheu@gmail.com>
// Marc-Etienne M. Leveille <marc.etienne.ml@gmail.com>
//
// License: MIT (see LICENSE for details)

#ifndef SCHEDULER_H
#define SCHEDULER_H

struct task {
    void *(*callback)(void *args);
    void *args;
};

void scheduler_init(void);
void scheduler_add_task(struct task *t);
void scheduler_execute(void);

#endif
