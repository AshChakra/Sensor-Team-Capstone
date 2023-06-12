/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

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
#include "orin_spi.h"

static void _pabort(const char *s)
{
	perror(s);
	abort();
}

uint8_t default_tx[2];

uint8_t default_rx[ARRAY_SIZE(default_tx)] = {0, };

void _int_to_uint8_array(int number, uint8_t *array, size_t size) {
    // Ensure the number is within the valid range for uint8
    if (number < 0 || number > 1024) {
        _pabort("Number must be between 0 and 1023 (inclusive).\n");
        return;
    }

    array[0] = 0x3; // Beginning uint for setup
    number <<= 2; // Shift 2 bits to left
    number |= 0x3000;

    array[0] = (number & 0xFF00) >> 8;
    array[1] = (number & 0x00FF);

	printf("%f\r\n", 5*number/4096);

    //printf("Resulting array: ");
    //for (size_t i = 0; i < size; ++i) {
    //    printf("%x ", array[i]);
    //}
    //printf("\n");

}

static void _transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
	int ret;
	int out_fd;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = len,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		_pabort("can't send spi message");
}

int close_spi(){
	int ret = 0;
    
    system("echo 0 > /sys/class/gpio/PBB.00/value");

    ret = close(fd);

    return ret;
}
int autoDac = 0;
int switch_auto(){

	if(autoDac == 0){
		autoDac = 1;
		
    system("echo 1 > /sys/class/gpio/PBB.00/value");
		
	}
	else{
		autoDac = 0;
		
    system("echo 0 > /sys/class/gpio/PBB.00/value");
	}
}

int setup(){
    int ret = 0;
    
    //send_spi_msg(244);
    //system("echo 1 > /sys/class/gpio/PBB.00/value");

    fd = open(device, O_RDWR);
	if (fd < 0)
		_pabort("can't open device");

    // SPI MODE (Set to mode 3)
    mode |= SPI_CPOL;
    mode |= SPI_CPHA;

	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		_pabort("can't set spi mode");
        return ret;

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		_pabort("can't get spi mode");
        return ret;

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		_pabort("can't set bits per word");
        return ret;

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		_pabort("can't get bits per word");
        return ret;

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		_pabort("can't set max speed hz");
        return ret;

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		_pabort("can't get max speed hz");
        return ret;

    return ret;
}

uint8_t* send_spi_msg(int num){

    //Bitmasking int to speed
    //Highest int is 1023

    size_t size = 2;  // Convert into an array of 4 uint8 values
    uint8_t uint8_array[2];
    if(num < 485){
    	num = 485;
    }

    _int_to_uint8_array(num, default_tx, size);

    _transfer(fd, default_tx, default_rx, sizeof(default_tx));
    
    return default_rx;

}
