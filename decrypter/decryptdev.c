#include <linux/init.h>
#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/fs.h>
#include <linux/uaccess.h> /* for get_user and put_user */
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME "decryptdev"

#define ENCRYPTION_SHIFT 1

#define SUCCESS 0
#define BUF_LEN 256

static dev_t first;
static struct cdev c_dev;
static struct class *encryptClass = NULL;
static struct device *encryptDevice = NULL;

static int Device_Open = 0;   //prevent concurrent access
static char Message[BUF_LEN]; //message device gives when asked
static char *Message_Ptr;	 //Useful if the message is larger than the size of the buffer we get to fill in device_read.

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

		if (Message[i] != 0) //if not NULL char
		{
			Message[i] = Message[i] - ENCRYPTION_SHIFT; //encrypt
		}
	}

	Message_Ptr = Message;
	return i;
}

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release};

static int __init encrypt_init(void)
{
	int ret_val;
	/* Dynamically choose a major number along with the first minor number in dev_t */
	ret_val = alloc_chrdev_region(&first, 0, 1, DEVICE_NAME); //register a range of char device numbers

	if (ret_val < 0)
	{
		printk(KERN_ALERT "%s failed with %d\n",
			   "Sorry, registering the character device ", ret_val);
		return ret_val;
	}

	encryptClass = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(encryptClass)) //if failed to create device class
	{
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT " Failed with creating class");
		return PTR_ERR(encryptClass);
	}

	encryptDevice = device_create(encryptClass, NULL, first, NULL, DEVICE_NAME);
	if (IS_ERR(encryptDevice)) //if failed to create device
	{
		class_destroy(encryptClass);
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT " Failed with creating device");
		return PTR_ERR(encryptDevice);
	}

	cdev_init(&c_dev, &fops);
	ret_val = cdev_add(&c_dev, first, 1);
	if (ret_val < 0)
	{
		device_destroy(encryptClass, first);
		class_destroy(encryptClass);
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT " Failed with registering character device");
		return ret_val;
	}

	printk(KERN_INFO "%s The major device number is %d.\n",
		   "Registeration is a success", MAJOR(first));

	return 0;
}

static void __exit encrypt_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(encryptClass, first);
	class_unregister(encryptClass);
	class_destroy(encryptClass);
	unregister_chrdev_region(first, 1);
}

module_init(encrypt_init);
module_exit(encrypt_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Decryption Character device driver");
