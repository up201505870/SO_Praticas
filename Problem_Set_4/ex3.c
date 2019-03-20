#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *function(void *arg) {

    int *in = arg;

    printf("Thread %d: PID [%d] | TID [%d]\n", *in, (int) getpid(), (int) pthread_self());

    int *ret = (int *) malloc(sizeof(int));
    *ret = *in;

    return ret;
}

int main(int argc, char **argv) {

    if(argc != 2) {
        printf("Wrong use of program.\n");
        exit(0);        
    }

    int n = atoi(argv[1]);
    printf("Number of threads needed: %d\n", n);

    pthread_t tids[n];

    int arg[n];

    for(int i = 0; i < n; i++) {

        arg[i] = i + 1;
        pthread_create(&tids[i], NULL, function, (void *) &arg[i]);

    }

    int *ret[n];

    for(int i = 0; i < n; i++) {

        pthread_join(tids[i], (void **) &ret[i]);

    }

    printf("Main PID [%d]", getpid());

    for(int i = 0; i < n; i++) {

        printf(" | Thread %d - %d", i + 1, *ret[i]);

    }

    printf("\n");

    return 0;
}