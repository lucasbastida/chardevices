#ifndef ENCRYPT_DEV_H
#define ENCRYPT_DEV_H

#define MAJOR_NUM 432
#define DEVICE_FILE_NAME "encryptdev"
#define DEVICE_NAME "encryptdev"
#define CLASS_NAME "encryptdev"

#define SUCCESS 0
#define BUF_LEN 256

static int __init encrypt_init(void);
static void __exit encrypt_exit(void);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#endif
