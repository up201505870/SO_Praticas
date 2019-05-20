/*                                                     
 * $Id: serp.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
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

#define UART_BASE 0x3f8

MODULE_LICENSE("Dual BSD/GPL");

static dev_t dev;
struct cdev *cdev;

static int serp_open(struct inode *inode, struct file *filp);
static int serp_release(struct inode *inode, struct file *filp);
ssize_t serp_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
ssize_t serp_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);

struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open  		= serp_open,
	.release 	= serp_release,
	.write		= serp_write,
	.read 		= serp_read,
	.llseek		= no_llseek,
};

static int serp_open(struct inode *inode, struct file *filp) {

	filp->private_data = &cdev;

	nonseekable_open(inode, filp);

	printk(KERN_ALERT "Open operation invoked.\n");

	return 0;

}

static int serp_release(struct inode *inode, struct file *filp) {

	printk(KERN_ALERT "Release operation invoked.\n");

	return 0;

}

ssize_t serp_read(struct file *filep, char __user *buff, size_t count, loff_t *offp) {

	int status;
	unsigned char rx;

	if(inb(UART_BASE + UART_LSR) & UART_LSR_OE) {

		return -EIO;

	} 
	
	if(!(inb(UART_BASE + UART_LSR) & UART_LSR_DR)) { // No char to be read

		return -EAGAIN;

	} else {

		rx = inb(UART_BASE + UART_RX);

		status = copy_to_user(buff, &rx, 1);
		if (status != 0)
			printk(KERN_ALERT "Couldn't copy all bytes.\n");

	}

	return 1;

}

ssize_t serp_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp) {

	char *buffer;
	unsigned long status;
	int i;

	printk(KERN_ALERT "Write operation invoked.\n");

	buffer = kmalloc(sizeof(char) * count, GFP_KERNEL);
	memset(buffer, 0, sizeof(char) * count);

	status = copy_from_user(buffer, buff, count);
	if (status != 0)
		printk(KERN_ALERT "Couldn't copy all bytes.\n");

	if (buffer[count - 1] != 0)
		buffer[count - 1] = 0;

	for(i = 0; i < count; i++) {

		while (inb(UART_BASE + UART_LSR_THRE)) { 
			schedule();
		}

		outb(buffer[i], UART_BASE + UART_TX);

	}

	kfree(buffer);
	return 0;

}

static int serp_init(void)
{
	int status;
	unsigned char lcr = 0; // Line Control
	unsigned char msb = 0, lsb = 0; // Msb and Lsb for DL

	status = alloc_chrdev_region(&dev, 0, 1, "serp");
	if (status == 0) {
		printk(KERN_ALERT "Hello, serp\n");
		printk(KERN_ALERT "Major: %d\n", MAJOR(dev));
	}
	else {
		unregister_chrdev_region(dev, 1);
		printk(KERN_ALERT "Error allocating chardev region: %d\n", status);
		return -1;
	}

	cdev = cdev_alloc();
	cdev->ops = &fops;
	cdev->owner = THIS_MODULE;

	status = cdev_add(cdev, dev, 1);
	if (status < 0) {
		printk(KERN_ALERT "Could not add cdev properly.\n");
	}

	// Disable Interrupts
	outb(0, UART_BASE + UART_IER);

	// Initialize UART
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
	while (inb(UART_LSR_THRE)) { // Check if THRE is empty and ready to receive a byte
		schedule(); // If not this function allows for the SO to do something else (acho eu xD)
	}

	outb('a', UART_BASE + UART_TX);

	return 0;
}

static void serp_exit(void)
{
	cdev_del(cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Goodbye, serp\n");
	return;
}

module_init(serp_init);
module_exit(serp_exit);
