#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

    char *argv2[] = {"NULL"};
    char *envp[] = {"NULL"};

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i+1, argv[i]);
    }

    printf("\nSystem Variables: \n");
    char *s = getenv("PATH");

    printf("%s\n", s);

    free(s);

    return 0;
}
