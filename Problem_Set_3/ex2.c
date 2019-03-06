#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>

// Sao iguais :)

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        printf("Nope.\n");
        return -1;
    }

    int output;
    char path[32];


    if(fork() == 0) { // Child Process

        sprintf(path, "child_%s", argv[1]);
        printf("Child file created.\n");

        output = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

        int status;
        waitpid(getppid(), &status, 0);

    } else { // Parent Process

        sprintf(path, "parent_%s", argv[1]);
        printf("Parent file created.\n");

        output = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);

    }

    char *s = getenv("PATH");

    write(output, s, strlen(s));

    close(output);

    return 0;
}
