#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("Wrong use of program.\n");
        return -1;
    }

    int stat;
    char *const parmList[] = {"/usr/bin/gcc", argv[1], NULL};

    if(fork() == 0) { // Child process

        execv("/usr/bin/gcc", parmList);

    } else { // Parent process

        wait(&stat);


    }

    return 0;
}
