
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void usage(char *in);

int main(int argc, char **argv) {

	if(argc == 1) {
		usage(argv[0]);
		return -1;
	}

	int file = open(argv[1], O_RDONLY);

	if(file < 0) {
		write(STDOUT_FILENO, "Could not open file. Exiting\n", 63);
		return -1;
	}

	char buf[64];

	while(read(file, buf, 64) != 0)
		write(STDOUT_FILENO, buf, 64);

	close(file);

	return 0;
}

void usage(char *in) {

	write(STDOUT_FILENO, "Usage: read_file \"file.txt\"\n", 30);

}
