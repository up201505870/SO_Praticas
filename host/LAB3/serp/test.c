#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong use of program.\n");
        return -1;
    }

    int fd = open(argv[1], O_RDWR);

    if (fd < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    printf("Reading...\n");

    char c = ' ';
    int s = 0;

    while(c != 'x') {

        s = read(fd, &c, 1);
        if (s == 1)
            printf("%c", c);

    }

    close(fd);
    return 0;

}
