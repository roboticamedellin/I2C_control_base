#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <chrono>
#include <thread>

#define I2C_BUS "/dev/i2c-1"
#define I2C_ADDR 0x11

int main() {
    int file;
    const char *filename = I2C_BUS;

    // Open I2C bus
    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the I2C bus");
        return 1;
    }

    // Connect to I2C slave
    if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("Failed to connect to the I2C slave");
        close(file);
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        uint8_t buffer[4];

        // Request 4 bytes from the ESP32
        ssize_t bytesRead = read(file, buffer, 4);
        if (bytesRead != 4) {
            perror("Failed to read from the I2C bus");
        } else {
            // Decode the 2 int16_t values (big endian)
            int16_t rpmA = (buffer[0] << 8) | buffer[1];
            int16_t rpmB = (buffer[2] << 8) | buffer[3];

            std::cout << "Read " << i+1 << ": "
                      << "RPM A = " << rpmA / 100.0
                      << ", RPM B = " << rpmB / 100.0 << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(file);
    return 0;
}
