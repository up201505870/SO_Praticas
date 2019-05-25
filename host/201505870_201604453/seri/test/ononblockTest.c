#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char **argv) {

    int fd0 = open("/dev/seri0", O_RDONLY | O_NONBLOCK);

    if (fd0 < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Reading...\n");

    char *c = (char *) malloc(sizeof(char) * 5);
    int s = 0;

    s = read(fd0, c, 5);
    if (s == 0) {
        printf("Received string - %s\n", c);
    } else {
        if(errno == EAGAIN)
            printf("NONBLOCK.\n", s);
        else {
            printf("Error reading.\n");
        }
    }

    free(c);

    close(fd0);
    return 0;

}
