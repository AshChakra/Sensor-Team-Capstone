#include "orin_spi.h"
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]){

    setup();
    int loop = 485;
    send_spi_msg(485);
    switch_auto();
    while(loop < 1023){
    	sleep(1);
	loop = loop + 10;
	printf("%d\r\n", loop);
	send_spi_msg(loop);

    }
    while(loop > 485){
    	sleep(1);
	loop = loop - 10;
	send_spi_msg(loop);
	printf("%d\r\n", loop);
    }

    
    //close_spi();

}
