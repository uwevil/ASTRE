/* 
 * 
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sched.h>

/********   pour regler le probleme des includes
 *          des differtentes familles LINUX ....  ***/

/*
#ifndef __USE_UNIX98
enum
{
 PTHREAD_PRIO_NONE,
 PTHREAD_PRIO_INHERIT,
 PTHREAD_PRIO_PROTECT
};
#endif
*/

/****************  ATTENTION       **********************
    les threads   qui utilisent des  mutexes   initialises avec :
            PTHREAD_PRIO_INHERIT ou PTHREAD_PRIO_PROTECT
     doivent avoir l'attribut de "contentionscope" positionne a:
             PTHREAD_SCOPE_SYSTEM 
     et la "scheduling policy " a :
             SCHED_FIFO  ou  SCHED_RR
     Voir :  pthread_attr_getsched
             pthread_attr_getscope
             pthread_getschedparam

**********************************************************/

/* variables globales */
pthread_mutex_t       Mutex1_id;
int                   min_priority, max_priority;
int  ceiling;
/*********************************************
      main
 *********************************************/
int main (void)  {
  void Task_0(void);
  
  pthread_t             main_id , Task0_id;
  pthread_mutexattr_t   m_attr;
  pthread_attr_t        th_attr;
  struct sched_param    param;
  
  int                  status, policy;

  /* Tid du thread main  */
  main_id = pthread_self();
  printf( "tid de main : 0x%08x\n", (int)main_id );
    
  max_priority = sched_get_priority_max(SCHED_FIFO);
  min_priority = sched_get_priority_min(SCHED_FIFO);
  
  printf ("Prio. min et max en FIFO : %d %d\n", min_priority , max_priority); 
  
  status = pthread_mutexattr_init(&m_attr );
  /* initialisation des attributs pour un mutex  a heritage de priorite */

  status = pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED );
  printf( "main-setpshared-status = %d\n", status );

   /* positionner la priorite du mutex  */
  /* ------------------ fait-------------------------------------- */
	pthread_mutexattr_setprioceiling(&m_attr, max_priority - 5);
 
  /* mettre le protocole du mutex a : PTHREAD_PRIO_PROTECT
    -> le mutex passe sa priorite au thread qui le prend.
   */
  /* ------------------ fait-------------------------------------- */
	pthread_mutexattr_setprotocol(&m_attr, PTHREAD_PRIO_PROTECT);   


  /* initialiser le  mutex */
  status = pthread_mutex_init( &Mutex1_id, &m_attr );
  /* verifier la priorite associee au mutex */
  /* ------------------ fait-------------------------------------- */
	pthread_mutexattr_getprioceiling(&m_attr, &ceiling);	



  printf( "main :  ceiling de M1 = %d\n", ceiling );

  pthread_attr_init(&th_attr);
  /* L option suivante permet aux threads l heritgae de priorite  */
   pthread_attr_setscope(&th_attr, PTHREAD_SCOPE_SYSTEM);
   policy=SCHED_FIFO; 
   pthread_attr_setschedpolicy(&th_attr, policy);
 
  /* creation du premier thread */
  /* ------------------ fait-------------------------------------- */
  	pthread_create(&Task0_id, NULL, (void *(*)(void *))Task_0, NULL);

printf( "main : tid de Task_0 : 0x%08x\n", (int)Task0_id );
  status = pthread_setschedparam( Task0_id, SCHED_FIFO, &param ); 

  /* attente de la fin du thread */
  pthread_join(Task0_id, NULL);
  
}
/*************************************************
      Fonction executee par le thread  T0
 *************************************************/
void Task_0 (void) {
  pthread_t             Task0_id, Task1_id;
  struct sched_param    param;
  int                 status, i, policy;

  void Task_1(void);

  Task0_id = pthread_self();
  printf("tid de Task 0  : 0x%08x\n", (int)Task0_id );
  status = pthread_getschedparam( Task0_id , &policy, &param );
  printf("Task 0 : pthread_getschedparam - priorite  = %d\n", param.sched_priority);
  
  /* prendre le mutex  M1 */
  /* ---------------- fait ---------*/
	status = pthread_mutex_lock(&Mutex1_id);


  printf( "Task 0 -lock-status = %d\n", status );

  /* creation du thread T1 */
 /* ---------------- fait ---------*/
	pthread_create(&Task1_id, NULL, (void *(*)(void *))Task_1, NULL);


  printf( "Task 0  : tid de Task_1 : 0x%08x\n", (int)Task1_id );

 /* mettre T1 en priorite superieure a celle de T0,
    mais inferieure a celle de M1 */
  param.sched_priority = max_priority - 10 ; 
  status = pthread_setschedparam( Task1_id, SCHED_FIFO, &param );
  status = pthread_getschedparam( Task1_id, &policy, &param );
  printf( "Task 0 : getschedparam pour T1 - priorite = %d\n", param.sched_priority);

 printf("Task 0 : debut de calcul\n");
  for (i=0; i< 0X7FFFFF; i++) {
    if (i%1000000 == 0) printf("Task 0 :  i = %d\n", i);
  };
  printf("Task 0 : fin de calcul\n");
 
  /* deverrouiller M1  */
  status = pthread_mutex_unlock( &Mutex1_id );
  printf( "Task 0 -unlock M1 -status = %d\n", status );

  status = pthread_getschedparam( Task0_id, &policy, &param );
  printf( "Task 0 : - priorite  = %d\n", param.sched_priority);
  
  status = pthread_join (Task1_id, 0);
  printf( "Task 0  : join , status = %d\n", status );
   
}
/*********************************************
      Fonction executee par le thread T1
 *********************************************/
void Task_1 (void) {
  int i, status;
  pthread_t   Task_id;

  /* si T0 a vu monter sa priorite en faisant
     lock(M1), alors cette fonction ne doit pas s'executer 
     au moment ou  la priorite de T1 est augmentee par T0 */
      
 Task_id = pthread_self();
  printf("Task 1 (%x) : debut de calcul\n",  Task_id );

  for (i=0; i< 0X7FFFFF; i++) {
    if (i%1000000 == 0) printf("Task 1 :  i = %d\n", i);
  };
  printf("Task 1 (%x): fin de calcul\n",  Task_id);
  
  printf( "Task 1 essaie de prendre M1 \n" );
  /* status = pthread_mutex_trylock( &Mutex1_id );  */
  status = pthread_mutex_trylock( &Mutex1_id );
  if ( status != EBUSY )
    printf( "Task 1 : trylock, status = %d\n", status );

  printf( "Task 1 : mutex M1 pris, va calculer\n" );
  
  for (i=0; i< 0XFFFFFF; i++) {};

  status = pthread_mutex_unlock( &Mutex1_id );

  printf("Task 1 (%x): exit\n",  Task_id);
  pthread_exit( 0 );
}

