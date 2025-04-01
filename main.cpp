#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" {
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

#define I2C_BUS "/dev/i2c-1"   // Change if using a different bus
#define I2C_ADDR 0x11

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <mode:44|55> <left_speed> <right_speed>\n", argv[0]);
        return 1;
    }

    int file;
    const char *filename = I2C_BUS;
    uint8_t mode = (uint8_t)strtol(argv[1], NULL, 16);  // e.g. 0x44 or 0x55
    int16_t left_speed = atoi(argv[2]);
    int16_t right_speed = atoi(argv[3]);

    unsigned char data[5 + 1]; // total 6 bytes

    data[0] = mode;
    data[1] = (left_speed >> 8) & 0xFF;
    data[2] = left_speed & 0xFF;
    data[3] = (right_speed >> 8) & 0xFF;
    data[4] = right_speed & 0xFF;

    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        close(file);
        return 1;
    }

    if (i2c_smbus_write_i2c_block_data(file, 0x00, 5, data) < 0) {
        perror("Failed to write to the i2c bus.");
        close(file);
        return 1;
    }

    close(file);
    return 0;
}
