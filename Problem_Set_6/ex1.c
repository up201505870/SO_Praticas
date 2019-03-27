#include <stdio.h>
#include <pthread.h>
#include "stack.h"

pthread_mutex_t empty;
pthread_mutex_t full;

void tfun(void *args) {

    struct stack *s = (struct stack *) args;

    struct stack_el el1, el2, el3;
    el1.n = 10;
    el1.str = "Ola\n";
    el2.n = 20;
    el2.str = "Ok\n";
    el3.n = 30;
    el3.str = "Nope\n";

    push(s, &el1);
    push(s, &el2);
    push(s, &el3);

    show(s);

    struct stack_el *el = pop(s);
}

int main(int argc, char **argv) {

    struct stack *s = init(6);

    pthread_t thread[5];

    for(int i = 0; i < 5; i++) {
        pthread_create(&thread[i], NULL, tfun, s);
    }

    for(int i = 0; i < 5; i++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}