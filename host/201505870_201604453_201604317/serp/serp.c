/*                                                     
 * $Id: serp.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 * Code for Serial Polling device driver
 * Code developed by:
 * Afonso Pereira
 * David Cunha
 * Joao Loureiro
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
#include<linux/ioport.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include "serial_reg.h"

MODULE_LICENSE("Dual BSD/GPL");

// Defines
#define SERP_MAJOR 0   /* dynamic major by default */
#define SERP_DEVS  4   /* seri0 through seri3 */

#define RX_BUFFERSIZE 128

#define UART_BASE 0x3f8

// Declarations
struct serp_dev {
	int rx_buffersize;
	char *rx_buffer;
	struct cdev cdev;
};

static int serp_open(struct inode *inode, struct file *filp);
static int serp_release(struct inode *inode, struct file *filp);
ssize_t serp_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
ssize_t serp_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
static int serp_setup_cdev(struct serp_dev *dev, int index);
static void w_char(const char c);

// Global Variables
int serp_major = SERP_MAJOR;
int serp_devs  = SERP_DEVS;

struct serp_dev *serp_devices;

struct file_operations serp_fops = {
	.owner 		= THIS_MODULE,
	.open  		= serp_open,
	.release 	= serp_release,
	.write		= serp_write,
	.read 		= serp_read,
	.llseek		= no_llseek,
};

static void w_char(const char c) {

	while (!(inb(UART_BASE + UART_LSR) & UART_LSR_THRE)) { // Check if THRE is empty and ready to receive a byte
		msleep_interruptible(10);
	}

	outb(c, UART_BASE + UART_TX);

}

static int serp_open(struct inode *inode, struct file *filp) {

	struct serp_dev *dev;

	dev = container_of(inode->i_cdev, struct serp_dev, cdev);

	nonseekable_open(inode, filp);

	filp->private_data = dev;

	return 0;

}

static int serp_release(struct inode *inode, struct file *filp) {

	return 0;

}

ssize_t serp_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {

	int status, i = 0;
	unsigned char rx = ' ';
	char *buffer;
	struct serp_dev *dev = filp->private_data;

	buffer = (char *) kzalloc(sizeof(char) * count + 1, GFP_KERNEL);

	// 13 - means (Enter)
	while((int)rx != 13 && (i < count)) { // Wait for enter to return
	
		if(inb(UART_BASE + UART_LSR) & UART_LSR_OE) {

			return -EIO;

		} 
		
		if(inb(UART_BASE + UART_LSR) & UART_LSR_DR) { 

			rx = inb(UART_BASE + UART_RX);

			buffer[i] = rx;
			i++;
		} else {
			schedule();
		}
	}

	if(buffer[i] != '\0') {
		buffer[i] = '\0';
		i++;
	}

	status = copy_to_user(buff, buffer, i * sizeof(char));
	if (status != 0) {
		printk(KERN_ALERT "Couldn't copy all bytes.\n");
		strncpy(dev->rx_buffer, buffer, i);

		return -1; // Failed to copy, message stored in buffer
	}

	kfree(buffer);

	return 0;

}

ssize_t serp_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {

	char *buffer;
	unsigned long status;
	int i;

	buffer = (char *) kzalloc(sizeof(char) * count, GFP_KERNEL);

	status = copy_from_user(buffer, buff, count);
	if (status != 0) {
		printk(KERN_ALERT "Couldn't copy all bytes.\n");
		return -1;

	}

	for(i = 0; i < count; i++) {

		w_char(buffer[i]);

	}

	kfree(buffer); 
	return 0;

}

static int serp_setup_cdev(struct serp_dev *dev, int index)
{
	int err, devno = MKDEV(serp_major, index);
    
	dev->rx_buffersize = RX_BUFFERSIZE;
	dev->rx_buffer = kmalloc(dev->rx_buffersize * sizeof(char), GFP_KERNEL);

	cdev_init(&dev->cdev, &serp_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &serp_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err) {
		printk(KERN_NOTICE "Error %d adding serp%d", err, index);
		return err;
	}

	return 0; // Success
}

static int serp_init(void)
{
	int status, i;
	dev_t dev = MKDEV(serp_major, 0);
	unsigned char lcr = 0; // Line Control
	unsigned char msb = 0, lsb = 0; // Msb and Lsb for DL
	char *c = "Hello!\n";

	if(request_region(UART_BASE, 1,"serp") == NULL) 
		return -1;

	if (serp_major) {
		status = register_chrdev_region(dev, serp_devs, "serp");
	} else {
		status = alloc_chrdev_region(&dev, 0, serp_devs, "serp");
		serp_major = MAJOR(dev);
	} // TODO: Error
	if (status < 0)
		return status;

	// Allocate the devices
	serp_devices = kmalloc(serp_devs * sizeof(struct serp_dev), GFP_KERNEL);
	if (!serp_devices) {
		status = -ENOMEM;
		goto fail_malloc;
	}
	memset(serp_devices, 0, serp_devs * sizeof(struct serp_dev)); // TODO: Error
	
	for (i = 0; i < serp_devs; i++) {
		
		status = serp_setup_cdev(serp_devices + i, i);
		if (status) {
			return status;
		}

	}	

	// UART CONFIGS

	// Disable Interrupts
	outb(0, UART_BASE + UART_IER);

	// Initialize UART
	lcr |= UART_LCR_WLEN8 | // 8 Bit chars    bit 0,1 - 11
		   UART_LCR_STOP |  // 2 stop bits    bit 2   - 1
		   UART_LCR_PARITY | UART_LCR_EPAR;  // Even parity bit 5,4,3 - 011
		   
	lcr |= UART_LCR_DLAB; // Activate DLAB to set bps
	outb(lcr, UART_BASE + UART_LCR);

	msb = (UART_DIV_1200 >> 8) ; // MSB
	lsb = UART_DIV_1200 & 0xff; // LSB

	outb(msb, UART_BASE + UART_DLM);
	outb(lsb, UART_BASE + UART_DLL);

	lcr &= ~UART_LCR_DLAB; // Deactivate DLAB
	outb(lcr, UART_BASE + UART_LCR);

	// Send char
	for (i = 0; i < 7; i++)
	{
		w_char(c[i]);
	}

	return 0;

  fail_malloc:
	unregister_chrdev_region(dev, serp_devs);
	return status;
}

static void serp_exit(void)
{
	int i;
	char *c = "Bye!\n";

	release_region(UART_BASE, 1);

	for (i = 0; i < serp_devs; i++) {

		cdev_del(&serp_devices[i].cdev);
	}

	kfree(serp_devices);

	unregister_chrdev_region(MKDEV (serp_major, 0), serp_devs);

	for (i = 0; i < 6; i++)
	{
		w_char(c[i]);
	}

}

module_init(serp_init);
module_exit(serp_exit);
