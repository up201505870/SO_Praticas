#include <stdio.h>
#include <pthread.h>
#include "stack.h"

pthread_mutex_t empty;
pthread_mutex_t full;

int main(int argc, char **argv) {

    struct stack *s = init(6);


    return 0;
}