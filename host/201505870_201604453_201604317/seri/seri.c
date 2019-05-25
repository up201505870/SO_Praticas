/*                                                     
 * $Id: seri.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 * Code for Serial Interrupt device driver
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
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kfifo.h>
#include <linux/spinlock.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include "seri.h"
#include "serial_reg.h"

MODULE_LICENSE("Dual BSD/GPL");

int seri_open(struct inode *inode, struct file *filp);
int seri_release(struct inode *inode, struct file *filp);
ssize_t seri_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);
ssize_t seri_read(struct file *filp, char __user *buff, size_t count, loff_t *offp);
int seri_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static unsigned int seri_poll(struct file *filp, poll_table *wait);

irqreturn_t seri_interrupt(int irq, void *dev_id);

struct seri_dev {
	struct cdev cdev;
	int n_users; // Number of users.
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
	.ioctl		= seri_ioctl,
	.poll 		= seri_poll,
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

int seri_open(struct inode *inode, struct file *filp) {

	struct seri_dev *dev;

	dev = container_of(inode->i_cdev, struct seri_dev, cdev);

	if (dev->n_users > 0) { // ENHANCEMENT: 3.6 Multiple users
		return -EBUSY;
	}

	dev->n_users++;

	filp->private_data = dev;

	nonseekable_open(inode, filp);

	return 0;

}

int seri_release(struct inode *inode, struct file *filp) {

	struct seri_dev *dev;

	dev = container_of(inode->i_cdev, struct seri_dev, cdev);

	dev->n_users--;

	return 0;

}

int seri_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg) { // ENHANCEMENT: 3.4 Add ioctl operation

	unsigned int config;
	unsigned char lcr = 0;
	unsigned char lsb, msb;
	unsigned char parity, stop_bits, info_bits; 

	// Check for wrong cmds
	if (_IOC_TYPE(cmd) != SERI_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SERI_IOC_MAXNR) return -ENOTTY;

	switch(cmd) {

		case GET_SERI:

			config = 0x000000;

			// Get LCR to check parity, stop bits and number of information bits
            lcr = inb(UART_BASE + UART_LCR);

			// Get DLL and DLM
            lcr |= UART_LCR_DLAB; 
            outb(lcr, UART_BASE + UART_LCR); // Set DLAB to 1, to get dll and dlm
            lsb = inb(UART_BASE + UART_DLL);
            msb = inb(UART_BASE + UART_DLM);
            lcr &= ~UART_LCR_DLAB;
            outb(lcr, UART_BASE + UART_LCR); // Reset DLAB to 0.

			// Return consists of 24 bits, 8 for LCR, 8 for DLL and 8 for DLM
            config = lcr;
            config |= lsb<<8;
            config |= msb<<16;

            return config;

			break;

		case SET_SERI:

			config = (int)arg;
            msb = (unsigned char) ((config & 0x00FF0000)>>16);
            lsb = (unsigned char) ((config & 0x0000FF00)>>8);
            parity = (unsigned char) (config & 0x00000070);
            stop_bits = (unsigned char) (config & 0x0000000C);
            info_bits = (unsigned char) (config & 0x00000003);
            lcr = info_bits | parity | stop_bits | UART_LCR_DLAB; // Setup LCR register and Set DLAB to 1
            outb(lcr, UART_BASE + UART_LCR); 
			// Set DLL and DLM
            outb(lsb, UART_BASE + UART_DLL);
            outb(msb, UART_BASE + UART_DLM);
            lcr &= ~UART_LCR_DLAB;//zero dlab
            outb(lcr, UART_BASE + UART_LCR);//zero dlab

            return 0;

            break;

		default: // Redundant because we checked for false cmds before
			return -ENOTTY;
			break;
	}

	return -ENOTTY;
}

static unsigned int seri_poll(struct file *filp, poll_table *wait) { // ENHANCEMENT: 3.8 select/poll operations

	struct seri_dev *dev = filp->private_data;
	unsigned int mask = 0;

	down(&dev->mutex);

	poll_wait(filp, &dev->rxwq, wait); // Receiver FIFO
	poll_wait(filp, &dev->txwq, wait); // Trasnmitter FIFO
	if (kfifo_len(dev->rxfifo) > 0) { // Readable
		mask |= POLLIN | POLLRDNORM;
	}
	if (kfifo_len(dev->txfifo) < FIFO_SIZE) {// Writable
		mask |= POLLIN | POLLWRNORM;
	}
	up(&dev->mutex);

	return mask;
}

ssize_t seri_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {

	int status;
	char *buffer;
	struct seri_dev *dev = filp->private_data;

	buffer = kmalloc(sizeof(char) * count, GFP_KERNEL);
	memset(buffer, 0, sizeof(char) * count);

	down_interruptible(&dev->mutex); // ENHANCEMENT: 3.2 race conditions

	status = 1;

	while( kfifo_len(dev->rxfifo) != count ) {

		if (filp->f_flags & O_NONBLOCK) { // ENHANCEMENT: 3.3 honor the O_NONBLOCK flag
			up(&dev->mutex);
			return -EAGAIN;
		}

		printk(KERN_ALERT "Waiting to receive data.\n");

		status = wait_event_interruptible(dev->rxwq, kfifo_len(dev->rxfifo) == count); // ENHANCEMENT: 3.5 Possible to Ctrl-C out of reading.

		if(status == -ERESTARTSYS) {
			printk("Interrupted read.\n");
			up(&dev->mutex); // Clear Mutex
			return -EAGAIN;
		}

	}

	up(&dev->mutex);

	status = kfifo_get(dev->rxfifo, buffer, (count > FIFO_SIZE) ? FIFO_SIZE : count);

	status = copy_to_user(buff, buffer, status * sizeof(char));
	if (status != 0) {
		printk(KERN_ALERT "Couldn't copy all bytes.\n");
		return -1; // Failed to copy, message stored in buffer
	}

	kfree(buffer);

	return 0;

}

ssize_t seri_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {

	struct seri_dev *dev = filp->private_data;
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

	sema_init(&dev->mutex, 1);

	dev->n_users = 0;

	dev->rxfifo = kfifo_alloc(FIFO_SIZE, GFP_KERNEL, dev->rxfifo_lock);
	dev->txfifo = kfifo_alloc(FIFO_SIZE, GFP_KERNEL, dev->txfifo_lock);

	init_waitqueue_head(&dev->rxwq);
	init_waitqueue_head(&dev->txwq);

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

	if(request_region(UART_BASE, 1,"seri") == NULL) 
		return -1;

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

	release_region(UART_BASE, 1);

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
