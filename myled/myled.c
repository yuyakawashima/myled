// SPDX-License-Identifier: GPL-3.0
/*
	Copyright (C) 2020  Ryuichi Ueda. All right reserved.
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>

MODULE_AUTHOR("Yuya Kawashima and Ryuichi Ueda");
MODULE_DESCRIPTION("task1 about driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
       	char c;
	if(copy_from_user(&c, buf, sizeof(char)))
		return -EFAULT;

//      printk(KERN_INFO "receive %c\n", c);

	if(c == '0'){
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 25;
	}//red
	else if(c == '1'){
		gpio_base[10] = 1 << 23;
		gpio_base[7] = 1 << 25;
	}//blue
	else if(c == '2'){
		gpio_base[7] = 1 << 23;
		gpio_base[10] = 1 << 25;
	}
	else if(c == '3'){
		int i = 0, j, k;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 25;
		while(i <= 2){
			for(j = 0; j <= 1; j++){
				for(k = 0; k <= 1; k++){
					gpio_base[7] = 1 << 25;
					mdelay(300);
					gpio_base[10] = 1 << 25;
					mdelay(100);
				}
				gpio_base[7] = 1 << 23;
				mdelay(450);
				gpio_base[10] = 1 << 23;
				mdelay(200);
			}
			for(j = 0;j <= 5; j++){
				gpio_base[7] = 1 << 25;
				mdelay(300);
				gpio_base[10] = 1 << 25;
				mdelay(80);
			}
			gpio_base[7] = 1 << 23;
			mdelay(450);
			gpio_base[10] = 1 << 23;
			mdelay(200);
			i++;
		}
	}
	else if(c == '4'){
		int i, j;
		gpio_base[10] = 1 << 23;
		gpio_base[10] = 1 << 25;
		for(i = 0; i <= 9; i++){
			for(j = 0; j <= 1; j++){
				gpio_base[7] = 1 << 25;
				mdelay(300);
				gpio_base[10] = 1 << 25;
				mdelay(100);
			}
			gpio_base[7] = 1 << 23;
			mdelay(450);
			gpio_base[10] = 1 << 23;
			mdelay(200);
		}
	}
	return 1;
}

static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {'s','u','s','h','i',0x0A};
	if(copy_to_user(buf + size,(const char *)sushi, sizeof(sushi))){
		printk(KERN_INFO "sushi : copy_to_user failed.\n");
		return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.read = sushi_read
};

static int __init init_mod(void)
{
	int retval;
	retval = alloc_chrdev_region(&dev, 0, 1, "myled");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
		return retval;
	}
	printk(KERN_INFO "%s is loaded.  major:%d\n", __FILE__, MAJOR(dev));

	cdev_init(&cdv, &led_fops);
	retval = cdev_add(&cdv, dev,1);
	if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d,\n", MAJOR(dev), MINOR(dev));
		return retval;
	}
	cls = class_create(THIS_MODULE, "myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));

	gpio_base = ioremap_nocache(0xfe200000, 0xA0);

	const u32 red_led = 25;
	const u32 red_index = red_led / 10;
	const u32 red_shift = (red_led%10) * 3;
	const u32 red_mask = ~(0x7 << red_shift);
	gpio_base[red_index] = (gpio_base[red_index] & red_mask) | (0x1 << red_shift);

	const u32 blue_led = 23;
	const u32 blue_index = blue_led / 10; 
	const u32 blue_shift = (blue_led%10) * 3;
	const u32 blue_mask = ~(0x7 << blue_shift);
	gpio_base[blue_index] = (gpio_base[blue_index] & blue_mask) | (0x1 << blue_shift);
	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d\n", __FILE__, MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
