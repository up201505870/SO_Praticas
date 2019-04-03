#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "stack.h"

int m, n;
int readers;

pthread_cond_t reading;
pthread_mutex_t mutex;

void *reader(void *args) {

    for(int i = 0; i < 50000000; i++) {

        pthread_mutex_lock(&mutex);
        readers++;
        pthread_mutex_unlock(&mutex);

        int _m = m;
        int _n = n;

        if(i % 1000000 == 0) {
            printf("Reader - m: %d | n: %d | diff: %d\n", _m, _n, _m - _n);
        }

        pthread_mutex_lock(&mutex);
        readers--;
        if(readers == 0)
            pthread_cond_signal(&reading);
        pthread_mutex_unlock(&mutex);
    }

}

void *writer(void *args) {

    for(int i = 0; i < 50000000; i++) {

        pthread_mutex_lock(&mutex);
        while(readers != 0)
            pthread_cond_wait(&reading, &mutex);
        m++;
        n++;
        pthread_mutex_unlock(&mutex);
    }

}

int main() {

    m = 0; n = 0; readers = 0;

    pthread_t wr[2];
    pthread_t rd[2];

    pthread_cond_init(&reading, NULL);
    pthread_mutex_init(&mutex, NULL);

    for(int i = 0; i < 2; i++) {
        pthread_create(&wr[i], NULL, writer, NULL);
        pthread_create(&rd[i], NULL, reader, NULL);
    }

    pthread_join(rd[0], NULL);
    pthread_join(rd[1], NULL);
    pthread_join(wr[0], NULL);
    pthread_join(wr[1], NULL);
    printf("Final - m: %d | n: %d | diff: %d \n", m, n, m - n);

}