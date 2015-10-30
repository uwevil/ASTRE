#ifndef SCENARIO_H
#define SCENARIO_H

#include "tasks.h"

extern TaskInfo ScenarioTable[];

extern TaskInfo periodicTaskTable[];
extern int lastPeriodicTask;

extern TaskInfo aperiodicTaskTable[];
extern int lastAperiodicTask;

void loadScenario(int s);

#endif
