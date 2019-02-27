#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

void usage();

int main(int argc, char **argv) {

	if(argc == 1) {
		usage();
		return -1;
	}

	int file = open(argv[1], O_RDONLY);

	if(file < 0) {
		write(STDOUT_FILENO, "Could not open file. Exiting\n", 63);
		return -1;
	}

	char buf[64];
	int size;
	int totalSize = 0;

	if(argc == 2) {

		while((size = read(file, buf, 64)) > 0) {
			write(STDOUT_FILENO, buf, size);
			totalSize += size;
		}

	} else if(argc == 3) {

		int output = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

		if(output < 0) {
			printf("%s is an existing file.\n", argv[2]);
			close(file);
			return 0;
		}

		while((size = read(file, buf, 64)) > 0) {
			write(output, buf, size);
			totalSize += size;
		}

		close(output);

	}

	close(file);

	printf("\nNumero total de bytes lidos: %d\n", totalSize);

	return 0;
}

void usage() {

	write(STDOUT_FILENO, "Usage: read_file \"file.txt\"\n", 30);

}
