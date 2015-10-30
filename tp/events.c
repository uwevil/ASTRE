#include "aperiodictasks.h"
#include "events.h"
#include "tasks.h"

EventInfo eventTable[32];

int lastEvent = -1;
int firstEvent = 0;
EventInfo nullEvent;

void appendEvent(EventInfo e){
  EventInfo swap;
  int       i;

  lastEvent++;
  eventTable[lastEvent] = e;

  // Sort the table. Assume the table was already sorted
  for (i=lastEvent; i>firstEvent; i--){
    if (eventTable[i-1].activation <= eventTable[i].activation) return;
    swap=eventTable[i];
    eventTable[i]=eventTable[i-1];
    eventTable[i-1]=swap;
  }
};

void setEvent(int i, EventInfo e){
  eventTable[i] = e;
}

void removeEvent(int n){
  int i;

  for (i=n; i < lastEvent; i++)
    eventTable[i]=eventTable[i+1];
  lastEvent--;
};

EventInfo getEvent(int i){
  if ((firstEvent <= i) && (i <= lastEvent))
    return eventTable[i];
  return nullEvent;
};

void produceEventTable(){
  TaskInfo  t;
  EventInfo e;
  int       i;

  nullEvent.kind        = ' ';
  nullEvent.computation = 0;
  nullEvent.activation  = 0;

  for (i=0; i <= lastAperiodicTask; i++){
    t = aperiodicTaskTable[i];
    e.name = t.name;
    e.kind = CONSUME;
    e.activation  = t.activation;
    e.computation = t.computation;
    appendEvent(e);
  }
}
  
