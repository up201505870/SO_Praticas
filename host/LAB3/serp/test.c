#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd = open("/dev/serp", O_RDWR);

    if (fd < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Writing...\n");

    write(fd, "Aloha\n", 6);

    printf("Reading...\n");

    char *c = (char *) malloc(sizeof(char) * 32);
    int s = 0;

    s = read(fd, c, 32);
    if (s == 1) {
        printf("Received string - %s\n", c);
    }

    close(fd);
    return 0;

}
