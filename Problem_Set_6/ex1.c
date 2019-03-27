#include <stdio.h>
#include <pthread.h>
#include "stack.h"

int main(int argc, char **argv) {

    struct stack *s = init(3);

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

    spfree(s);

    return 0;
}