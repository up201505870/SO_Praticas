#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>

int main(int argc, char const *argv[]) {

    int stat;

    if(fork() == 0) { // Child Process
        if(fork() == 0) {

            printf("[GC] Current PID: %d - Parent PID: %d\n", getpid(), getppid());

            exit(3);

        }

        wait(&stat);

        printf("[C] Current PID: %d - Parent PID: %d - Child exit code: %d\n", getpid(), getppid(), WEXITSTATUS(stat));

        exit(2);

    } else {

        wait(&stat);

        printf("[P] Current PID: %d - Parent PID: %d - Child exit code: %d\n", getpid(), getppid(), WEXITSTATUS(stat));

    }

    return 0;
}
