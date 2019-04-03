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
    pthread_mutex_init(&sp->mutex, NULL);
    pthread_cond_init(&sp->full, NULL);
    pthread_cond_init(&sp->empty, NULL);

    return sp;
}

int push(struct stack *sp, struct stack_el *el) {

    pthread_mutex_lock(&sp->mutex);
    while(sp->top == sp->size)
        pthread_cond_wait(&sp->empty, &sp->mutex);
    sp->sp[sp->top] = el;
    sp->top++;
    printf("Pushed -> ");
    show(sp);
    pthread_cond_signal(&sp->full);
    pthread_mutex_unlock(&sp->mutex);
    return 1;
}

struct stack_el *pop(struct stack *sp) {

    pthread_mutex_lock(&sp->mutex);
    while(sp->top == 0)
        pthread_cond_wait(&sp->full, &sp->mutex);
    struct stack_el *el = sp->sp[sp->top];
    sp->sp[sp->top] = NULL;
    sp->top--;
    printf("Popped -> ");
    show(sp);
    pthread_cond_signal(&sp->empty);
    pthread_mutex_unlock(&sp->mutex);
    return el;
}

void show(struct stack *sp) {

    for(int i = 0; i < sp->top; i++) {
        printf("Element #%d: %d, %s", i + 1, sp->sp[i]->n, sp->sp[i]->str);
        if(i != sp->top - 1)
            printf(" | ");
    }
    printf("\n");
    return;
}

void spfree(struct stack *sp) {

    free(sp->sp);
    free(sp);

    return;
}


