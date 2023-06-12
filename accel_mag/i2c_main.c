#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>

#define LSM6DSOX_ADDR 0x6A // I2C address of LSM6DSOX
#define CTRL1_XL 0x10 // Accelerometer control register
#define CTRL2_G 0x11 // Gyroscope control register
#define OUTX_L_G 0x22 // Gyroscope output registers
#define OUTX_L_XL 0x28 // Accelerometer output registers
#define OUTX_H_A 0x29
uint8_t read_register_data(int file, uint8_t register_addr, uint8_t *buffer, uint8_t length) {
    write(file, &register_addr, 1);
    read(file, buffer, length);
    printf("%d\r\n",buffer);
}

int16_t convert_to_int16(uint8_t high_byte, uint8_t low_byte) {
    return (int16_t)((high_byte << 8) | low_byte);
}

int main() {
    int file;
    char *bus = "/dev/i2c-1"; // I2C bus
    uint8_t data[2] = {0};

    // Open I2C bus
    if ((file = open(bus, O_RDWR)) < 0) {
        printf("Failed to open the I2C bus.\n");
        return 1;
    }

    // Get I2C device
    ioctl(file, I2C_SLAVE, LSM6DSOX_ADDR);

    // Configure accelerometer (CTRL1_XL)
    uint8_t accel_config = 0x40; // ODR 104 Hz, FS 4g
    write(file, (uint8_t[]){CTRL1_XL, accel_config}, 2);

    // Configure gyroscope (CTRL2_G)
    uint8_t gyro_config = 0x40; // ODR 104 Hz, FS 245 dps
    write(file, (uint8_t[]){CTRL2_G, gyro_config}, 2);

    while(1){
        // Read accelerometer data

        read_register_data(file, OUTX_L_XL, data, 1);
        read_register_data(file, OUTX_H_A, data, 1);
        int16_t ax = convert_to_int16(data[1], data[0]);
        printf("Accelerometer: AX=%d\n", ax);

        sleep(1);        
    }


    return 0;
}
