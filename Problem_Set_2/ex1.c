#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 

#define BUFFERSIZE 64

int main(int argc, char **argv) {

	if (argc != 3) {
		printf("Wrong use of program.\n");
		return -1;
	}

	int file = open(argv[1], O_RDONLY);
	int file2 = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

	if (file2 < 0 ) { // Error while creating file
		printf("File already exists.\n");
		return -1;
	}

	int size = 0;
	char buffer[BUFFERSIZE];
	memset(buffer, 0, BUFFERSIZE);
	int b;

	while( (b = read(file, buffer, BUFFERSIZE)) != 0) {

		printf("%s", buffer);
		size += b;
		write(file2, buffer, b);

		memset(buffer, 0, BUFFERSIZE);

	}

	close(file);
	close(file2);

	printf("\nRead %d bytes.\n", size);

	return 0;
}