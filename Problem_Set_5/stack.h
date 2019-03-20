#ifndef STACK_H
#define STACK_H

struct stack_el{
		char *str;              /* String to be printed in show() */
		int n;                  /* Integer to be printed in show() */
		struct stack_el *next;  /* Link to next stack element */
};

typedef struct stack_el stack_el;
stack_el *init();
void show(stack_el *stack);
void push(stack_el *stack, int n, char *str);
char *pop(stack_el *stack);

#endif