#include "scenarii.h"
#include "tasks.h"
#include "backgroundserver.h"
#include "pollingserver.h"
#include "deferredserver.h"
#include "sporadicserver.h"
#include "periodictasks.h"

#define MAXPERIODICTASKS  32

TaskInfo periodicTaskTable[MAXPERIODICTASKS];
int lastPeriodicTask;

void *(*runFunctionTable[SPORADICSERVER+1])(void *);

pthread_t *runnableTaskTable;

void sortByPeriod ();

void *runPeriodicTask (void *t){
  TaskInfo           s;      // self reference
  Time               c = 0;  // completion time
  Time               d = 0;  // deadline

  s = periodicTaskTable[*(int *)t];

  while (systemNotCompleted()){

    // Compute the deadline or the next activation
    d=nextActivation(s.period);

    putHeader (s.name);
    putString ("activated");
    newLine();

    // Simulate the execution of this task using
    // computeDuringTimeSpan. Provide the name of the task, its worst
    // case execution time, and the period of the task. ATTENTION :
    // the period parameter in computeDuringTimeSPan is used to
    // compute the execution priority. It returns the completion time
    // that should be used to check whether the task missed its
    // deadline. Use variable c to save this value. See tasks.h.


    //    NYI("compute task for its computation time");
    //********************************************************
      c = computeDuringTimeSpan(s.name, s.computation, s.period);
    
    //    NYI("save the completion time in variable c for later use");
    //********************************************************
    // c = timeAfterTimeSpan(s.computation);
    
    putHeader (s.name);
    putString ("completed");
    newLine();

    // Check that the completion time did not occur after the deadline
    if (d < c) {
      putHeader (s.name);
      putString ("OVERRUN ");
      putTime   (d);
      putString (" < ");
      putTime   (c);
      newLine();
      break;
    };

    // Wait for the next activation. In other words, wait for the deadline.
    // NYI("wait for the next activation");
    //**********************************************************
    delayUntil(d);

    
  }

  return NULL;
}

void setScheduler(int s){
  TaskInfo t;
  int l, i;

  initTaskLists();

  switch(s){
  case RMSCHEDULER :
    /* sort by decreasing period */
    for(l=lastPeriodicTask; l>0; l--){
      for(i=0; i<l; i++){
	if (periodicTaskTable[i].period < periodicTaskTable[i+1].period){
	  t = periodicTaskTable[i];
	  periodicTaskTable[i] = periodicTaskTable[i+1];
	  periodicTaskTable[i+1] = t;
	}
      }
    }
    break;
  default :
    break;
  }

  printf("Compute priorities\n");
  printf("------------------\n");
  for(i=0; i<=lastPeriodicTask; i++){
    putTask(periodicTaskTable[i]);
  }
  printf("------------------\n");
}

void activatePeriodicTasks(){
  int i;

  runnableTaskTable = malloc((lastPeriodicTask+1)*sizeof(pthread_t));

  runFunctionTable[NOTASK]           = NULL;
  runFunctionTable[PERIODICTASK]     = runPeriodicTask;
  runFunctionTable[APERIODICTASK]    = NULL;
  runFunctionTable[BACKGROUNDSERVER] = runBackgroundServer;
  runFunctionTable[POLLINGSERVER]    = runPollingServer;
  runFunctionTable[DEFERREDSERVER]   = runDeferredServer;
  runFunctionTable[SPORADICSERVER]   = runSporadicServer;

  for(i=0; i<=lastPeriodicTask; i++){
    int * t = malloc (sizeof (int));
    *t = i;
    int k = periodicTaskTable[i].kind;
   if (pthread_create(&runnableTaskTable[i], NULL, runFunctionTable[k], t)) {
      printf("Failed to create thread %d\n", i);
      exit (1);
    }
  }
  for(i=0; i<=lastPeriodicTask; i++){
    pthread_join(runnableTaskTable[i], NULL);
  }
}



