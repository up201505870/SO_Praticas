#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef char name[30];
typedef struct {
	name st;
	int grades[3];
} test_t;

int main(int argc, char **argv) {

    test_t test;
    int grade;

    if(argc < 2) {
        printf("Wrong use of program.\n");
        return -1;
    }

    int file = open(argv[1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    printf("Name of student: ");
    fgets( test.st, 30, stdin);

    if(test.st[strlen(test.st) - 1] == '\n')
        test.st[strlen(test.st) - 1] = '\0';

    printf("First Grade: ");
    scanf("%d", &test.grades[0]);

    printf("Second Grade: ");
    scanf("%d", &test.grades[1]);

    printf("Third Grade: ");
    scanf("%d", &test.grades[2]);

    printf("%s: ", test.st);
    printf("%d | %d | %d\n", test.grades[0], test.grades[1], test.grades[2]);

    char buff[3];

    for(int i = 0; i < strlen(test.st); i++) {

        sprintf(buff, "%x", (int)test.st[i]);
        write(file, buff, 2);

    }
    write(file, "\n", 1);

    for(int i = 0; i < 3; i++) {

        sprintf(buff, "%x", test.grades[i]);
        write(file, buff, 2);
    }
	write(file, "\n", 1);
    close(file);

    return 0;
}
