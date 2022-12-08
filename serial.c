#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <errno.h>

char get_letter(char current_letter)
{
    if (current_letter == 'z') {
        return 'a';
    }

    return ++current_letter;
}

int main(void) {
        if (wiringPiSetup () == -1)
    {
        printf("Unable to start wiringPi\n");
        return 1 ;
    }

    int fd ;

    if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
    {
        printf("Unable to open serial device\n");
        return 1 ;
    }

    char tx_letter = 'a' - 1;
    char rx_letter = 0;

    while(1) {
        tx_letter = get_letter(tx_letter);
        printf("Sending %c", tx_letter);

        serialPutchar (fd, tx_letter) ;

        rx_letter = serialGetchar (fd);
        printf(" -> Received : %c\n", rx_letter);

	for(int j = 0; j < 100; j++) {
        	for(unsigned int i = 0; i<64000; i++);
	}
    }

    return 0;
}
