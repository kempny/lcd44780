/********************************************************************************
* demo1.c
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
*    stepmotor is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
*
*********************************************************************************
**   demo program for 1 display                                                ** 
**   compile with:                                                             **
**     make                                                                    **
**   run:                                                                      **
**     ./demo1                                                                 **
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



//  i2c address, change if necessary

#define I2C_ADDR0   0x27 // I2C device address


char buftime[25];


int display0;

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


// display 0, 4 lines, 20 columns
  display0=i2copen(1, I2C_ADDR0);
  lcdinit(display0, 4, 20); // setup LCD

// turn display and backlight on, cursor and blinking off
  lcdSet(display0,1, 0, 0, 1); 

//Clear display
  lcdClr(display0);

// Display some text
lcdLine(display0,6,0,"Test");
lcdLine(display0,3,1,"Display 0");
sleep(3);

// Create thread to display date and time on display 0

  pthread_t thread_id;

  pthread_create (&thread_id, NULL, disptime, (void *)display0);

//Display animated text 

//Mode 0, display 0
lcdRun(display0, 0, 3, 1, "++ To be or not to be -- ");
sleep(15);

lcdStop(display0);
lcdClr(display0);
sleep(2);

// Mode 1, display 0
lcdRun(display0, 1, 3, 1, "++ To be or not to be -- ");
sleep(10);

//stop displaying animated text on display 0
lcdStop(display0);
lcdClr(display0);
sleep(2);

//Display line of text on display 0
lcdLine(display0,0,1,"ABCDEFGHI");
sleep(2);
lcdClr(display0);



//Display block of text on display 0
lcdBlock(display0,5,1,"block 0123456789 0123456789 01234567890");
sleep(4);
lcdClr(display0);

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

//Switch off display 0
  lcdSet(display0,0, 0, 0, 1); 
sleep(2);

//Switch on display 0
  lcdSet(display0,1, 0, 0, 1); 
sleep(2);

lcdClr(display0);
lcdLine(display0,5,1,"The End");

//  the time is still displayed by thread

sleep(5);

}                              




