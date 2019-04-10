#include <stdio.h>

int main(int argc, char **argv) {

    // Alinea a)
    printf("\nAlinea a)\n");
    char c;
    short s;
    int i;
    long l;
    long long ll;

    printf("Size of char:       %ld\n", sizeof(c));
    printf("Size of short:      %ld\n", sizeof(s));
    printf("Size of int:        %ld\n", sizeof(i));
    printf("Size of long:       %ld\n", sizeof(l));
    printf("Size of long long:  %ld\n", sizeof(ll));

    // Alinea b)
    printf("\nAlinea b)\n");

    int a[10];
    struct complex {
        double re, im;
    } z;
    char *hello = "Hello, World!";

    printf("Size of a:          %ld\n", sizeof(a));
    printf("Size of complex:    %ld\n", sizeof(z));
    printf("Size of hello:      %ld\n", sizeof(hello));

    // Alinea c)
    printf("\nAlinea c)\n");

    printf("Size of a[0]:       %ld\n", sizeof(a[0]));
    printf("Size of *hello:     %ld\n", sizeof(*hello));

    return 0;
}