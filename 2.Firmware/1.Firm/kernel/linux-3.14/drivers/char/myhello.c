#include <linux/module.h>
#include <linux/kernel.h>

int __init myhello_init(void)
{
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");
	printk("##############################################\n");

	return 0;
}

void __exit myhello_exit(void)
{
	printk("myhello will exit\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hiro Wang");
MODULE_DESCRIPTION("It is only a simple test");
MODULE_ALIAS("HI");

module_init(myhello_init);
module_exit(myhello_exit);
