#include <stdio.h>
#include <stdlib.h>

void in_fibonacci(int *fib, int n) {

    int hi = 1;
    int lo = 0;

    for(int i = 0; i < n; i++) {
        int extra = hi;
        hi = hi + lo;
        lo = extra;
    
        fib[i] = lo;
    }
    
    return;

}

int main() {

    int *x;
    int n = 10;
    
    x = (int*) malloc(n * sizeof(int));

    in_fibonacci(x ,n);

    for(int i = 0; i < n; i++) {
    
        printf("%d\n", x[i]);
    }
    
    return 0;

}
