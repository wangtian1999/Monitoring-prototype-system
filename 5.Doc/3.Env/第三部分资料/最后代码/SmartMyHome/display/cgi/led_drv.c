#include <linux/init.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/device.h>



#define GPX2_CON 0x11000C40  //LED2
#define GPX1_CON 0x11000C20 // LED3
#define GPF3_CON 0x114001E0 //LED4  and LED5
#define GP_size 8



struct led_desc  
{
	unsigned int dev_major;
	struct class *cls;
	struct device *dev[4];
	void *reg_virt_base[4];
};
struct led_desc *led_dev;

static int kernel_val = 555;

ssize_t led_drv_read (struct file *file, char __user *buf, size_t count, loff_t *fpos)
{
	int ret;
	printk("-----%s-----\n",__FUNCTION__);
	ret = copy_to_user(buf,&kernel_val,count);
	if(ret > 0)
	{
		printk("copy_to_user is failed!\n");
		return -EFAULT;
		}
	return 0;
}
ssize_t led_drv_write (struct file *file, const char __user *buf, size_t count, loff_t *fpos)
{
	int ret;
	int value;
	printk("--------%s---------\n",__FUNCTION__);
	ret = copy_from_user(&value,buf,count);
	if(ret > 0)
	{
		printk("copy_from_user error!\n");
		return -EFAULT;
	}
	
	printk("-----KERN----:value = %d\n",value);	
	// @value    0/1---LED2    2/3----LED3    4/5---LED4   6/7----LED5  
	switch(value){
		case 0:
			writel(readl(led_dev->reg_virt_base[0] + 4) & ~(1<<7),led_dev->reg_virt_base[0] +4);
			break;
		case 1:
			writel(readl(led_dev->reg_virt_base[0] + 4) | (1<<7),led_dev->reg_virt_base[0] +4);
			break;

		case 2:
			writel(readl(led_dev->reg_virt_base[1] + 4) & ~(1),led_dev->reg_virt_base[1] +4);
			break;
		case 3:
			writel(readl(led_dev->reg_virt_base[1] + 4) | (1),led_dev->reg_virt_base[1] +4);
			break;

		case 4:
			writel(readl(led_dev->reg_virt_base[2] + 4) & ~(1<<4),led_dev->reg_virt_base[2] +4);
			break;
		case 5:
			writel(readl(led_dev->reg_virt_base[2] + 4) | (1<<4),led_dev->reg_virt_base[2] +4);
			break;

		case 6:
			writel(readl(led_dev->reg_virt_base[2] + 4) & ~(1<<5),led_dev->reg_virt_base[2] +4);
			break;
		case 7:
			writel(readl(led_dev->reg_virt_base[2] + 4) | (1<<5),led_dev->reg_virt_base[2] +4);
			break;
		 default :
		 	printk("cmb error!\n");
			break;
		}
	return 0;
}
int led_drv_open (struct inode *inode, struct file *file)
{
	printk("------------%s---------",__FUNCTION__);
	return 0;
}
int led_drv_close (struct inode *inode, struct file *file)
{
	printk("---------%s--------",__FUNCTION__);
	return 0;
}

struct file_operations my_fops = {
	.open = led_drv_open,
	.release = led_drv_close,
	.write = led_drv_write,
	.read = led_drv_read,
};

static int __init led_dev_init(void)
{
	int ret;
	led_dev = kmalloc(sizeof(struct led_desc),GFP_KERNEL);
	if(led_dev == NULL)
	{
		printk(KERN_ERR "malloc error!\n");
		return -ENOMEM;
	}
	led_dev->dev_major = register_chrdev(0 ,"LED_dev_test",&my_fops);
	if(led_dev->dev_major > 0)
	{	
		printk("register ok!\n");
	}else{
		printk(KERN_ERR"register failed!\n");
		ret = -ENODEV;
		goto err_0;
	}
	led_dev->cls = class_create(THIS_MODULE,"LED_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk(KERN_ERR "class_create error!\n");
		ret = PTR_ERR(led_dev->cls);
		goto err_1;
	}
	
	//创建对应设备
	led_dev->dev[0] = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0),NULL,"LED%d",2);
	if(IS_ERR(led_dev->dev))
	{
		printk(KERN_ERR"device_create is fail!\n");
		goto err_2;
	}
	led_dev->dev[1] = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 1),NULL,"LED%d",3);
		if(IS_ERR(led_dev->dev))
		{
			printk(KERN_ERR"device_create is fail!\n");
			goto err_2;
		}
		led_dev->dev[2] = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 2),NULL,"LED%d",4);
	if(IS_ERR(led_dev->dev))
	{
		printk(KERN_ERR"device_create is fail!\n");
		goto err_2;
	}
	led_dev->dev[3] = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 3),NULL,"LED%d",5);
	if(IS_ERR(led_dev->dev))
	{
		printk(KERN_ERR"device_create is fail!\n");
		goto err_2;
	}
		

	//地址映射
	led_dev->reg_virt_base[0] = ioremap(GPX2_CON,GP_size);
	if(IS_ERR(led_dev->reg_virt_base))
	{
		printk(KERN_ERR "ioremap---0-- error!\n");
		ret = -ENOMEM;
		goto err_3;
	}
	led_dev->reg_virt_base[1] = ioremap(GPX1_CON,GP_size);
	if(IS_ERR(led_dev->reg_virt_base))
	{
		printk(KERN_ERR "ioremap---1--- error!\n");
		ret = -ENOMEM;
		goto err_3;
	}
	led_dev->reg_virt_base[2] = ioremap(GPF3_CON,GP_size);
	if(IS_ERR(led_dev->reg_virt_base))
	{
		printk(KERN_ERR "ioremap--2-- error!\n");
		ret = -ENOMEM;
		goto err_3;
	}

	//配置GPIO输出功能
	u32 value0 = readl(led_dev->reg_virt_base[0]);
	value0 &= ~(0xf << 28);
	value0 |= (0x1 << 28);
	writel(value0,led_dev->reg_virt_base[0]);

	u32 value1 = readl(led_dev->reg_virt_base[1]);
	value1 &= ~(0xf);
	value1 |= (0x1);
	writel(value1,led_dev->reg_virt_base[1]);

	u32 value2 = readl(led_dev->reg_virt_base[2]);
	value2 &= ~(0xff << 16);
	value2 |= (0x11 << 16);
	writel(value2,led_dev->reg_virt_base[2]);
		
	return 0;

err_3:
	device_destroy(led_dev->cls,MKDEV(led_dev->dev_major,0));
err_2:
	class_destroy(led_dev->cls);
err_1:
	unregister_chrdev(led_dev->dev_major,"LED_dev_test");
err_0:
	kfree(led_dev);
	return ret;
	
}
static void __exit led_dev_exit(void)
{
	int i = 0; 
	for(; i<3; i++)
	{
		iounmap(led_dev->reg_virt_base[i]);
		device_destroy(led_dev->cls,MKDEV(led_dev->dev_major,i));

	}
	class_destroy(led_dev->cls);
	unregister_chrdev(led_dev->dev_major,"LED_dev_test");
	kfree(led_dev);
}
module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");


