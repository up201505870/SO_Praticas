#include <stdio.h>

#define N 10

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

int main(int argc, char **argv) {

    int fib[N];
    in_fibonnaci(fib, N);

    for(int i = 0; i < N; i++) {
        printf("Element %d: %d\n", i+1, fib[i]);
    }

    return 0;
}