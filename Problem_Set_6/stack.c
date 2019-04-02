#include "stack.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct stack *init(int size) {

    struct stack *sp = (struct stack *) malloc(sizeof(struct stack));

    sp->sp = (struct stack_el **)
            malloc(sizeof(struct stack_el *) * size);
    sp->size = size;
    sp->top = 0;
    sp->mutex = PTHREAD_MUTEX_INITIALIZER;
    sp->full  = PTHREAD_COND_INITIALIZER;
    sp->empty = PTHREAD_COND_INITIALIZER;

    return sp;
}

int push(struct stack *sp, struct stack_el *el) {

    sp->sp[sp->top] = el;
    sp->top++;
    return 1;
}

struct stack_el *pop(struct stack *sp) {

    struct stack_el *el = sp->sp[sp->top];
    sp->sp[sp->top] = NULL;
    sp->top--;
    return el;
}

void show(struct stack *sp) {

    printf("Showing\n");

    for(int i = 0; i < sp->top; i++) {
        printf("Element #%d: %d, %s", i + 1, sp->sp[i]->n, sp->sp[i]->str);
    }
    return;
}

void spfree(struct stack *sp) {

    free(sp->sp);
    free(sp);

    return;
}


