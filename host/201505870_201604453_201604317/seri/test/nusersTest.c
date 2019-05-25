#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    printf("[fd0] Opening file once.\n");
    int fd0 = open("/dev/seri0", O_RDONLY);

    if (fd0 < 0) {

        if (errno == -EBUSY) {
            printf("[fd0] Special file already in use.\n");
        } else { 
            printf("[fd0] Error opening file.\n");
        }
        return -1;
        
    }

    printf("[fd1] Opening file twice.\n");
    int fd1 = open("/dev/seri0", O_RDONLY);

    if (fd1 < 0) {
        if (errno == EBUSY) {
            printf("[fd1] Special file already in use.\n");
        } else { 
            printf("[fd1] Error opening file.\n");
            return -1;
        }
    }

    printf("Closing first file descriptor.\n");
    close(fd0);
    
    printf("[fd2] Opening file again.\n");
    int fd2 = open("/dev/seri0", O_RDONLY);

    if (fd2 < 0) {
        if (errno == EBUSY) {
            printf("[fd2] Special file already in use.\n");
        } else { 
            printf("[fd2] Error opening file.\n");
            return -1;
        }
    }

    close(fd0);
    close(fd1);
    close(fd2);

    return 0;

}
