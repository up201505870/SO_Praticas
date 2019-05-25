#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong usage: %s <text_to_write>\n", argv[0]);
        return 0;
    }

    int fd0 = open("/dev/serp0", O_WRONLY);

    if (fd0 < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Writing...\n");

    int status = write(fd0, argv[1], strlen(argv[1]));

    if (status != 0) {
        printf("Error writing to Serial Port /dev/seri0\n");
    }

    close(fd0);
    return 0;

}
