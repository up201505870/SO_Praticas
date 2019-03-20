#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int *array;
    int startPos;
    int numberOfInits;
} Arg;

void * initialize(void *arg) {

    Arg *in = (Arg *) arg;

    for(int i = 0; i < in->numberOfInits; i++) {
        int n = i + in->startPos;
        in->array[n] = n;
    }
    return NULL;
}

int main(int argc, char **argv) {

    if(argc != 3) {
        printf("Wrong use of program.\n");
        exit(0);
    }

    int size = atoi(argv[1]);
    int nthreads = atoi(argv[2]);
    Arg args[nthreads];

    int *array = (int *) malloc(sizeof(int) * size);

    pthread_t tids[nthreads];

    int currentPos = 0;
    int numberOfInits = size / nthreads;
    int extraInits = size - numberOfInits * nthreads; // Calcula o numero de inits extra, para dividir igualmente pelas threads.

    for(int i = 0; i < nthreads; i++) {
        args[i].array = array;
        args[i].startPos = currentPos;

        if(i < extraInits) {
            args[i].numberOfInits = numberOfInits + 1;
            currentPos += numberOfInits + 1;
        } else {
            args[i].numberOfInits = numberOfInits;
            currentPos += numberOfInits;
        }

        pthread_create(&tids[i], NULL, initialize, &args[i]);
    }

    for(int i = 0; i < nthreads; i++) {
        pthread_join(tids[i] ,NULL);
    }

    printf("Array = [");
    for(int i = 0; i < size; i++) {

        if(i != 0)
            printf(", ");
        printf("%d", args[0].array[i]);

    }
    printf("]\n");

    return 0;
}