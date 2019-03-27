#include "stack.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct stack *init(int size) {

    struct stack *sp = (struct stack *) malloc(sizeof(struct stack));

    sp->sp = (struct stack_el **)
            malloc(sizeof(struct stack_el *) * size);
    sp->size = size;
    sp->top = 0;

    return sp;
}

int push(struct stack *sp, struct stack_el *el) {

    if(sp->top == sp->size) {
        return 0;
    }

    sp->sp[sp->top] = el;
    sp->top++;
    return 1;
}

struct stack_el *pop(struct stack *sp) {

    if(sp->top == 0)
        return NULL;

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