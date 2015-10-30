#ifndef EVENTS_H
#define EVENTS_H

#include "tasks.h"

#define CONSUME 1 // consommer les jetons
#define PRODUCE 2 // distribuer les jetons

typedef char     EventKind;
typedef char *   EventName;

typedef struct {
  EventKind  kind;
  EventName  name;
  Time       activation;
  TimeSpan   computation;
} EventInfo;

// Append this event in the event table and keep it sorted
void appendEvent (EventInfo e);

// Modify the i-th event in the table (between firstEvent and lastEvent)
void setEvent (int i, EventInfo e);

// Remove the i-th event from table
void removeEvent (int i);

// Read the i-th event but do not remove it from table
EventInfo getEvent(int i);

// Produce the intial event table (from aperiodicTaskTable)
void produceEventTable();

extern int lastEvent;
extern int firstEvent;
extern EventInfo nullEvent;

#endif
