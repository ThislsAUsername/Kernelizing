#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */

static int __init my_name(void)
{
	printk(KERN_INFO "Josiah White\n");
	return 0;
}

static void __exit my_exit(void)
{
}

module_init(my_name);
module_exit(my_exit);
