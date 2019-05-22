#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

    // printf("%s", );

    if(strcmp(argv[1], "1") == 0)
        read(fd0, &x, 1);
    else 
        write(fd0, "1", 1);

    
    close(fd0);
    return 0;

}
