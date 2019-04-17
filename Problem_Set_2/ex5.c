#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFFERSIZE 64

int main(int argc, char **argv) {

    if (argc != 2 && argc != 3) {
		printf("Wrong use of program.\n");
		return -1;
	}

	int file = open(argv[1], O_RDONLY);
    if (file < 0) {
        printf("Error opening file.\n");
		return -1;
    }

    int file2;
    int std;

    if(argc == 3) {

        file2 = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);

        if (file2 < 0 ) { // Error while creating file
            printf("File already exists.\n");
            return -1;
        }

	    std = dup2(file2, STDOUT_FILENO);
    } 

	int size = 0;
	char buffer[BUFFERSIZE];
	memset(buffer, 0, BUFFERSIZE);
	int b;

	while( (b = read(file, buffer, BUFFERSIZE)) != 0) {

		size += b;
		write(STDOUT_FILENO, buffer, b);

		memset(buffer, 0, BUFFERSIZE);

	}

	close(file);

    if(argc == 3)
        close(file2);


    if(dup2(std, STDOUT_FILENO) == -1) {
        printf("Error.\n");
        return -1;
    }

	printf("\nRead %d bytes.\n", size);

    return 0;
}