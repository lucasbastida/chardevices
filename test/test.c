#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define BUF_MAX 256

int main()
{

	char *test = "My password is 1234";

	
        printf("-------------Starting device test---------\n");
	printf("-----------------encryption---------------\n");

        int fd = open("/dev/encryptdev", O_RDWR);
        if (fd < 0)
        {
                perror("Failed to open the device...");
                return errno;
        }

        char temp[BUF_MAX + 1];

        printf("writing the following message:\n%s\n", test);

        int n_rw;
        n_rw = write(fd, test, strlen(test));
        if (n_rw < 0)
        {
                perror("Failed to write the message to the device.");
                return errno;
        }
        printf("wrote %d bytes\n", n_rw);

        n_rw = read(fd, temp, n_rw);
        if (n_rw < 0)
        {
                perror("Failed to read the message from the device.");
                return errno;
        }
        printf("read %d bytes\n", n_rw);

        printf("Reading encrypted message from the device:\n%s\n", temp);

        close(fd);
	printf("------------------------------------------\n");
	printf("-----------------decryption---------------\n");

        int fd1 = open("/dev/decryptdev", O_RDWR);
        if (fd < 0)
        {
                perror("Failed to open the device...");
                return errno;
        }

        printf("Writing encrypted message to the device:\n%s\n", temp);
        n_rw = write(fd, temp, strlen(temp));
        if (n_rw < 0)
        {
                perror("Failed to write the message to the device.");
                return errno;
        }
        printf("wrote %d bytes\n", n_rw);

        n_rw = read(fd, temp, n_rw);
        if (n_rw < 0)
        {
                perror("Failed to read the message from the device.");
                return errno;
        }
        printf("read %d bytes\n", n_rw);

        printf("Reading decrypted message from the device:\n%s\n", temp);
	printf("------------------------------------------\n");

        return 0;
}
