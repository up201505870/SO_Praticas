#ifndef SERI_H
#define SERI_H

#define SERI_MAJOR 0   /* dynamic major by default */
#define SERI_DEVS  4   /* seri0 through seri3 */

#define READ_TIMEOUT 4 // Timeout for read system call in seconds

#define FIFO_SIZE 128 // Size for fifo

#define UART_BASE 0x3f8 // Base address of UART

//ioctl enhancement
#define SERI_IOC_MAGIC 'p'
#define GET_SERI _IOW(SERI_IOC_MAGIC,  0, int)
#define SET_SERI _IOW(SERI_IOC_MAGIC,  1, int)

#define SERI_IOC_MAXNR 1

#endif
