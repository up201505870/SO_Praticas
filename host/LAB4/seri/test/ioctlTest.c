#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include "../seri.h"
#include "../serial_reg.h"

void display_config(unsigned int config) {

    unsigned char msb = (unsigned char) ((config & 0x00FF0000)>>16);
    unsigned char lsb = (unsigned char) ((config & 0x0000FF00)>>8);
    unsigned char lcr = (unsigned char) (config & 0x000000FF);

    int div = ( ((int)msb)<<8 ) | ((int) lsb);
    int bitrate = 115200/div;
    printf("BITRATE: %d\n", bitrate);
    printf("CHAR_WIDTH: %d\n", (int) (lcr & 0x03) + 5);
    printf("STOP_BITS: %d\n", (int) ((lcr & 0x04)>>2) + 1);
    printf("PARTY: ");
    if(!(lcr & 0x08)) printf("NONE\n");
    else {

        switch((lcr & 0x30)>>4) {
            case 0: printf("Odd\n"); break;
            case 1: printf("Even\n"); break;
            case 2: printf("1\n"); break;
            case 3: printf("0\n"); break;
        }
    } 
    printf("BREAK ENABLED: %d\n", lcr & UART_LCR_SBC);
    printf("\n");

}

unsigned int create_config(int bitrate, int char_width, int parity, int stop_bits) {

    unsigned char lcr = 0x00;
    unsigned char msb, lsb;
    int div;

    if(bitrate == 9600) {
        div = UART_DIV_9600;
    } else if (bitrate == 1200) {
        div = UART_DIV_1200;
    }

    msb = (div >> 8) ; // MSB
	lsb = div & 0xff; // LSB

    lcr |= (char_width - 5) | ((stop_bits - 1) << 2) | (parity << 3);

    return (msb << 16) | (lsb << 8) | lcr;

}

int main(int argc, char const **argv) {
    
    if (argc != 6 && argc != 2) {
        printf("Wrong usage: %s GET|SET [BIT_RATE CHAR_WIDTH STOP_BITS PARTIY\nBIT_RATE: 1200, 9600\nCHAR_WIDTH: 5, 6, 7, 8\nSTOP_BITS: 1, 2\nPARITY: None, Odd, Even, 1, 0\n", argv[0]);
        return -1;
    }

    int fd = open("/dev/seri0", O_RDWR);

    if (fd < 0) {

        printf("Error opening file.\n");
        return -1;
        
    }

    if ( !strcmp(argv[1], "GET") ) {

        int config = ioctl(fd, GET_SERI);
        display_config(config);

    } else if (!strcmp(argv[1], "SET")) {

        int bitrate = atoi(argv[2]);
        if (bitrate != 1200 && bitrate != 9600 ) {
            printf("Bad BITRATE\n"); return -1;
        } 

        int char_width = atoi(argv[3]);
        if (char_width < 5 || char_width > 8) {
            printf("Bad CHAR_WIDTH\n"); return -1;
        }

        int stop_bits = atoi(argv[4]);
        if (stop_bits != 1 && stop_bits != 2) {
            printf("Bad STOP_BITS\n"); return -1;
        }

        int parity = -1;
        if (!strcmp(argv[5], "None")) parity = 0;
        else if (!strcmp(argv[5], "Odd")) parity = 1;
        else if (!strcmp(argv[5], "Even")) parity = 3;
        else if (!strcmp(argv[5], "1")) parity = 5;
        else if (!strcmp(argv[5], "0")) parity = 7;

        if (parity == -1) {
            printf("Bad PARITY\n"); return -1;
        }

        int config = create_config(bitrate, char_width, parity, stop_bits);
        ioctl(fd, SET_SERI, config);

    } else {
        printf("Wrong usage: %s GET|SET [BIT_RATE CHAR_WIDTH PARTIY STOP_BITS\nBIT_RATE: 1200, 9600\nCHAR_WIDTH: 5, 6, 7, 8\nPARITY: None, Odd, Even, 1, 0\nSTOP_BITS: 1, 2", argv[0]);
        return -1;
    }

    return 0;
}
