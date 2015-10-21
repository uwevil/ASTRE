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
pthread_mutex_t M1;

pthread_mutexattr_t mutexattr;

pthread_attr_t attr;

struct sched_param param;

int  max_priority;

/*********************************************
 main
 *********************************************/
int main (void)  {
    pthread_t T0, T1, T2;
    void task0Func(void);
    void task1Func(void);
    void task2Func(void);
    
 //   pthread_mutexattr_setprotocol(&mutexattr, PTHREAD_PRIO_INHERIT);
 //   pthread_mutexattr_setprioceiling(&mutexattr, max_priority - 5);
    
    pthread_mutex_init(&M1, &mutexattr);
    
    max_priority = sched_get_priority_max(SCHED_FIFO);
    
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    
    param.sched_priority = max_priority - 15;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_create(&T0, &attr, (void *(*)(void *))task0Func, NULL);
    
    param.sched_priority = max_priority - 10;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_create(&T1, &attr, (void *(*)(void *))task1Func, NULL);
    
    param.sched_priority = max_priority - 5;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_create(&T2, &attr, (void *(*)(void *))task2Func, NULL);
    
    pthread_getschedparam( T0 , NULL, &param );
    printf("Task 0 : pthread_getschedparam - priorite  = %d\n", param.sched_priority);
    pthread_getschedparam( T1 , NULL, &param );
    printf("Task 1 : pthread_getschedparam - priorite  = %d\n", param.sched_priority);
    pthread_getschedparam( T2 , NULL, &param );
    printf("Task 2 : pthread_getschedparam - priorite  = %d\n", param.sched_priority);
    
    pthread_join(T0, NULL);
    pthread_join(T1, NULL);
    pthread_join(T2, NULL);

}

void task0Func(void){
    sleep(1);
    printf("T0 veut M1\n");
    pthread_mutex_lock(&M1);
    int i = 0;
    while (i < 10000) {
        i++;
    }
    pthread_mutex_unlock(&M1);
    printf("T0 lâche M1\n");
}


void task1Func(void){
    printf("T1 commence sleep\n");
    sleep(2);
    printf("T1 veut M1\n");
    
    pthread_getschedparam( pthread_self() , NULL, &param );
    printf("Task 1 : priorite avant  = %d\n", param.sched_priority);
    
    pthread_mutex_lock(&M1);
    int i = 0;
    
    while (i < 10) {
        pthread_getschedparam( pthread_self() , NULL, &param );
        printf("T1 run %d prio %d\n", i++, param.sched_priority);
    }
    
    pthread_mutex_unlock(&M1);
    
    pthread_getschedparam( pthread_self() , NULL, &param );
    printf("Task 1 : priorite apres  = %d\n", param.sched_priority);
    
    printf("T1 lâche M1\n");
}


void task2Func(void){
    printf("T2 commence sleep\n");
    sleep(2);
    printf("T2 veut M1\n");
    
    pthread_getschedparam( pthread_self() , NULL, &param );
    printf("Task 2 : priorite avant  = %d\n", param.sched_priority);
    
    
    pthread_mutex_lock(&M1);
    int i = 0;
    
 //   pthread_getschedparam( pthread_self() , NULL, &param );
 //   printf("Task 2 : priorite pendant  = %d\n", param.sched_priority);
    
    while (i < 10) {
        pthread_getschedparam( pthread_self() , NULL, &param );
        printf("T2 run %d prio %d\n", i++, param.sched_priority);
    }
    pthread_mutex_unlock(&M1);
    
    pthread_getschedparam( pthread_self() , NULL, &param );
    printf("Task 2 : priorite apres  = %d\n", param.sched_priority);
    
    printf("T2 lâche M1\n");
}







