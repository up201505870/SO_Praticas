#include <stdio.h>
#include <string.h>

int my_strlen(char *in) {

	int i;

	for(i = 0; in[i] != '\0'; i++);

	return i;

}

int main(int argc, char **argv) {

	if(argc <= 1)
		return -1;

	printf("Size of input string \"%s\" is %d, my_strlen, and %ld, strlen.\n", argv[1], my_strlen(argv[1]), strlen(argv[1]));

	return 0;
}
