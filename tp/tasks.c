#include "tasks.h"
#include "aperiodictasks.h"
#include "periodictasks.h"

#define MAXTASKS 32
#define NONE -1

TimeSpan q = 10000; /* 10 milliseconds */

typedef struct {
  TaskName name;
  TimeSpan period;
  TimeSpan computation;
  
  int      next;
} TaskInfoNode;

pthread_mutex_t fakedMutex;
pthread_cond_t fakedCond;

pthread_mutex_t mcMutex;
pthread_cond_t  mcCondVar;
TaskInfoNode    taskList[MAXTASKS];
int             headReadyList = NONE;
int             headFreeList = 0;
int             nReadyList = 0;
int             debugLevel = 0;

void printReadyList(char *s){
  int i = headReadyList;
  
  if (debugLevel == 0) return;

  putHeader (s);
  putString ("<");
  while (i != NONE) {
    putString (" ");
    putInteger (i);
    putString (" ");
    putString (taskList[i].name);
    i = taskList[i].next;
  }
  putString (">");
  newLine();
  putHeader ("free list at");
  putInteger (headFreeList);
  newLine();
}

int activateTask (TaskName n, TimeSpan c, TimeSpan t) {
  int i;
  int previous = NONE;
  int current  = NONE;

  pthread_mutex_lock(&mcMutex);

  if (headReadyList == NONE) {
    i = headFreeList;
    headFreeList = taskList[headFreeList].next;
    taskList[i].name = n;
    taskList[i].period = t;
    taskList[i].computation = c;

    taskList[i].next = NONE;
    headReadyList = i;

  } else {
    i = headFreeList;
    headFreeList = taskList[headFreeList].next;
    taskList[i].name = n;
    taskList[i].period = t;
    taskList[i].computation = c;

    // Insert the task in the ready list
    previous = NONE;
    current = headReadyList;
    while (current != NONE && taskList[current].period < t) {
      previous = current;
      current  = taskList[current].next;
    }
    taskList[i].next = current;
    if (previous == NONE) {
      headReadyList = i;
    } else {
      taskList[previous].next = i;
    }
  }
  printReadyList("activateTask");

  pthread_mutex_unlock(&mcMutex);

  return i;
}

void completeTask (){
  int t;
  pthread_mutex_lock(&mcMutex);
  t = headReadyList;
  headReadyList = taskList[t].next;
  taskList[t].next = headFreeList;
  headFreeList = t;
  pthread_cond_broadcast(&mcCondVar);
  printReadyList("completeTask");
  pthread_mutex_unlock(&mcMutex);
}

int scheduleTask (int t){
  int preempted = 0;

  pthread_mutex_lock(&mcMutex);
  while (headReadyList != t) {
    preempted = 1;
    pthread_cond_wait (&mcCondVar, &mcMutex);
  }
    
  printReadyList("scheduleTask");
  pthread_mutex_unlock(&mcMutex);

  return preempted;
}

void putHeader (char *s) {
  Time t1, t0;

  pthread_mutex_lock (&fakedMutex);

  /*
  t0 = localClock() / 1000;
  t1 = t0;
  if (t1 < 0) t1 = 0;
  t1 = (t1 + 100) / 1000;
  */

  t0 = localClock();
  t1 = roundClock(t0);

  if (t1 < 0) t1 = 0;
  printf ("%s %02d ", s, t1 / 1000000);
  // printf ("(%05d) ", t0 / 1000);
  
}

void putInteger (int i) {
  printf ("%d", i);
}

void putString (char *s) {
  printf ("%s", s);
}

void putTime (int t) {
  putInteger (t/1000000);
}

void newLine () {
  printf ("\n");
  pthread_mutex_unlock (&fakedMutex);
}

void putTask (TaskInfo i) {
  putHeader (i.name);
  putString (" ");
  putInteger (i.kind);
  putString (" C=");
  putTime (i.computation);
  if (i.kind == APERIODICTASK){
    putString (" A=");
    putTime (i.activation);
  } else {
    putString (" P=");
    putTime (i.period);
  };
  newLine ();
}

Time computeDuringTimeSpan(TaskName n, TimeSpan c, TimeSpan t){
  Time     s, e;
  TimeSpan work = 0;
  int      task;
  int      preempted;

  task = activateTask(n, c, t);
  s = roundClock(localClock());
  e = s;
  
  /*
  putHeader (n);
  putString ("compute for ");
  putTime   (c);
  newLine();
  */

  while (work + (e-s) + q <= c) {
    preempted = scheduleTask(task);
    if (preempted) {
    
      /*
      putHeader (n);
      putString ("resumed after work ");
      putInteger (work);
      newLine();
      */

      work = work + roundClock (e-s);
      s = roundClock(localClock());
      e = s;
    }
    while (localClock () <= e + q) usleep (q/100);
    e = localClock();
  }
  completeTask();
  return e;
}

void initTaskLists(){
  int i;

  for(i = 0; i < MAXTASKS - 1; i++)
    taskList[i].next = i+1;
  taskList[MAXTASKS - 1].next = NONE;
}
 
struct timeval s = {0, 0};
Time           e = 60 * MEGA; /* 60 seconds */

void setActivationTime (){
  pthread_mutex_init (&fakedMutex, NULL);
  pthread_cond_init (&fakedCond, NULL);
  pthread_mutex_init(&mcMutex, NULL);
  pthread_cond_init(&mcCondVar, NULL);
  gettimeofday(&s, NULL);
}

int systemNotCompleted () {
  return ((localClock () + q) < e);
}

int systemCompletedAt (int t) {
  return (e < localClock () + t);
}

Time localClock(){
  struct timeval tv;
  int usec;

  if ((s.tv_sec == 0) && (s.tv_usec == 0)) return 0;

  gettimeofday(&tv, NULL);
  usec=(tv.tv_sec - s.tv_sec) * 1000000 + (tv.tv_usec - s.tv_usec);
  return (Time)usec;
}

Time roundClock (Time c) {
  return ((c+200000)/1000000)*1000000;
}

Time localClockRounded(){
  int usec;

  usec=localClock();
  usec=roundClock(usec);
  return (Time)usec;
}

void delayUntil(Time t){
  struct timespec ts;
  int usec;

  if (localClock () > t) return;
   
  usec = s.tv_usec + t;
  ts.tv_sec  = s.tv_sec + usec / 1000000;
  ts.tv_nsec = (usec % 1000000) * 1000;
  pthread_mutex_lock(&fakedMutex);
  pthread_cond_timedwait(&fakedCond, &fakedMutex, &ts);
  pthread_mutex_unlock(&fakedMutex);
};

Time timeAfterTimeSpan (TimeSpan t) {
  return (int) roundClock(t + localClock ());
}
  
TimeSpan timeSpanBeforeTime (Time t) {
  return (int) roundClock(t - localClock ());
}
  
Time nextActivation(TimeSpan period){
  Time c1, c2;

  c1 = localClock();
  // Add 10ms to avoid incorrect roundup
  c2 = (((c1+q) / period) + 1) * period;

  return c2;
}

void NYI(char *s){
  printf("NOT YET IMPLEMENTED : %s\n", s);
}
