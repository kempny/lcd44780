/********************************************************************************
* demo.c
*       This file is part of lcdi2c - RaspberryPi library
*       for LCD display with the HITACHI HD44780 and compatible controlleri,
*       I2C interface with PCF8574 or/and PCF8574A I/O expander
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************
*    lcdi2c is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    lcdi2c is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
*
*********************************************************************************
**   demo program for 3 displays connected to I2C bus                          **    
**   compile with:                                                             **
**     make                                                                    **
**   run:                                                                      **
**     ./demo                                                                  **
********************************************************************************/

#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <lcdi2c.h>
#include <pthread.h>



// 3 displays,   i2c addresses
// change if necessary to your addresses

#define I2C_ADDR0   0x26 // I2C device address
#define I2C_ADDR1   0x27 // I2C device address
#define I2C_ADDR2   0x23 // I2C device address

char buftime[25];


int display0;
int display1;
int display2;

int n, m;
time_t czas1;
struct tm *loctime;

void *disptime(void *ptr)
{
int fdlcd;
int i;

  fdlcd =  (int)ptr;

  for(;;)
   {
    time (&czas1);
    loctime = localtime (&czas1);
    strftime (buftime, 25, "%a %H:%M:%S", loctime);
    lcdLine(fdlcd,0,3,buftime);
    Delay_mls(500);
   }
}


/* S T A R T */
void main (argc, argv)
int argc;
char *argv[];

{

unsigned char matrix[8]; 

// display 0, 4 lines, 16 columns
  display0=i2copen(1, I2C_ADDR0);
  lcdinit(display0, 4, 16); // setup LCD

// display 1, 4 lines, 20 columns
  display1=i2copen(1, I2C_ADDR1);
  lcdinit(display1, 4, 20); // setup LCD

// display 2, 2 lines, 16 columns
  display2=i2copen(1, I2C_ADDR2);
  lcdinit(display2, 2, 16); // setup LCD


// turn display and backlight on, cursor and blinking off
  lcdSet(display0,1, 0, 0, 1); 
  lcdSet(display1,1, 0, 0, 1); 
  lcdSet(display2,1, 0, 0, 1); 

//Clear displays
  lcdClr(display0);
  lcdClr(display1);
  lcdClr(display2);

// define custom characters
  matrix[0] = 0x1f;
  matrix[1] = 0x11;
  matrix[2] = 0x11;
  matrix[3] = 0x11;
  matrix[4] = 0x11;
  matrix[5] = 0x11;
  matrix[6] = 0x11;
  matrix[7] = 0x1f;
  lcdDefc(display0, 0, matrix);
  lcdChar(display0, 0x00);

  matrix[0] = 0x04;
  matrix[1] = 0x0e;
  matrix[2] = 0x1f;
  matrix[3] = 0x1f;
  matrix[4] = 0x04;
  matrix[5] = 0x04;
  matrix[6] = 0x04;
  matrix[7] = 0x0e;
  lcdDefc(display0, 1, matrix);

  matrix[0] = 0x0e;
  matrix[1] = 0x04;
  matrix[2] = 0x04;
  matrix[3] = 0x04;
  matrix[4] = 0x1f;
  matrix[5] = 0x1f;
  matrix[6] = 0x0e;
  matrix[7] = 0x04;
  lcdDefc(display0, 2, matrix);
  lcdLoc(display0, 1,0);
  lcdChar(display0, 0x01);
  lcdChar(display0, 0x02);
  lcdLine(display0,2,1,"Custom");
  lcdLine(display0,2,2,"characters");
  sleep(3);
  lcdClr(display0);


// Display block of text
  lcdBlock(display0,0,1,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvw");
  lcdLoc(display0, 0,0);

  sleep(5);

  lcdClr(display0);
  lcdClr(display2);
  lcdClr(display2);

// Display some texts on different displays
  lcdLine(display0,6,0,"Test");
  lcdLine(display0,3,1,"Display 0");
  lcdLine(display1,6,0,"Test");
  lcdLine(display1,3,1,"Display 1");
  lcdLine(display2,6,0,"Test");
  lcdLine(display2,3,1,"Display 2");
  sleep(3);

// Create thread to display date and time on display 0

  pthread_t thread_id;

  pthread_create (&thread_id, NULL, disptime, (void *)display0);

//Display animated text 

// Mode 1, display 0
  lcdRun(display0, 1, 3, 1, "++To be or not to be -- ");

//Mode 0, display 1
  lcdRun(display1, 0, 3, 1, "++To be or not to be, that is the question --");

//Mode 0, display 2
  lcdRun(display2, 0, 3, 1, "++To be or not to be, that is the question --");

  sleep(10);

//stop displaying animated text on display 0
  lcdStop(display0);

// Clear display 0
  lcdClr(display0);
  sleep(2);


//display single characters on  display 0
  lcdLoc(display0, 0,0);
  lcdChar(display0, 'A');
  lcdLoc(display0, 5,1);
  lcdChar(display0, 'B');
  lcdLoc(display0, 10,2);
  lcdChar(display0, 'C');
  lcdLoc(display0, 15,3);
  lcdChar(display0, 'D');

  sleep(3);

//Stop animation on display2
  lcdStop(display2);
  sleep(2);


//Switch off display 2
  lcdSet(display2,0, 0, 0, 1); 
  sleep(3);

//Switch on display 2
  lcdSet(display2,1, 0, 0, 1); 
  sleep(2);

//Switch off backlight of display 1
  lcdSet(display1,1, 0, 0, 0); 
  sleep(3);

//Switch off backlight of display 1
  lcdSet(display1,1, 0, 0, 1); 
  sleep(2);

//Stop animation on display1
  lcdStop(display1);

  lcdClr(display0);
  lcdClr(display1);
  lcdClr(display2);
  lcdLine(display0,5,1,"The End");
  lcdLine(display1,6,1,"The End");
  lcdLine(display2,5,0,"The End");

//  the time is still displayed by thread

  sleep(5);

}                              




