#ifndef ORIN_SPI_H_   /* Include guard */
#define ORIN_SPI_H_


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


// Const used
static const char *device = "/dev/spidev0.0";
static uint32_t mode;
static uint8_t bits = 8;
static char *input_file;
static char *output_file;
static uint32_t speed = 10000;
static uint16_t delay;
int fd;

// Function prototypes
uint8_t* send_spi_msg(int);
int setup();
int close_spi();
int switch_auto();

#endif
