/*                                                     
 * $Id: seri.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include "serial_reg.h"
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

#define SERI_MAJOR 0   /* dynamic major by default */
#define SERI_DEVS  4   /* seri0 through seri3 */

#define UART_BASE 0x3f8

static int seri_open(struct inode *inode, struct file *filp);
static int seri_release(struct inode *inode, struct file *filp);
ssize_t seri_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
ssize_t seri_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);

irqreturn_t seri_interrupt(int irq, void *dev_id);

struct seri_dev {
	struct cdev cdev;
	struct semaphore mutex;
	struct kfifo *rxfifo;
};

int seri_major = SERI_MAJOR;
int seri_devs  = SERI_DEVS;

struct seri_dev *seri_devices;

struct file_operations seri_fops = {
	.owner 		= THIS_MODULE,
	.open  		= seri_open,
	.release 	= seri_release,
	.write		= seri_write,
	.read 		= seri_read,
	.llseek		= no_llseek,
};

irqreturn_t seri_interrupt(int irq, void *dev_id) {

	while (!inb(UART_BASE + UART_LSR_THRE)) { // Check if THRE is empty and ready to receive a byte
		schedule(); // If not this function allows for the SO to do something else (acho eu xD)
	}

	outb('i', UART_BASE + UART_TX);

	return IRQ_HANDLED;

}

static int seri_open(struct inode *inode, struct file *filp) {

	struct seri_dev *dev;
	int status;

	dev = container_of(inode->i_cdev, struct seri_dev, cdev);

	if (status) {
		printk(KERN_ALERT "Error with requesting IRQ %d.\n", status);
	}

	filp->private_data = dev;

	return 0;

}

static int seri_release(struct inode *inode, struct file *filp) {

	return 0;

}

ssize_t seri_read(struct file *filep, char __user *buff, size_t count, loff_t *offp) {

	struct seri_dev *dev = filep->private_data;

	return 0;

}

ssize_t seri_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp) {

	return 0;

}

static void seri_setup_cdev(struct seri_dev *dev, int index)
{
	int err, devno = MKDEV(seri_major, index);
    
	cdev_init(&dev->cdev, &seri_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &seri_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding seri%d", err, index);
}

static int seri_init(void)
{
	int status, i;
	dev_t dev = MKDEV(seri_major, 0);
	unsigned char lcr = 0; // Line Control
	unsigned char msb = 0, lsb = 0; // Msb and Lsb for DL
	unsigned char ier = 0;

	if (seri_major) {
		status = register_chrdev_region(dev, seri_devs, "seri");
	} else {
		status = alloc_chrdev_region(&dev, 0, seri_devs, "seri");
		seri_major = MAJOR(dev);
	}
	if (status < 0)
		return status;

	// Allocate the devices
	seri_devices = kmalloc(seri_devs * sizeof(struct seri_dev), GFP_KERNEL);
	if (!seri_devices) {
		status = -ENOMEM;
		goto fail_malloc;
	}
	memset(seri_devices, 0, seri_devs * sizeof(struct seri_dev));
	
	for (i = 0; i < seri_devs; i++) {
		
		seri_setup_cdev(seri_devices + i, i);
		status = request_irq(4, seri_interrupt, 0, "seri", seri_devices + i);

	}	

	// Initialize UART

	// Enable Reciever Data Available and Transmitter Holding Register Empty Interrupts
	ier |= UART_IER_RDI | UART_IER_THRI;
	outb(ier, UART_BASE + UART_IER);

	// Control Registers
	lcr |= UART_LCR_WLEN8 | // 8 Bit chars    bit 0,1 - 11
		   UART_LCR_STOP |  // 2 stop bits    bit 2   - 1
		   UART_LCR_PARITY | UART_LCR_EPAR;  // Even parity bit 5,4,3 - 011
		   
	lcr |= UART_LCR_DLAB; // Activate DLAB to set bps
	outb(lcr, UART_BASE + UART_LCR);

	msb = (UART_DIV_1200 >> 4) ; // MSB
	lsb = UART_DIV_1200 & 0x0f; // LSB

	outb(msb, UART_BASE + UART_DLM);
	outb(lsb, UART_BASE + UART_DLL);

	lcr &= ~UART_LCR_DLAB; // Deactivate DLAB
	outb(lcr, UART_BASE + UART_LCR);

	// Send char
	while (!inb(UART_BASE + UART_LSR_THRE)) { // Check if THRE is empty and ready to receive a byte
		schedule(); // If not this function allows for the SO to do something else (acho eu xD)
	}

	outb('a', UART_BASE + UART_TX);

	return 0; // Success

  fail_malloc:
	unregister_chrdev_region(dev, seri_devs);
	return status;
}

static void seri_exit(void)
{
	int i;

	for (i = 0; i < seri_devs; i++) {

		cdev_del(&seri_devices[i].cdev);
		free_irq(4, seri_devices + i);

	}
	kfree(seri_devices);

	unregister_chrdev_region(MKDEV (seri_major, 0), seri_devs);
	
}

module_init(seri_init);
module_exit(seri_exit);