#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd = open("/dev/seri", O_RDWR);

    if (fd < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    char x;

    read(fd, &x, 1);

    close(fd);
    return 0;

}
