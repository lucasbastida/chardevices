# chardevices
Simple character devices that encrypt and decrypt a message written to them.

## How the encryption works

It's also called a ceaser cypher or encryption shift. In this case,writing to the device will encrypt a string, Taking each character and shifting its value by 1 (adding 1 to each char). Reading from the device will return the encrypted string.

Similarly, to decrypt, it will take a string and shift it in reverse by 1 (subtracting 1 to a char). Reading from the device will return the decrypted string.


# Installation

Note that you are required to have the corresponding linux headers installed for kernel development.

Type the following inside the terminal:

```
cd ./encrypter
make
sudo insmod encryptdev.ko
cd ../decrypter
make
sudo insmod decryptdev.ko
```

# How to use

You can read/write to the device files as long as you have the corresponding root permission like the following example:

```
echo "My password is 1234" > /dev/encryptdev
cat /dev/encryptdev > /dev/decryptdev
cat /dev/decryptdev
```

There is also a test program inside the test folder that encrypts and decrypts the following string: “My password is 1234"


## Modifying the device file permissions

Since you generally need admin permissions to use these files you can change it the following way:

```
chmod 0666 /dev/encryptdev
chmod 0666 /dev/decryptdev
```

# References 
- Linux Device Drivers, Third Edition - ​ https://static.lwn.net/images/pdf/LDD3/ch03.pdf
- Linux Kernel Module Programming Guide - ​ http://tldp.org/LDP/lkmpg/2.6/html/index.html


This was built on a system with the following kernel version: 4.15.0-70-generic
