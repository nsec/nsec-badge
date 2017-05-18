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
