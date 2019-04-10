#include <stdio.h>

int fibonnaci(int arg1, int arg2, int n) {

    if (n == 3)
        return arg1 + arg2;

    return fibonnaci(arg2, arg1 + arg2, n - 1);

}

int main(int argc, char **argv) {

    int x = fibonnaci(0, 1, 10);
    printf("Result: %d\n", x);

    return 0;
}