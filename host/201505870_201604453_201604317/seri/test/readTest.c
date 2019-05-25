#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong usage: %s <bytes_to_read>\n", argv[0]);
        return 0;
    }

    int n = atoi(argv[1]);

    int fd0 = open("/dev/seri0", O_RDONLY);

    if (fd0 < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Reading...\n");

    char *c = (char *) malloc(sizeof(char) * n);
    int s = 0;

    s = read(fd0, c, n);
    if (s == 0) {
        printf("Received string - %s\n", c);
    } else {
        printf("Error reading: %d\n", s);
    }

    free(c);

    close(fd0);
    return 0;

}
