#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tasks.h"
#include "periodictasks.h"
#include "aperiodictasks.h"

void addAperiodicTask (TaskName n, seconds c, seconds a, seconds p);
void addPeriodicTask (TaskName n, seconds c, seconds a, seconds p);

void loadScenario (int s) {
  lastPeriodicTask   = -1;
  lastAperiodicTask  = -1;

  printf("load scenario %d\n", s);

  switch (s) {
  case 1 :
    addPeriodicTask("p1",  4,  0, 10);
    addPeriodicTask("p2",  4,  0, 15);
    break; // OK

  case 2 :
    addPeriodicTask("p1",  4,  0, 10);
    addPeriodicTask("p2",  4,  0, 15);
    addPeriodicTask("p3",  6,  0, 18);
    break; // KO

  case 3:
    addPeriodicTask("p1",  2,  0,  5);
    addPeriodicTask("p2",  4,  0, 15);
    addPeriodicTask("p3",  1,  0,  3);
    break;
  
  case 4:
    addPeriodicTask("p1",  6,  0, 20);
    addPeriodicTask("p2",  4,  0, 10);
    addPeriodicTask("bs",  0,  0, 61);
    addAperiodicTask("a2", 4, 11,  0);
    addAperiodicTask("a1", 3,  7,  0);
    break;
    
  case 5:
    addPeriodicTask("p1",  6,  0, 20);
    addPeriodicTask("p2",  4,  0, 10);
    addPeriodicTask("ps",  2,  0,  8);
    addAperiodicTask("a1", 3,  7,  0);
    addAperiodicTask("a2", 4, 11,  0);
    break;
    
  case 6:
    addPeriodicTask("p1",  6,  0, 20);
    addPeriodicTask("p2",  4,  0, 10);
    addPeriodicTask("ds",  2,  0,  8);
    addAperiodicTask("a1", 3,  7,  0);
    addAperiodicTask("a2", 4, 11,  0);
    break;

  case 7:
    addPeriodicTask("p1",  6,  0, 20);
    addPeriodicTask("p2",  4,  0, 10);
    addPeriodicTask("ss",  2,  0,  8);
    addAperiodicTask("a1", 3,  7,  0);
    addAperiodicTask("a2", 4, 11,  0);
    break;
    
  default:
    printf("no such scenario %d\n", s);
    exit(1);
  };

  printf("Periodic Tasks\n");
  printf("--------------\n");

  int i;
  for(i=0; i<=lastPeriodicTask; i++){
    putTask(periodicTaskTable[i]);
  }
  printf("--------------\n");

  printf("Aperiodic Tasks\n");
  printf("---------------\n");

  for(i=0; i<=lastAperiodicTask; i++){
    putTask(aperiodicTaskTable[i]);
  }
  printf("---------------\n");
};

void addPeriodicTask (TaskName n, seconds c, seconds a, seconds p){
  int k;

  lastPeriodicTask++;
  periodicTaskTable[lastPeriodicTask].name = malloc(strlen(n));
  strcpy(periodicTaskTable[lastPeriodicTask].name, n);
  if (strcmp(n,"bs") == 0) 
    k = BACKGROUNDSERVER;
  else if (strcmp(n,"ps") == 0)
    k = POLLINGSERVER;
  else if (strcmp(n,"ds") == 0)
    k = DEFERREDSERVER;
  else if (strcmp(n,"ss") == 0)
    k = SPORADICSERVER;
  else
    k = PERIODICTASK;

  periodicTaskTable[lastPeriodicTask].kind        = k;
  periodicTaskTable[lastPeriodicTask].computation = c * MEGA;
  periodicTaskTable[lastPeriodicTask].activation  = a * MEGA;
  periodicTaskTable[lastPeriodicTask].period      = p * MEGA;
}
  
void addAperiodicTask (TaskName n, seconds c, seconds a, seconds p){
  int i;
  TaskInfo s;

  lastAperiodicTask++;
  aperiodicTaskTable[lastAperiodicTask].name = malloc(strlen(n));
  strcpy (aperiodicTaskTable[lastAperiodicTask].name, n);
  aperiodicTaskTable[lastAperiodicTask].kind        = APERIODICTASK;
  aperiodicTaskTable[lastAperiodicTask].computation = c * MEGA;
  aperiodicTaskTable[lastAperiodicTask].activation  = a * MEGA;
  aperiodicTaskTable[lastAperiodicTask].period      = p * MEGA;
  for(i=lastAperiodicTask; i>0; i--){
    if (aperiodicTaskTable[i].activation < aperiodicTaskTable[i-1].activation){
      s = aperiodicTaskTable[i];
      aperiodicTaskTable[i] = aperiodicTaskTable[i-1];
      aperiodicTaskTable[i-1] = s;
    }
  }
}
  
