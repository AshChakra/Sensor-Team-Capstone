#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define	CONSUMER	"MyProgram"

#define LSM6DSOX_ADDR 0x6A // SPI address of LSM6DSOX
#define CTRL1_XL 0x10 // Accelerometer control register
#define CTRL2_G 0x11 // Gyroscope control register
#define OUTX_L_G 0x22 // Gyroscope output registers
#define OUTX_L_XL 0x28 // Accelerometer output registers

static const char *device = "/dev/spidev0.0";
static uint8_t mode = 3;
static uint8_t bits = 8;
static uint32_t speed = 500000;

static void pabort(const char *s)
{
	perror(s);
	abort();
}

int transfer(int fd, uint8_t tx[], uint8_t rx[], size_t len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = speed,
        .bits_per_word = bits,
    };
    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

int main(int argc, char *argv[]) {
    /*
    //########## CHIP SELECT STUFF ##########
    
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    int ret;

    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip)
        return -1;

    line = gpiod_chip_get_line(chip, 25); // Replace with your GPIO number
    if (!line) {
        gpiod_chip_close(chip);
        return -1;
    }

    ret = gpiod_line_request_output(line, CONSUMER, 0); // Initialize to LOW
    if (ret < 0) {
        gpiod_chip_close(chip);
        return -1;
    }

    gpiod_line_set_value(line, 1); // Set the line HIGH

    //######## CHIP SELECT STUFF END ########
    */
    int fd;
    uint8_t tx[2] = {0}, rx[2] = {0};

    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1)
        pabort("can't set spi mode");

    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1)
        pabort("can't set bits per word");

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1)
        pabort("can't set max speed hz");

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1)
        pabort("can't get max speed hz");

    printf("\nspi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    // Set accelerometer register
    tx[0] = CTRL1_XL | 0x00; // 0x00 for write
    tx[1] = 0x60; // Set accelerometer to 104 Hz, 4 g
    transfer(fd, tx, rx, ARRAY_SIZE(tx));

    // Set gyroscope register
    tx[0] = CTRL2_G | 0x00; // 0x00 for write
    tx[1] = 0x60; // Set gyroscope to 104 Hz, 2000 dps
    transfer(fd, tx, rx, ARRAY_SIZE(tx));

    // Read accelerometer
    tx[0] = OUTX_L_XL | 0x80; // 0x80 for read
    transfer(fd, tx, rx, ARRAY_SIZE(tx));
    printf("Accelerometer X Low: %02X\n", rx[1]);

    // Read gyroscope
    tx[0] = OUTX_L_G | 0x80; // 0x80 for read
    transfer(fd, tx, rx, ARRAY_SIZE(tx));
    printf("Gyroscope X Low: %02X\n", rx[1]);

    close(fd);
    //gpiod_line_release(line);
    //gpiod_chip_close(chip);
    return 0;
}
