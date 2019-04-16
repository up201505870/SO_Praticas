#include <stdio.h>
#include <stdlib.h>

void in_fibonnaci(int *fib, int n) {

    int lw = 0;
    int hg = 1;

    fib[0] = 0;

    for(int i = 1; i < n; i++) {
    
        fib[i] = hg;
        int xtra = hg;
        hg = lw + hg;
        lw = xtra;

    }

}

int main (int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong use of program.\n");
        return -1;

    }

    int n = atoi(argv[1]);
    long int n1 = strtoll(argv[1], NULL, 10);

    int *fib = (int *) malloc(n * sizeof(int));
    int *fib1 = (int *) malloc(n1 * sizeof(int));

    in_fibonnaci(fib, n);
    in_fibonnaci(fib1, n1);

    for(int i = 0; i < n; i++) {
        printf("Element %d: %d\n", i+1, fib[i]);
    }
    
    for(int i = 0; i < n1; i++) {
        printf("Element %d: %d\n", i+1, fib1[i]);
    }

    return 0;
}