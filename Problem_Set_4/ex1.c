#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int a;
    int b;
} params;

void *addition(void *arg) {
    params *in = arg;
    int a = (int) in->a;
    int b = (int) in->b;
    int op = a + b;
    int *ret = (int *) malloc(sizeof(int));
    *ret = op;

    return ret;
}

void *subtraction(void *arg) {
    params *in = arg;
    int a = (int) in->a;
    int b = (int) in->b;
    int op = a - b;
    int *ret = (int *) malloc(sizeof(int));
    *ret = op;

    return ret;
}

void *multiply(void *arg) {
    int *in = arg;
    int a = in[0];
    int b = in[1];
    int op = a * b;
    int *ret = (int *) malloc(sizeof(int));
    *ret = op;

    return ret;
}

void *division(void *arg) {
    int *in = arg;
    int a = in[0];
    int b = in[1];
    int op = a / b;
    int *ret = (int *) malloc(sizeof(int));
    *ret = op;
    
    return ret;
}

int main(int argc, char const *argv[]) {

    if(argc != 3) {
        printf("Wrong use of program.\n");
        return -1;
    }

    params paramaters1;

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    paramaters1.a = a;
    paramaters1.b = b;

    int paramaters2[2] = {a, b};

    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_t tid4;

    pthread_create(&tid1, NULL, addition, &paramaters1);
    pthread_create(&tid2, NULL, subtraction, &paramaters1);
    pthread_create(&tid3, NULL, multiply, paramaters2);
    pthread_create(&tid4, NULL, division, paramaters2);

    int *ret[4];

    pthread_join(tid1, (void **) &ret[0]);
    pthread_join(tid2, (void **) &ret[1]);
    pthread_join(tid3, (void **) &ret[2]);
    pthread_join(tid4, (void **) &ret[3]);

    printf("%d + %d = %d\n", a, b, *ret[0]);
    printf("%d - %d = %d\n", a, b, *ret[1]);
    printf("%d * %d = %d\n", a, b, *ret[2]);
    printf("%d / %d = %d\n", a, b, *ret[3]);

    return 0;
}
