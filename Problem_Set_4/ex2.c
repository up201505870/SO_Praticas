#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *function(void *arg) {

    int *in = arg;

    printf("PID [%d] | TID [%d]\n", (int) getpid(), (int) pthread_self());

    int *ret = (int *) malloc(sizeof(int));
    *ret = *in;

    return ret;
}

int main(int argc, char **argv) {

    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    int arg1 = 1;
    int arg2 = 3;
    int arg3 = 2;

    pthread_create(&tid1, NULL, function, (void *) &arg1);
    pthread_create(&tid2, NULL, function, (void *) &arg2);
    pthread_create(&tid3, NULL, function, (void *) &arg3);

    int *ret[3];

    pthread_join(tid1, (void **) &ret[0]);
    pthread_join(tid2, (void **) &ret[1]);
    pthread_join(tid3, (void **) &ret[2]);

    printf("Main PID [%d] | t1 - %d | t2 - %d | t3 - %d\n", 
                    getpid(), *ret[0], *ret[1], *ret[2]);

    return 0;
}