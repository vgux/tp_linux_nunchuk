/*
* Affichage de la position du joystick de 2 Nunchuk
* 1 Nunchuk en local, connecté en I2C
* 1 Nunchuk sur une autre raspberry qui transmet ses données sur la liaison série
*
* Affichage sur un écan OLED connecté en I2C
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <errno.h>
#include <u8g2port.h>
 
#define X_CIRCLE_CENTER 63
#define Y_CIRCLE_CENTER 31
#define CIRCLE_RADIUS   31
#define RIGHT_X (X_CIRCLE_CENTER + CIRCLE_RADIUS)
#define BOTTOM_Y (Y_CIRCLE_CENTER - CIRCLE_RADIUS)
 
 
#define DISC_RADIUS     1
 
int main(void)
{
    u8g2_t u8g2;
    int NUNCHUCK_DEVICE = 0x52;
 
    printf("GAME ON!\n");
    wiringPiSetup();
    int fd_i2c = wiringPiI2CSetup(NUNCHUCK_DEVICE);
    if (fd_i2c < 0) {
        printf("Error setting up I2C: %d\n", errno);
        exit(0);
    }


    int fd_serial;
    printf("serialOpen!\n");
    if ((fd_serial = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
    {
        printf("Unable to open serial device\n");
        return 1 ;
    }

    wiringPiI2CWriteReg8(fd_i2c, 0x40, 0x00);
    delayMicroseconds(500);
 
    // Initialization    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( &u8g2, U8G2_R0, u8x8_byte_arm_linux_hw_i2c, u8x8_arm_linux_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
 
    int bytes1[6];
    int bytes2[2];

    serialFlush (fd_serial);

    while(1) {

	wiringPiI2CWrite(fd_i2c, 0x00);
        delayMicroseconds(500);
 
	for (int i=0; i<6; i++) {
            bytes1[i] = wiringPiI2CRead(fd_i2c);
        }
 
        int joyX1 = bytes1[0];
        int joyY1 = bytes1[1];
 
        printf("data: joyX1=%x joyY1=%x\n", joyX1, joyY1);


 	      while(!serialDataAvail (fd_serial));

        for (int i=0; i<2; i++) {
            bytes2[i] = serialGetchar(fd_serial);
        }
 	      serialFlush(fd_serial);

        int joyX2 = bytes2[0];
        int joyY2 = bytes2[1];
 
        printf("data: joyX2=%x joyY2=%x\n", joyX2, joyY2);
 
        // Printing on screen
        u8g2_ClearBuffer(&u8g2);
 
        // Screen size : 128 x 64
        u8g2_DrawCircle(&u8g2, X_CIRCLE_CENTER, Y_CIRCLE_CENTER,CIRCLE_RADIUS, U8G2_DRAW_ALL);
        u8g2_DrawDisc(&u8g2, RIGHT_X - (joyX1/4), BOTTOM_Y + (joyY1/4), DISC_RADIUS, U8G2_DRAW_ALL);
 	      u8g2_DrawDisc(&u8g2, RIGHT_X - (joyX2/4), BOTTOM_Y + (joyY2/4), DISC_RADIUS, U8G2_DRAW_ALL);
 
        u8g2_SendBuffer(&u8g2);
      }
 
    return 0;
}
