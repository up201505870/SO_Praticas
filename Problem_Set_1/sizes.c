#include <stdio.h>

int a[10];

struct complex {
    double re, im;
} z;

char *hello = "Hello, World!";

int main(int argc, char** argv) {

    printf("Size of char: %ld\n", sizeof(char));
    printf("Size of short: %ld\n", sizeof(short));
    printf("Size of int: %ld\n", sizeof(int));
    printf("Size of double: %ld\n", sizeof(double));
    printf("Size of long: %ld\n", sizeof(long));
    printf("Size of long long: %ld\n", sizeof(long long));
    
    printf("Size of a vector: %ld\n", sizeof(a));
    printf("Size of complex struct: %ld\n", sizeof(z));
    printf("Size of hello string: %ld\n", sizeof(hello));
        
    printf("Size of a[0]: %ld\n", sizeof(a[0]));
    printf("Size of *hello: %ld\n", sizeof(*hello));

    return 0;
}
