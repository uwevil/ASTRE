#include "events.h"
#include "periodictasks.h"
#include "tasks.h"

// Main procedure of polling server

void *runDeferredServer (void *t){
  TaskInfo           s;
  EventInfo          e;
  TimeSpan           c = 0;
  TimeSpan           l = 0;
  Time               d = 0;
  int                i;

  s = periodicTaskTable[*(int *)t];

  // Schedule the initial replenishment event
  e.kind        = PRODUCE;
  e.name        = s.name;
  e.activation  = nextActivation (s.period);
  e.computation = s.computation;
  appendEvent (e);

  // Print the event pushed in the queue
  putHeader (s.name);
  putString ("@");
  putTime   (e.activation);
  putString (" ");
  putString (s.name);
  putString ("=+");
  putTime   (e.computation);
  newLine ();

  while (systemNotCompleted () && (0 <= lastEvent)) {
    e = getEvent (firstEvent);
    
    // Before handling next event check the server budget is not empty
    if (s.computation == 0) {

      // Look for the first PRODUCE event
      for (i=firstEvent; i <= lastEvent; i++){
	e = getEvent(i);
	if (e.kind == PRODUCE) {
	  if (systemCompletedAt (e.activation)) {
	    return NULL;
	  }

          // Remove PRODUCE event and then wait for its activation time.
		//  NYI("remove event and wait for its activation");
		//************************************************************
		removeEvent(i);
		delayUntil(e.activation);


          // Update the server budget and schedule the next PRODUCE
          // event. To do so compute the next activation time and the
          // computation time related to this replenishment event.
         // NYI("update server budget and schedule replenishment");
		//**********************************************************
		s.computation = e.computation;
		e.activation = nextActivation(s.period);
		appendEvent(e);


          // Print the arrival of this event
	  putHeader (s.name);
	  putString (s.name);
	  putString ("=+");
	  putTime   (e.computation);
	  newLine ();

          // Print the event pushed in the queue
	  putHeader (s.name);
	  putString ("@");
	  putTime   (e.activation);
	  putString (" ");
	  putString (s.name);
	  putString ("=+");
	  putTime   (e.computation);
	  newLine ();
      
	  break;
	}
      }
    }

    e = getEvent (firstEvent);
    if (systemCompletedAt (e.activation)){
      return NULL;
    }

    if (e.kind == PRODUCE) {
      // Remove Produce event and then wait for its activation time
     // NYI("remove event and wait for its activation time");
	//****************************************************************
	removeEvent(firstEvent);
	delayUntil(e.activation);



      // Update the server budget. And schedule the next replenishment.
     // NYI ("update server budget and schedule its replenishment");
	//*****************************************************************
	s.computation = e.computation;
	e.activation = nextActivation(s.period);
	appendEvent(e);

      // Print the event pushed in the queue
      putHeader (s.name);
      putString (s.name);
      putString ("=+");
      putTime   (e.computation);
      putString (" @");
      putTime   (e.activation);
      newLine ();
      
    } else {

      // Wait for event activation
      //NYI("wait for event activation");
	//******************************************************************
	delayUntil(e.activation);



      
      // Evaluate the computation time that can be possibly allocated to
      // handle this event, that is the computation time requested by
      // the event and the one available on the server. But the
      // computation time must not extend over the next replenishment in
      // order to take into account a replenishment that would happen
      // while handling an event. As a consequence, the final
      // computation time must not last after the next replenishment.
     // NYI("evaluate computation time that does not overlap with replishment");
	//********************************************************************
	



      // Update computation time needed to complete event in queue.
      // Remove the event once it is completed.
      // Do not update server budget yet.
      // We want to print the server status before and after this operation.
     // NYI("evaluate remaining computation time of current event");
     // NYI("remove event when completed");
     // NYI("update event in queue when not completed");
     c = e.computation;
     int ok = 0;
     
	 if (c > timeSpanBeforeTime(nextActivation(s.period))){
		c = timeSpanBeforeTime(nextActivation(s.period));
		removeEvent(firstEvent);
		e.computation = e.computation - c;
		e.activation = e.activation + c;
		appendEvent(e);
		ok = 1;
     }else {
		 if (e.computation <= s.computation){
		e.computation = 0;
		removeEvent(firstEvent);
		}else{
			ok = 2;
		removeEvent(firstEvent);
		e.computation = e.computation - s.computation;
		e.activation  = e.activation + 1;
		appendEvent (e);
		c = s.computation;
      }
     }
     
     
     

      // Print status of both server and event status
      putHeader    (s.name);
      putString    (s.name);
      putString    ("=");
      putTime      (s.computation);
      putString    ("-");
      putTime      (c);
      putString    (" ");
      putString    (e.name);
      putString    ("=");
      putTime      (e.computation);
      newLine ();

      // Update the server budget after this operation.

      // Simulate the execution of this event using
      // computeDuringTimeSpan. Provide the name of the event, its
      // worst case execution time, and the period of the server.
      // ATTENTION : the period parameter in computeDuringTimeSPan is
      // used to compute the execution priority. See tasks.h.
     // NYI("update server budget");
     // NYI("compute event");
     
     if (ok == 1){
		 s.computation = s.computation - c;
		 computeDuringTimeSpan(e.name, c, s.period);
	}else if (ok == 2){
		computeDuringTimeSpan(e.name, c, s.period);
		s.computation = 0;
	}else{
		computeDuringTimeSpan(e.name, c, s.period);
		s.computation -= c;
	}
     
     
     
     

      // When the computation overlaps with a replenishment, we limit
      // the computation time to let this event arises. But we need to
      // force the server to extract a PRODUCE event. In this specific
      // case, force the server budget to zero.
    // NYI("set server budget to zero to force the replenishment");
	s.computation = 0;



      // Print event completion if needed
     if (e.computation == 0) {
	putHeader    (s.name);
	putString    ("completed ");
	putString    (e.name);
	newLine ();
      }
    }
  }

  return NULL;
}
