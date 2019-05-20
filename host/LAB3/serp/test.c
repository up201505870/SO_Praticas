#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd = open("/dev/serp", O_RDWR);

    if (fd < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Reading...\n");

    char c = ' ';
    int s = 0;

    while(c != 'x') {

        s = read(fd, &c, 1);
        if (s == 1) {
            printf("Received char - %c\n", c);
        }

    }

    close(fd);
    return 0;

}
