#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {

    if(argc <= 1) {
        printf("Incorrect use of program.\n");
        return 0;
    }

    char prefix = argv[1][0];
    char fileName[16];

    char append1 = 'a';
    char append2 = 'a';

    char mergedPath[16];
    sprintf(mergedPath, "%c.mrg", prefix);

    int mergedFile = open(mergedPath, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, S_IRUSR | S_IWUSR);

    char buffer[64];

    while(1) {

        sprintf(fileName, "split/%c%c%c", prefix, append1, append2);

        int splitFile = open(fileName, O_RDONLY);

        if(splitFile < 0)
            break;

        int size;
        while((size = read(splitFile, buffer, 64)) > 0)
            write(mergedFile, buffer, size);

        close(splitFile);

        append2 += 1;

        if(append2 > 'z') {
            append1 += 1;
            append2 = 'a';
        }

        if(append1 > 'z')
            break;

    }

    printf("Done!\n");

    close(mergedFile);

    return 0;
}
