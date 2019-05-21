#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd0 = open("/dev/serp0", O_RDWR);
    int fd1 = open("/dev/serp1", O_RDWR);

    if (fd0 < 0 || fd1 < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Writing...\n");

    write(fd0, "Aloha\n", 7);
    write(fd1, "Hey!\n", 6);

    printf("Reading...\n");

    char *c = (char *) malloc(sizeof(char) * 32);
    int s = 0;

    s = read(fd0, c, 32);
    if (s == 1) {
        printf("[fd0] Received string - %s\n", c);
    } else {
        printf("[fd0] Error reading: %d\n", s);
    }

    s = read(fd1, c, 32);
    if (s == 1) {
        printf("[fd1] Received string - %s\n", c);
    } else {
        printf("[fd1] Error reading: %d\n", s);
    }

    close(fd0);
    close(fd1);
    return 0;

}
