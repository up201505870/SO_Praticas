#include "stack.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

stack_el *init(int n, char *str) {

    stack_el *stack = (stack_el *) malloc(sizeof(stack_el));

    stack->n = n;
    stack->str = str;
    stack->next = NULL;
    return stack;
}

void show(stack_el *stack) {
    
    while(stack->next != NULL) {
        printf("Object: %d - %s\n", stack->n, stack->str);
        stack = stack->next;
    }
    printf("Object: %d - %s\n", stack->n, stack->str);
}

void push(stack_el *stack, int n, char *str) {

    stack_el *s = (stack_el *) malloc(sizeof(stack_el));
    while(stack->next != NULL) {stack = stack->next;}
    s->n = n;
    s->str = str;
    s->next = NULL;

    stack->next = s;

}

char *pop(stack_el *stack) {


}