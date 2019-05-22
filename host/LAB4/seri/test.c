#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    int fd0 = open("/dev/seri0", O_RDWR);

    if (fd0 < 0) {

        printf("Error opening file 0.\n");
        return -1;
        
    }

    char x;

    read(fd0, &x, 1);
    
    close(fd0);
    return 0;

}
