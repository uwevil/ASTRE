#include "test.h"

pthread_t th1;
pthread_t th2;
pthread_t th3;


pthread_attr_t attr;
struct sched_param param;
pthread_mutex_t lock;


void *thread(void *data) {
    int i;
    for (i = 0; i < 100; i++) {
        printf("%d Hello world from thread\n", i);
    }
}

void *thread2(void *data){
    int i = 0;
    while(i < 5){
        printf("%s veut IN\n", data);
        pthread_mutex_lock(&lock);
        sleep(1);
        printf("%s IN\n", data);
        pthread_mutex_unlock(&lock);
        printf("%s OUT\n", data);
        i++;
    }
}

int main(void) {
    pthread_attr_init(&attr);
    
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = 1;
    pthread_attr_setschedparam(&attr, &param);
    
    pthread_mutex_init(&lock, NULL);

    
    /*
    pthread_create(&th, &attr, thread, NULL);
    printf("min FIFO : %d\n", sched_get_priority_min(SCHED_FIFO));
    printf("max FIFO : %d\n", sched_get_priority_max(SCHED_FIFO));
    printf("min RR : %d\n", sched_get_priority_min(SCHED_RR));
    printf("max RR : %d\n", sched_get_priority_max(SCHED_RR));
//    printf("min OTHERS : %d\n", sched_get_priority_min(SCHED_OTHERS));
//   printf("max OTHERS : %d\n", sched_get_priority_max(SCHED_OTHERS));
    pthread_join(th, NULL);
 */
    
    pthread_create(&th1, &attr, thread2, "a");
    pthread_create(&th2, &attr, thread2, "b");
    pthread_create(&th3, &attr, thread2, "c");

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);

    pthread_mutex_destroy(&lock);
    
    
    
    
}




