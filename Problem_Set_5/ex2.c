#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stack.h"

int main(int argc, char **argv) {

    stack_el *stack = init(1, "Yup");

    push(stack, 5, "Ola");
    push(stack, 100, "Lolz");
    
    show(stack);

    return 0;
}