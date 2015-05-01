#include <linux/module.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/kernel.h>
#include <linux/bootmem.h>
#include <linux/platform_device.h>

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/i2c/ltc3589.h>
#include <linux/math64.h>


#define SYSFS_PERMISSIONS         0644 /* default permissions for sysfs files */
#define SYSFS_READONLY            0444

// #define PAGE_SIZE 4096 // defined in asm/page.h
static ssize_t get_hello(struct device *dev, struct device_attribute *attr, char *buf);

static int __init elphelmem_init(void)
{
    struct device_node *node;
	const __be32 *bufsize_be;
	u32 bufsize;
	static void *bufaddr;

	printk("======== Allocating memory buffer ========\n");

	node = of_find_node_by_name(NULL, "elphel393-mem");
	if (!node)
	{
		printk("No node found ((((");
		return -ENODEV;
	}

	bufsize_be = (__be32 *)of_get_property(node, "memsize", NULL);
	bufsize = be32_to_cpup(bufsize_be);
    printk("Requested size = %u pages\n", bufsize);

    bufaddr = alloc_bootmem_low_pages(bufsize);

    if(bufaddr)
    {
    	printk("Allocated successfully at address %x\n", (u32)bufaddr);
    }

    return 0;
}

static void __exit elphelmem_exit(void)
{
    printk("Goodbye Cruel World!\n");
}



// SYSFS

static ssize_t get_hello(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf,"Hello again.\n");
}

static DEVICE_ATTR(hello_device,  SYSFS_PERMISSIONS & SYSFS_READONLY,    get_hello,          NULL);

static struct attribute *root_dev_attrs[] = {
		&dev_attr_hello_device.attr,
	    NULL
};

static const struct attribute_group dev_attr_root_group = {
	.attrs = root_dev_attrs,
	.name  = NULL,
};

static int elphel393_mem_sysfs_register(struct platform_device *pdev)
{
	int retval=0;
	struct device *dev = &pdev->dev;
	if (&dev->kobj) {
		if (((retval = sysfs_create_group(&dev->kobj, &dev_attr_root_group)))<0) return retval;
	}
	return retval;
}

static int elphel393_mem_probe(struct platform_device *pdev)
{
	// TODO: clientdata
	elphel393_mem_sysfs_register(pdev);
	dev_info(&pdev->dev,"Probing elphel393-pwr\n");
	return 0;
}

static int elphel393_mem_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev,"Removing elphel393-pwr");
	return 0;
}

static struct of_device_id elphel393_mem_of_match[] = {
	{ .compatible = "elphel,elphel393-mem-1.00", },
	{ /* end of table */}
};
MODULE_DEVICE_TABLE(of, elphel393_pwr_of_match);

static struct platform_driver elphel393_pwr = {
	.probe   = elphel393_mem_probe,
	.remove  = elphel393_mem_remove,
	.driver  = {
		.name  = "elphel393-mem",
		.owner = THIS_MODULE,
		.of_match_table = elphel393_mem_of_match,
		.pm = NULL, /* power management */
	},
};

module_platform_driver(elphel393_pwr);


module_init(elphelmem_init);
module_exit(elphelmem_exit);
MODULE_LICENSE("GPL");
