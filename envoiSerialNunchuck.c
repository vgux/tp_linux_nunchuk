#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <errno.h>
#include <wiringSerial.h>

#define X_CIRCLE_CENTER 63
#define Y_CIRCLE_CENTER 31
#define CIRCLE_RADIUS   31
#define RIGHT_X (X_CIRCLE_CENTER + CIRCLE_RADIUS)
#define BOTTOM_Y (Y_CIRCLE_CENTER - CIRCLE_RADIUS)


#define DISC_RADIUS     1


int main(void)
{
    int NUNCHUCK_DEVICE = 0x52;

    printf("GAME ON!\n");
    wiringPiSetup();
    int fd_i2c = wiringPiI2CSetup(NUNCHUCK_DEVICE);
    if (fd_i2c < 0) {
        printf("Error setting up I2C: %d\n", errno);
        exit(0);
    }

    int fd_serial ;

    if ((fd_serial = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
    {
        printf("Unable to open serial device\n");
        return 1 ;
    }


    wiringPiI2CWriteReg8(fd_i2c, 0x40, 0x00);
    delayMicroseconds(500);

    int bytes[6];
    int i;
    while(1) {
        wiringPiI2CWrite(fd_i2c, 0x00);
        delayMicroseconds(500);

        for (i=0; i<6; i++) {
            bytes[i] = wiringPiI2CRead(fd_i2c);
        }

        int joyX = bytes[0];
        int joyY = bytes[1];


        printf("data: joyX=%x joyY=%x\n", joyX, joyY);
        serialPutchar(fd_serial,joyX);
        serialPutchar(fd_serial,joyY);
        for(int j = 0; j < 100; j++) {
        	for(unsigned int i = 0; i<64000; i++);

        }
        serialFlush(fd_serial);

      }

    return 0;
}
