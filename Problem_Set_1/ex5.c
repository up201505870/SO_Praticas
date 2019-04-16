#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int myStrlen(char *str) {

    int count = 0;
    char c;
    while( (c = str[count]) != '\0') {

        count++;

    }

    return count;

}

int main (int argc, char **argv) {

    if (argc != 2) {
        printf("Wrong use of program\n");
        return -1;
    }

    char *str = argv[1];

    printf("strlen: %d\n", strlen(str));
    printf("myStrlen: %d\n", myStrlen(str));

    return 0;
}