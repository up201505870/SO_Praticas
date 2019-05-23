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
#include <linux/kfifo.h>
#include <linux/spinlock.h>

MODULE_LICENSE("Dual BSD/GPL");

#define SERI_MAJOR 0   /* dynamic major by default */
#define SERI_DEVS  4   /* seri0 through seri3 */

#define FIFO_SIZE 128

#define UART_BASE 0x3f8

static int seri_open(struct inode *inode, struct file *filp);
static int seri_release(struct inode *inode, struct file *filp);
ssize_t seri_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);
ssize_t seri_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);

irqreturn_t seri_interrupt(int irq, void *dev_id);

struct seri_dev {
	struct cdev cdev;
	unsigned char txflag; // Flag for possible trasmission
	struct semaphore mutex;
	struct kfifo *rxfifo; // receiver info
	struct kfifo *txfifo; // transmitter buffer
	spinlock_t *rxfifo_lock;
	spinlock_t *txfifo_lock;
	wait_queue_head_t rxwq; // for IH sync
	wait_queue_head_t txwq;
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

	unsigned char in = inb(UART_BASE + UART_IIR);
	unsigned char error;
	unsigned char read_byte = 0;
	unsigned char write_byte = 0;
	int ret = 0;
	int status;

	if (in == UART_IIR_NO_INT) { // No Interruption - Just in case - 1

	} else if (in == UART_IIR_RLSI) { // Line Status Interruption - 6

		error = inb(UART_BASE + UART_LSR);
		if(error & UART_LSR_OE) {
			printk(KERN_ALERT "-> Overrun Error\n");
		} else if(error & UART_LSR_PE) {
			printk(KERN_ALERT "-> Parity Error\n");
		} else if(error & UART_LSR_FE) {
			printk(KERN_ALERT "-> Framing Error\n");
		} else if(error & UART_LSR_BI) {
			printk(KERN_ALERT "-> Break Interrupt Error\n");
		}

	} else if (in == UART_IIR_RDI) { // Read Data Interruption - 4
		read_byte = inb(UART_BASE + UART_RX);

		ret = kfifo_put(((struct seri_dev *) dev_id)->rxfifo, &read_byte, 1);
		if (ret != 1) {
			printk(KERN_ALERT "Buffer full.\n");
		}

		wake_up_interruptible(&((struct seri_dev *) dev_id)->rxwq);

	} else if (in == UART_IIR_THRI) { // Transmitter Hold Register Interrupt - 2

		status = kfifo_get(((struct seri_dev *) dev_id)->txfifo, &write_byte, 1);

		if (status == 1) {
			outb(write_byte, UART_BASE + UART_TX);
			((struct seri_dev *) dev_id)->txflag = 0;
		} else {
			((struct seri_dev *) dev_id)->txflag = 1;
		}
	}

	return IRQ_HANDLED;

}

static int seri_open(struct inode *inode, struct file *filp) {

	struct seri_dev *dev;

	dev = container_of(inode->i_cdev, struct seri_dev, cdev);

	filp->private_data = dev;

	return 0;

}

static int seri_release(struct inode *inode, struct file *filp) {

	return 0;

}

ssize_t seri_read(struct file *filep, char __user *buff, size_t count, loff_t *offp) {

	int status;
	char *buffer;
	struct seri_dev *dev = filep->private_data;

	buffer = kmalloc(sizeof(char) * count, GFP_KERNEL);
	memset(buffer, 0, sizeof(char) * count);

	// Check concurrency here?

	while( !kfifo_len(dev->rxfifo) ) {
		printk(KERN_ALERT "Waiting...\n");

		wait_event_interruptible(dev->rxwq, kfifo_len(dev->rxfifo));

	}

	status = kfifo_get(dev->rxfifo, buffer, (count > FIFO_SIZE) ? FIFO_SIZE : count);

	printk(KERN_ALERT "Finished Waiting...\n");

	status = copy_to_user(buff, buffer, status * sizeof(char));
	if (status != 0) {
		printk(KERN_ALERT "Couldn't copy all bytes.\n");
		return -1; // Failed to copy, message stored in buffer
	}

	kfree(buffer);

	return 0;

}

ssize_t seri_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp) {

	struct seri_dev *dev = filep->private_data;
	char *buffer;
	unsigned char c;
	unsigned long status;

	if(count == 0) {
		return 0;
	}

	buffer = kmalloc(sizeof(char) * count, GFP_KERNEL);
	memset(buffer, 0, sizeof(char) * count);

	status = copy_from_user(buffer, buff, count);
	if (status != 0) {
		printk(KERN_ALERT "Couldn't copy all bytes.\n");
		return -1;

	}

	status = kfifo_put(dev->txfifo, buffer, count);
	if (status < count) {
		printk(KERN_ALERT "Buffer full.\n");
	}

	if (dev->txflag == 1) {
		kfifo_get(dev->txfifo, &c, 1);
		outb(c, UART_BASE + UART_TX);
	}
	
	kfree(buffer);

	return 0;

}

static void seri_setup_cdev(struct seri_dev *dev, int index)
{
	int err, devno = MKDEV(seri_major, index);
    
	spin_lock_init(dev->rxfifo_lock);
	spin_lock_init(dev->txfifo_lock);

	dev->rxfifo = kfifo_alloc(FIFO_SIZE, GFP_KERNEL, dev->rxfifo_lock);
	dev->txfifo = kfifo_alloc(FIFO_SIZE, GFP_KERNEL, dev->txfifo_lock);

	init_waitqueue_head(&dev->rxwq);
	init_waitqueue_head(&dev->txwq);

	// err = request_irq(4, seri_interrupt, IRQF_SHARED, "seri", dev);
	// if (err) {
	// 	printk(KERN_ALERT "Error with requesting IRQ %d.\n", err);
	// }

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

	}

	// Initialize UART

	// Enable Reciever Data Available and Transmitter Holding Register Empty Interrupts
	ier |= UART_IER_RLSI | UART_IER_THRI | UART_IER_RDI; //
	outb(ier, UART_BASE + UART_IER);

	// Control Registers
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
	if (seri_devices[0].txflag == 1) {
		kfifo_put(seri_devices[0].txfifo, "ello!", 6);
		outb('H', UART_BASE + UART_TX);

	} else {
		kfifo_put(seri_devices[0].txfifo, "Hello!", 6);
	}

	status = request_irq(4, seri_interrupt, 0, "seri", &seri_devices[0]);
	if (status) {
		printk(KERN_ALERT "Error with requesting IRQ %d.\n", status);
	}

	return 0; // Success

  fail_malloc:
	unregister_chrdev_region(dev, seri_devs);
	return status;
}

static void seri_exit(void)
{
	int i;

	for (i = 0; i < seri_devs; i++) {

		kfifo_free(seri_devices[i].rxfifo);
		kfifo_free(seri_devices[i].txfifo);

		cdev_del(&seri_devices[i].cdev);

	}
	free_irq(4, &seri_devices[0]);
	kfree(seri_devices);

	unregister_chrdev_region(MKDEV (seri_major, 0), seri_devs);
	
}

module_init(seri_init);
module_exit(seri_exit);
