#ifndef DECRYPT_DEV_H
#define DECRYPT_DEV_H

#define MAJOR_NUM 433
#define DEVICE_FILE_NAME "decryptdev"
#define DEVICE_NAME "decryptdev"
#define CLASS_NAME "decryptdev"

#define SUCCESS 0
#define BUF_LEN 256

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#endif
