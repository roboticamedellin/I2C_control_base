#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>  // for int16_t
#include <stdlib.h>  // for atoi

extern "C" {
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

#define I2C_BUS "/dev/i2c-1"   // Typically, it's i2c-1 for newer RPi models
#define I2C_ADDR 0x11

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <value1> <value2>\n", argv[0]);
        return 1;
    }

    int file;
    const char *filename = I2C_BUS;
    int16_t value1 = atoi(argv[1]);
    int16_t value2 = atoi(argv[2]);
    unsigned char data[4];

    // Pack the integers into a 4-byte array
    data[0] = (value1 >> 8) & 0xFF; // High byte of value1
    data[1] = value1 & 0xFF;        // Low byte of value1
    data[2] = (value2 >> 8) & 0xFF; // High byte of value2
    data[3] = value2 & 0xFF;        // Low byte of value2

    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        return 1;
    }

    // Send the data
    if (i2c_smbus_write_i2c_block_data(file, 0x00, 4, data) < 0) {
        perror("Failed to write to the i2c bus.");
        return 1;
    }

    close(file);
    return 0;
}
