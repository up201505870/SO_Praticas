#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile long int counter;
pthread_mutex_t mptr = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {

    int *in = (int *) arg;

    for(int i = 0; i < *in; i++) {
        pthread_mutex_lock(&mptr);
        counter++;
        pthread_mutex_unlock(&mptr);
    }

}

int main(int argc, char **argv) {

    if(argc != 2) {
        printf("Wrong use of program.\n");
        exit(0);
    }

    counter = 0;

    int n = atoi(argv[1]);

    pthread_t tids[3];

    pthread_mutex_init(&mptr, NULL);

    pthread_create(&tids[0], NULL, increment, &n);
    pthread_create(&tids[1], NULL, increment, &n);
    pthread_create(&tids[2], NULL, increment, &n);

    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);
    pthread_join(tids[2], NULL);

    printf("Counter: %ld\n", counter);

    return 0;
}