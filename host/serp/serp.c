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

MODULE_LICENSE("Dual BSD/GPL");

static dev_t dev;

static int serp_init(void)
{
	int status;

	status = alloc_chrdev_region(&dev, 0, 1, "serp");
	if (status == 0) {
		printk(KERN_ALERT "Hello, serp\n");
		printk(KERN_ALERT "Major: %d\n", MAJOR(dev));
	}
	else
		printk(KERN_ALERT "Error allocating chardev region: %d\n", status);
	
	return 0;
}

static void serp_exit(void)
{
	unregister_chrdev_region(dev, 1);
	printk(KERN_ALERT "Goodbye, serp\n");
	return;
}

module_init(serp_init);
module_exit(serp_exit);
