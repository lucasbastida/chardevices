#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/fs.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/device.h>

#include "decryptdev.h"

static struct class *encdevClass = NULL;
static struct device *encdevDevice = NULL;

static int Device_Open = 0;   //prevent concurrent access
static char Message[BUF_LEN]; //message device gives when asked
/* 
 * How far did the process reading the message get?
 * Useful if the message is larger than the size of the
 * buffer we get to fill in device_read. 
 */
static char *Message_Ptr;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release};

int device_open(struct inode *inode, struct file *file)
{
	if (Device_Open)
		return -EBUSY;

	Device_Open++;

	/*
	 * Initialize the message 
	 */
	Message_Ptr = Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file *file,   /* see include/linux/fs.h   */
						   char __user *buffer, /* buffer to be filled with data */
						   size_t length,		/* length of the buffer     */
						   loff_t *offset)
{
	int bytes_read = 0;

	/* 
	 * If we're at the end of the message, return 0
	 * (which signifies end of file) 
	 */
	if (*Message_Ptr == 0)
		return 0;

	/* 
	 * Actually put the data into the buffer 
	 */
	while (length && *Message_Ptr)
	{

		/* 
		 * Because the buffer is in the user data segment,
		 * not the kernel data segment, assignment wouldn't
		 * work. Instead, we have to use put_user which
		 * copies data from the kernel data segment to the
		 * user data segment. 
		 */
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	//add NULL to end of user buffer
	put_user(0, buffer);

	/* 
	 * Read functions are supposed to return the number
	 * of bytes actually inserted into the buffer 
	 */
	return bytes_read;
}

static ssize_t device_write(struct file *file,
							const char __user *buffer,
							size_t length,
							loff_t *offset)
{
	int i;

	for (i = 0; i < length && i < BUF_LEN; i++)
	{
		get_user(Message[i], buffer + i); //place char into Message
		Message[i] = Message[i] - 1;	  //encrypt
	}

	Message_Ptr = Message;
	return i;
}

static int __init decrypt_init(void)
{
	int ret_val;
	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);

	if (ret_val < 0)
	{
		printk(KERN_ALERT "%s failed with %d\n",
			   "Sorry, registering the character device ", ret_val);
		return ret_val;
	}

	encdevClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(encdevClass))
	{
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_INFO "encdev: Failed to register device class.\n");
		return PTR_ERR(encdevClass);
	}
	printk(KERN_INFO "decryptdev Device class registered.\n");

	encdevDevice = device_create(encdevClass, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_NAME);
	if (IS_ERR(encdevDevice))
	{
		class_destroy(encdevClass);
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_INFO "decryptdev: Failed to create the device.\n");
		return PTR_ERR(encdevDevice);
	}
	printk(KERN_INFO "decryptdev: Device class created.\n");

	printk(KERN_INFO "%s The major device number is %d.\n",
		   "Registeration is a success", MAJOR_NUM);

	return 0;
}

static void __exit decrypt_exit(void)
{
	device_destroy(encdevClass, MKDEV(MAJOR_NUM, 0));
	class_unregister(encdevClass);
	class_destroy(encdevClass);
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init(decrypt_init);
module_exit(decrypt_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Decryption Character device");
