#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "stack.h"

typedef struct {
    struct stack *sp;
    int i;
} args_t;

void *input(void *args) {

    args_t *in = (args_t *) args;
    for(int i = 0; i < in->i; i++) {
        struct stack_el *el = (struct stack_el *) malloc(sizeof(struct stack_el));
        el->n = i;
        el->str = "xxx";
        push(in->sp, el);
    }
}

void *output(void *args) {

    args_t *in = (args_t *) args;
    for(int i = 0; i < in->i; i++) {
        struct stack_el *el = (struct stack_el *) malloc(sizeof(struct stack_el));
        el = pop(in->sp);
    }

}

int main(int argc, char **argv) {

    struct stack *s = init(7);

    pthread_t tid[2];

    args_t arg1;
    arg1.i = 10;
    arg1.sp = s;
    pthread_create(&tid[1], NULL, input, &arg1);
    pthread_create(&tid[0], NULL, output, &arg1);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}