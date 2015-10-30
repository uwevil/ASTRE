#ifndef PERIODIC_TASKS_H
#define PERIODIC_TASKS_H
#include "tasks.h"

#define RMSCHEDULER 1

extern TaskInfo periodicTaskTable[];
extern int lastPeriodicTask;

void setScheduler(int s);
void *runPeriodicTask(void *t);

void activatePeriodicTasks();

#endif
