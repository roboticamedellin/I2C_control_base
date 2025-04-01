#include <iostream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <cstdlib>

#define I2C_BUS "/dev/i2c-1"
#define I2C_ADDR 0x11

void send_stream_data(int16_t vel_l, int16_t vel_r, int duration_seconds) {
    int file;
    const char *filename = I2C_BUS;

    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return;
    }

    if (ioctl(file, I2C_SLAVE, I2C_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        close(file);
        return;
    }

    unsigned char payload[5];
    payload[0] = 0x55; // stream mode
    payload[1] = (vel_l >> 8) & 0xFF;
    payload[2] = vel_l & 0xFF;
    payload[3] = (vel_r >> 8) & 0xFF;
    payload[4] = vel_r & 0xFF;

    unsigned char buffer[6];
    buffer[0] = 0x00; // dummy register
    memcpy(&buffer[1], payload, 5);

    for (int i = 0; i < duration_seconds * 10; ++i) {
        if (write(file, buffer, 6) != 6) {
            perror("Failed to write to the i2c bus.");
        } else {
            std::cout << "Sent I2C stream [" << i + 1 << "/" << duration_seconds * 10 << "]\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    close(file);
    std::cout << "Finished streaming.\n";
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <left_speed> <right_speed> [duration_seconds]\n";
        return 1;
    }

    int16_t left_speed = static_cast<int16_t>(std::atoi(argv[1]));
    int16_t right_speed = static_cast<int16_t>(std::atoi(argv[2]));
    int duration_seconds = 5; // default duration

    if (argc == 4) {
        duration_seconds = std::atoi(argv[3]);
        if (duration_seconds <= 0) duration_seconds = 5;
    }

    std::thread sender(send_stream_data, left_speed, right_speed, duration_seconds);
    sender.join();

    return 0;
}
