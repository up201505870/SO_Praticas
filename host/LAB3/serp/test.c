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

    printf("---------WRITE--------\n");

    write(fd, "Ola!\n\0", 6);

    printf("-----------------\n");
    char x;
    close(fd);
    return 0;

}
