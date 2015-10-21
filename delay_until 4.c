#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NB_TASKS 2

/*************** main ********************/
int main(int argc, char* argv[]){
  pthread_t Les_Threads[NB_TASKS];
  int delai[NB_TASKS];
  void Une_Tache (void *arg);

  if (argc != 3) {
    printf("Utilisation : %s delai_1 delai_2 !\n", argv[0]);
    return 1;
  }

  /* creation des threads */
  /* A FAIRE */
	delai[0] = atoi(argv[1]);
	delai[1] = atoi(argv[2]);
    pthread_create(&Les_Threads[0], 
	NULL, (void *(*)(void *))Une_Tache, &delai[0]);
    pthread_create(&Les_Threads[1], 
	NULL, (void *(*)(void *))Une_Tache, &delai[1]);
  
/* attendre la fin des threads precedemment crees */
	int i = 0;
  for (i = 0 ; i < NB_TASKS ; i++){
	pthread_join(Les_Threads[i], NULL);
    printf("main (tid %d) fin de tid %d\n", (int)pthread_self(), (int)Les_Threads[i]);
  }

  return 0;
}

/*************** Une_Tache ********************/
void Une_Tache (void *arg) {
  int *Ptr;
  int compteur, delai;
  pthread_cond_t var_cond;
  pthread_mutex_t verrou;
  struct timespec time;

  Ptr = (int *)arg;
  delai = *Ptr;


  /* initialiser le verrou et la var. cond. */
  /* A FAIRE */
	pthread_cond_init(&var_cond, NULL);
	pthread_mutex_init(&verrou, NULL);
  
  compteur = 0;
  while (compteur < 10){
	pthread_mutex_lock(&verrou);
    clock_gettime (CLOCK_REALTIME, &time);
    time.tv_sec += delai ;
    compteur = compteur + 1;
    printf("tid %d : date (s.ns) : %d.%d, compteur %d, delai %d\n", 
	   (int)pthread_self(), (int)time.tv_sec, (int)time.tv_nsec, compteur, delai);
    pthread_cond_timedwait(&var_cond, &verrou, &time);
	pthread_mutex_unlock(&verrou);
  }

}
