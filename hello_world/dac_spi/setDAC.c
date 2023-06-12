#include "orin_spi.h"
#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[]){

    // MIN IS 485
    setup();
    send_spi_msg(1023);
    switch_auto();

}
