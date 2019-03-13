#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    if(argc < 2) {
        printf("Wrong use of program.\n");
        return -1;
    }

    int file = open(argv[1], O_RDONLY);
	if(file == -1) {
		printf("Problem reading file.\n");
		return -1;
	}

	char buffer[3];
	char name[30];
	int i = 0;
	int grade;

	while(read(file, buffer, 2) > 0) {
		if(buffer[0] == '\n' || i >= 30) {
			puts(name);
			memset(&name, 0, 30);

			buffer[0] = buffer[1];

			read(file, &buffer[1], 1);
			grade = strtol(buffer, NULL, 16);
			printf("Grade 1: %d\n", grade);
			memset(buffer, 0, 3);

			read(file, buffer, 2);
			grade = strtol(buffer, NULL, 16);
			printf("Grade 2: %d\n", grade);
			memset(buffer, 0, 3);

			read(file, buffer, 2);
			grade = strtol(buffer, NULL, 16);
			printf("Grade 3: %d\n\n", grade);
			memset(buffer, 0, 3);

			read(file, buffer, 1);
			i = 0;
		} else {
			name[i] = (char) strtol(buffer, NULL, 16);
			i++;
		}
	}
    close(file);

    return 0;
}
