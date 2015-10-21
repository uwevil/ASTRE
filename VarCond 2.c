/*   
 *   ATTENTION : edition de liens avec -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
  pthread_cond_t   Var_Cond;        /* signalee quand un thread se termine */
  pthread_mutex_t  Verrou;          /* Verrou en ecriture pour les champs suivants */
  pthread_t        Thread_Id;       /* Identite du thread qui s est termine */
  int              Threads_Finis;   /* Nbre de threads qui se sont termines */
} Zone_Part_t;

/* Donnees partagees  */
Zone_Part_t Zone_Part;

sem_t sem;

#define MAX_THREADS 1000

/*------------------------------------------------------- 
  main
  ------------------------------------------------------- */
int main(int argc, char *argv[]){

  pthread_t Tid_main, Threads[MAX_THREADS];
  int  i, Acquittes, Nbre_Threads;
  void Un_Thread (void);
 
  if (argc != 2){
    printf("Utilisation : %s n (nbre de threads)\n", argv[0]);
    exit(1);
  }

  Nbre_Threads = atoi(argv[1]);
  if (Nbre_Threads > MAX_THREADS){
    printf(" Pas plus de  %d threads !\n", Nbre_Threads);
    exit(2);
  }
  srandom(time(NULL));    
  Tid_main=pthread_self();
    
  /* Nombre de threads acquittes */
  Acquittes = 0;

  /* initialisation des donnees partagees */
  Zone_Part.Threads_Finis = 0;
  
  /* initialisation des outils de synchronisation */
  /* initialisation des outils de synchronisation */
  pthread_cond_init(&Zone_Part.Var_Cond, NULL);
  pthread_mutex_init(&Zone_Part.Verrou, NULL);
    
    sem_init(&sem, 0, 0);

  /* creation des threads */
  for (i=0; i < Nbre_Threads ; i++){ 
    pthread_create(&Threads[i], NULL, (void *(*)(void *))Un_Thread, NULL);
    printf("main (Tid (0x)%x vient de creer : (0x)%x\n", (int)Tid_main, (int)Threads[i]);
  }
    
    sem_post(&sem);
  
  /* attendre la fin de TOUS ces threads 
   * on n utilise pas join pour ne pas imposer d ordre.
   * L attente se fait donc en utilisant une variable conditionnelle.
   * On recuperer l identite du thread qui s est termine
   * dans la variable Zone_Part.Thread_Id.
   */
  
  pthread_mutex_lock(&Zone_Part.Verrou);

  /**************** 111 ****************/
  while (Zone_Part.Threads_Finis < Nbre_Threads){
    pthread_cond_wait(&Zone_Part.Var_Cond, &Zone_Part.Verrou);
    Acquittes++;
      printf("\n\n----------------%d----------------\n\n", Acquittes);
    printf("main (Tid (0x)%x) : fin du thread (0x)%x\n", 
	   (int)Tid_main, (int)Zone_Part.Thread_Id);
  
      sem_post(&sem);
  }   
  pthread_mutex_unlock(&Zone_Part.Verrou);
  printf("main (Tid (0x)%x) : FIN, nbre de threads acquittes : %d, nbre de threads termines : %d\n",
         (int)Tid_main, Acquittes,Zone_Part.Threads_Finis);
  return 0;
}

/*-------------------------------------------------------------------
  Un_Thread
  Fonction executee par les threads.
  -------------------------------------------------------------------*/
void Un_Thread(void){
  pthread_t Mon_Tid;
  int i, Nbre_Iter;
  
  Mon_Tid = pthread_self();
  Nbre_Iter = random()/10000;

  printf("Thread (0x)%x : DEBUT, %d iterations\n", (int)Mon_Tid, Nbre_Iter);
  
    sem_wait(&sem);
    
  i=0;
  while(i < Nbre_Iter) i++;
   
  /* Signaler la fin au thread main qui s est bloque
   * sur une variable conditionnelle.
   * Ecrire le tid du thread dans la variable partagee Zone_Part.Thread_Id
   * Incrementer Zone_Part.Threads_Finis.
   */
  pthread_mutex_lock(&Zone_Part.Verrou);
  Zone_Part.Thread_Id = Mon_Tid ;
  Zone_Part.Threads_Finis++ ;
  pthread_cond_signal(&Zone_Part.Var_Cond);

  printf("Thread (0x)%x : FIN \n", (int)Mon_Tid);
  pthread_mutex_unlock(&Zone_Part.Verrou);

}
