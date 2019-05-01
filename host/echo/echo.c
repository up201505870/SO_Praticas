/*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
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

MODULE_LICENSE("Dual BSD/GPL");

// Prototypes
static int echo_open(struct inode *inode, struct file *filp);
static int echo_release(struct inode *inode, struct file *filp);
ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp);
ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp);

static dev_t dev;
struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open  		= echo_open,
	.release 	= echo_release,
	.read 		= echo_read,
	.write		= echo_write,
	.llseek		= no_llseek,
};

struct echo_dev {
	struct cdev cdev;
	int cnt;
};

static struct echo_dev *echo_device;

static int echo_open(struct inode *inode, struct file *filp) {

	struct echo_dev *dev;

	dev = container_of(inode->i_cdev, struct echo_dev, cdev);

	filp->private_data = dev;
	printk(KERN_ALERT "Open operation invoked.\n");

	nonseekable_open(inode, filp);

	return 0;

}

static int echo_release(struct inode *inode, struct file *filp) {

	printk(KERN_ALERT "Release operation invoked.\n");

	return 0;

}

ssize_t echo_read(struct file *filep, char __user *buff, size_t count, loff_t *offp) {

	int status;

	printk(KERN_ALERT "Read operation invoked.\n");

	status = copy_to_user(buff, &echo_device->cnt, sizeof(&echo_device->cnt));
	if (status != 0)
		printk(KERN_ALERT "Couldn't copy all bytes.\n");

	return 0;

}

ssize_t echo_write(struct file *filep, const char __user *buff, size_t count, loff_t *offp) {

	char *buffer;
	unsigned long status;

	printk(KERN_ALERT "Write operation invoked.\n");

	buffer = kmalloc(sizeof(char) * count, GFP_KERNEL);
	memset(buffer, 0, sizeof(char) * count);

	status = copy_from_user(buffer, buff, count);
	if (status != 0)
		printk(KERN_ALERT "Couldn't copy all bytes.\n");

	if (buffer[count - 1] != 0)
		buffer[count - 1] = 0;

	echo_device->cnt += count;

	printk(KERN_ALERT "%s", buffer);

	kfree(buffer);
	return 0;

}

static int echo_init(void) {

	int status;

	status = alloc_chrdev_region(&dev, 0, 1, "echo");
	if (status == 0)
		printk(KERN_ALERT "Major: %d\n", MAJOR(dev));
	else
		printk(KERN_ALERT "Error allocating chardev region: %d\n", status);

	echo_device = (struct echo_dev *) kmalloc(sizeof(struct echo_dev), GFP_KERNEL);

	cdev_init(&echo_device->cdev, &fops);
	(&echo_device->cdev)->owner = THIS_MODULE;

	echo_device->cnt = 0;

	status = cdev_add(&echo_device->cdev, dev, 1);
	if (status < 0) {
		printk(KERN_ALERT "Could not add cdev properly.\n");
	}

	return 0;
}

static void echo_exit(void) {
	cdev_del(&echo_device->cdev);

	kfree(echo_device);

	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Freeing major: %d\n", MAJOR(dev));

	return;
}

module_init(echo_init);
module_exit(echo_exit);
