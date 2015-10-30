#include "scenarii.h"
#include "tasks.h"
#include "periodictasks.h"
#include "aperiodicservers.h"

int main(int argc, char *argv[]) {
  int   i;
  int   n = 0;

  if (argc <= 1) {
    printf("%s [-d=<n>] <scenario>\n", argv[0]);
    exit(1);
  }
  for (i=1; i < argc; i++) {
    if ((strcmp (argv[i], "-d") == 0) && (++i < argc))
      debugLevel = atoi (argv[i]);
    else if (n == 0)
      n = atoi(argv[i]);
    else {
      printf("%s [-d <n>] <scenario>\n", argv[0]);
      exit(1);
    }	
  }
  
  loadScenario (n);
  setScheduler(RMSCHEDULER);
  setActivationTime();
  activateAperiodicServer();
  activatePeriodicTasks();
  return 0;
}
