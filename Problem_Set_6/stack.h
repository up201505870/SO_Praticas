#ifndef STACK_H
#define STACK_H

struct stack_el {
	char *str;
	int n;
};

struct stack {    	
	struct stack_el **sp;  
	int size;            
	int top;			  
};

struct stack *init(int size);	// initialize stack. Returns pointer to
								//  to initialized stack of desired size
int push(struct stack *sp, struct stack_el *el); // returns 0 if the stack is full, and 1 otherwise
struct stack_el *pop(struct stack *sp); // returns NULL if the stack is empty
void show(struct stack *sp);
void spfree(struct stack *sp);	// frees all data structures allocated

#endif