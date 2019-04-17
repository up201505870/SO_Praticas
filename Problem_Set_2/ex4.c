#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Wrong use of program.\n");
        return -1;
    }

    int file = open(argv[1], O_RDONLY);
    if(file < 0) {
        printf("Couldn't open file.\n");
        return -1;
    }

    char s[32] = "";
    sprintf(s, "%s.smp", argv[1]);

    int dest = open(s, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if(dest < 0) {
        printf("Couldn't create dest.\n");
        return -1;
    }

    struct stat buf;

    if(fstat(file, &buf) != 0) {
        printf("Couldn't Read stats properly.\n");
        return -1;
    }

    long size = buf.st_size;
    int offset = atoi(argv[2]);

    int sampleSize = (size - offset < size / 10) ? (size - offset) : (size / 10);

    if(lseek(file, offset, SEEK_SET) == -1) {
        printf("Couldn't lseek.\n");
        return -1;
    }

    char *buffer = (char *) malloc(sizeof(char) * sampleSize);

    if(read(file, buffer, sampleSize) == -1) {
        printf("Couldn't read.\n");
        return -1;
    }

    if(write(dest, buffer, sampleSize) == -1) {
        printf("Couldn't write.\n");
        return -1;
    }

    free(buffer);
    if(close(file) == -1) {
        printf("Couldn't close file.\n");
        return -1;
    }
    
    if(close(dest) == -1) {
        printf("Couldn't close dest.\n");
        return -1;
    }

    printf("Done.\n");

    return 0;
}