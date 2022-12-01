#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
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
    int fd = wiringPiI2CSetup(NUNCHUCK_DEVICE);
    if (fd < 0) {
        printf("Error setting up I2C: %d\n", errno);
        exit(0);
    }
    wiringPiI2CWriteReg8(fd, 0x40, 0x00);
    delayMicroseconds(500);
 
    // Initialization    
    u8g2_Setup_ssd1306_i2c_128x64_noname_f( &u8g2, U8G2_R0, u8x8_byte_arm_linux_hw_i2c, u8x8_arm_linux_gpio_and_delay);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
 
    int bytes[6];
    int i;
    while(1) {
        wiringPiI2CWrite(fd, 0x00);
        delayMicroseconds(500);
 
        for (i=0; i<6; i++) {
            bytes[i] = wiringPiI2CRead(fd);
        }
 
        int joyX = bytes[0];
        int joyY = bytes[1];
        int accelX = (bytes[2] << 2) | ((bytes[5] & 0xc0) >> 6);
        int accelY = (bytes[3] << 2) | ((bytes[5] & 0x30) >> 4);
        int accelZ = (bytes[4] << 2) | ((bytes[5] & 0x0c) >> 2);
        int c = (bytes[5] & 0x02) >> 1;
        int z = bytes[5] & 0x01;
 
        printf("data: joyX=%x joyY=%x accelX=%x accelY=%x accelZ=%x c=%x z=%x\n", joyX, joyY, accelX, accelY, accelZ, c, z);
 
 
        // Printing on screen
        u8g2_ClearBuffer(&u8g2);
 
        // Screen size : 128 x 64
        u8g2_DrawCircle(&u8g2, X_CIRCLE_CENTER, Y_CIRCLE_CENTER,CIRCLE_RADIUS, U8G2_DRAW_ALL);
        u8g2_DrawDisc(&u8g2, RIGHT_X - (joyX/4), BOTTOM_Y + (joyY/4), DISC_RADIUS, U8G2_DRAW_ALL);
 
 
        u8g2_SendBuffer(&u8g2);
      }
 
    return 0;
}
