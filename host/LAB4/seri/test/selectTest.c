#define _XOPEN_SOURCE
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <poll.h>

int main(void) {

    int fd0 = open("/dev/seri0", O_RDWR);
    struct pollfd pfd;

    pfd.fd = fd0;
    pfd.events = POLLIN | POLLRDNORM | POLLWRNORM;

    poll(&pfd, 1, -1);

    if (pfd.revents == (POLLIN | POLLRDNORM)) {
        printf("Readable.\n");
    }
    if (pfd.revents == (POLLIN | POLLWRNORM)) {
        printf("Writable.\n");
    } 
    if (pfd.revents == (POLLIN | POLLRDNORM | POLLWRNORM)) {
        printf("Readable and Writable.\n");
    }

    return 0;
}
