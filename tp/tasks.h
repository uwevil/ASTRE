#ifndef TASKS_H
#define TASKS_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define NOTASK 0
#define PERIODICTASK 1
#define APERIODICTASK 2
#define BACKGROUNDSERVER 3
#define POLLINGSERVER 4
#define DEFERREDSERVER 5
#define SPORADICSERVER 6

#define MEGA 1000000

extern int debugLevel;

typedef char     TaskKind;
typedef char *   TaskName;
typedef int      TimeSpan; /* microseconds */
typedef int      Time;     /* microseconds */
typedef int      seconds;

typedef struct {
  TaskName name;
  TaskKind kind;        // task kind (periodic, background server, ...)
                        // "pX" : periodic task
                        // "aX" : aperiodic task
                        // "bs" : background server
                        // "ps" : polling server
                        // "ds" : deferred server
                        // "ss" : sporadic server
  TimeSpan computation; // worst case execution time
  TimeSpan activation;  // activation time
  TimeSpan period;      // task period
} TaskInfo;

/* Functions to use in this lab */

// Return the next activation time for a task of a given period t
Time nextActivation (TimeSpan t);

// Return the current clock
Time localClock (); 

// Wait until time t
void delayUntil (Time t);

// Task n with period t consumes for c time units. Period t is used to
// determine the priority at which task n is executed. Return the time
// at which the function has completed. This returned value is useful
// to check whether the function completed before deadline
Time computeDuringTimeSpan (TaskName n, TimeSpan c, TimeSpan t);

// Return the time interval between current clock and t
TimeSpan timeSpanBeforeTime (Time t);

// Return
Time timeAfterTimeSpan (TimeSpan t);

int  systemNotCompleted ();
int  systemCompletedAt (int t);
void initiateSystem ();
void initTaskLists();
void setActivationTime ();
Time roundClock (Time c);

// Fonctions to sequentially print messages
void putTask (TaskInfo i);
void putDebug (char *s, int d);

// To safely print a message in a concurrent environment :
// Start with putHeader to get a lock.
// Then use the other putX functions.
// At last end your session with newLine to release this lock.
void putHeader (char *s);
void putInteger (int i);
void putTime (int t);
void putString (char *s);
void newLine ();

void NYI(char *s);

#endif
