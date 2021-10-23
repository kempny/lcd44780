/********************************************************************************
* lcdi2clib.c
*       This file is part of lcdi2c - RaspberryPi library
*       for LCD display with the HITACHI HD44780 and compatible controller,
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
*********************************************************************************/

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
#include "pthread.h"


#define DATA  1 // Mode - Sending data
#define CMD  0 // Mode - Sending command
#define ENABLE  0b00000100 // Enable bit
#define MAXDISPL 16  // how many displays we handle

int konvline[4]={0x80, 0xC0, 0x94, 0xd4};

unsigned char byte;

struct dispdef parm[MAXDISPL];


int dispfree(int fdlcd);
void lcdLocI(int fdlcd, int col, int line);
void lcdCharI(int fdlcd, char val);

unsigned char cmd_datac;



//*****************************************
// Display one line of text, truncated on the right if necessary 

void lcdLine(int fdlcd, int x, int y, char* text)
{
int i, j; 

  if ( dispfree(fdlcd) ) return;

    lcdLocI (fdlcd, x, y) ; 

    j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
    for(i=0;i<j;i++)
    lcdCharI(fdlcd, text[i]);
    parm[fdlcd].free_disp=0;
}

//*****************************************
// Display one line of text, truncated on the right if necessary 
// For internal use of library, display is already locked

void lcdLineI(int fdlcd, int x, int y, char* text)
{
int i, j; 

    lcdLocI (fdlcd, x, y) ; 

    j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
    for(i=0;i<j;i++)
    lcdCharI(fdlcd, text[i]);
}

//*****************************************
// Display block of text, wrap lines, truncated at the 
// end of display if necessary

void lcdBlock(int fdlcd, int x, int y, char* text)
{
int i, j;


  if ( dispfree(fdlcd) ) return;

    j = (strlen(text) + x + (parm[fdlcd].cols * y ) < parm[fdlcd].cols * parm[fdlcd].rows) ? strlen(text) : (parm[fdlcd].cols * parm[fdlcd].rows - (y * parm[fdlcd].cols) - x);


    for(i=0;i<j;i++)
     {
      lcdLocI(fdlcd, (x + i) % parm[fdlcd].cols, ((x + (y * parm[fdlcd].cols) + i) / parm[fdlcd].cols));
      lcdCharI(fdlcd, text[i]);
     }
    parm[fdlcd].free_disp=0;
}

/**********************************************************/
void *run(void *ptr)
{

int fdlcd;
int i;


  fdlcd =  (int)ptr;

  parm[fdlcd].pos = 0;
  parm[fdlcd].move = 1;
  while(parm[fdlcd].move)
   {
    if (! dispfree(fdlcd) )
     {
      if((parm[fdlcd].move_mode == 0)) // at end start from begining
       {
        lcdLineI(fdlcd, 0, parm[fdlcd].line, &parm[fdlcd].text[parm[fdlcd].pos]);
        if (parm[fdlcd].pos > strlen(parm[fdlcd].text) - parm[fdlcd].cols)
          lcdLineI(fdlcd, 
                   strlen(parm[fdlcd].text) - parm[fdlcd].pos, 
                   parm[fdlcd].line, 
                   "                              ");
          
         {
          parm[fdlcd].pos++;
          if (parm[fdlcd].pos == strlen(parm[fdlcd].text))
             parm[fdlcd].pos = 0;
         }
       }
      else
       if((parm[fdlcd].move_mode == 1)) // append begining of text to end of text
        {
          lcdLineI(fdlcd, 0, parm[fdlcd].line, &parm[fdlcd].text[parm[fdlcd].pos]);
          if(strlen(parm[fdlcd].text) > parm[fdlcd].cols)
           {
            parm[fdlcd].pos++;
            if (parm[fdlcd].pos > strlen(parm[fdlcd].text) - parm[fdlcd].cols -1)
             {
               lcdLineI(fdlcd, strlen(parm[fdlcd].text) - parm[fdlcd].pos +1, 
                              parm[fdlcd].line, parm[fdlcd].text);

               if (parm[fdlcd].pos > strlen(parm[fdlcd].text))
                 parm[fdlcd].pos = 1; //  not 0, the line is already displayed
             }
           }
        }
     }
    parm[fdlcd].free_disp = 0;
    for(i = 0; i < parm[fdlcd].speed; i++)
      Delay_mls(100);
   }
  return(0);
}

/**********************************************************/
// speed  - speed of animation, in 1/10 sec increments
// mode - mode of animation

void lcdRun(int fdlcd, int mode, int speed, int line, char* text)
{
    pthread_t thread_id;
    parm[fdlcd].speed = speed;
    parm[fdlcd].move_mode = mode;
    parm[fdlcd].line = line;
    strncpy(parm[fdlcd].text, text, ANIMLEN); 
    pthread_create (&thread_id, NULL, run, (void *)fdlcd);

}

/**********************************************************/
void lcdStop(int fdlcd)
{
  parm[fdlcd].move = 0;
}

//****************************************
// Wait for display free 
int dispfree(int fdlcd)
{
 static int j;
 for(j=0; j<100; j++)
  {
   if(parm[fdlcd].free_disp == 0)
    {parm[fdlcd].free_disp = 1;
     return 0;
    }
   Delay_mls(10);
  }

   printf("Display busy\n");
   return 1;
}

//*************************************
// Clear screen, cursor home
// Not thread safe

void lcdClr(int fdlcd)   {

  if ( dispfree(fdlcd) ) return;

  send_byte(fdlcd, 0x01, CMD);
  Delay_mls(5);  // clear screen takes long time to complete
  send_byte(fdlcd, 0x02, CMD);
  parm[fdlcd].free_disp=0;
}
//*************************************
// Define character


void lcdDefc(int fdlcd, int addr, unsigned char* matrix) {

int i;

send_byte(fdlcd, 0x40 | addr * 8, CMD);
for (i=0; i<8; i++)
  send_byte(fdlcd, matrix[i], DATA);

send_byte(fdlcd, 0x80, CMD); //switch to DDRAM addressing
} 


//*************************************
// Set cursor to location row/column on LCD
// For internal use of library, display is already locked

void lcdLocI(int fdlcd, int col, int line)   {


  if(line < 2)
    send_byte(fdlcd, konvline[line] + col, CMD);
  else
    send_byte(fdlcd, konvline[line] + col - (20 - parm[fdlcd].cols), CMD);
}

//*************************************
// Set cursor to location row/column on LCD

void lcdLoc(int fdlcd, int col, int line)   {

  if ( dispfree(fdlcd) ) return;

  if(line < 2)
    send_byte(fdlcd, konvline[line] + col, CMD);
  else
    send_byte(fdlcd, konvline[line] + col - (20 - parm[fdlcd].cols), CMD);
  parm[fdlcd].free_disp=0;
}


//*************************************
// Display char at cursor position
// For internal use of library, display is already locked

void lcdCharI(int fdlcd, char val)   {

  send_byte(fdlcd, val, DATA);
}

//*************************************
// Display char at curor position

void lcdChar(int fdlcd, char val)   {

  if ( dispfree(fdlcd) ) return;

  send_byte(fdlcd, val, DATA);
  parm[fdlcd].free_disp=0;
}

//*************************************
// Set display on/off, cursor on/off blinking on/off, backlight on/off
// 1 - ON, 0 - OFF, 99 - unchanged

void lcdSet(int fdlcd, int disp, int cur, int blin, int back) // display, cursor, blink,
                                                    //  backlight on/off,
{

  if ( dispfree(fdlcd) ) return;

  if(back !=99)
     parm[fdlcd].backl = back << 3; 
  if(disp != 99)
     parm[fdlcd].disp = disp;
  if(cur != 99)
     parm[fdlcd].cur = cur;
  if(cur != 99)
     parm[fdlcd].blin = blin;
  byte = (parm[fdlcd].disp << 2) | (parm[fdlcd].cur << 1) | parm[fdlcd].blin | parm[fdlcd].backl;
  send_byte(fdlcd, byte, CMD);
  parm[fdlcd].free_disp=0;
}

//*************************************
void send_byte(int fdlcd, int data, int cmd_data)   {

  //Send byte to data pins
  // bits = the data
  // cmd_data = 1 for data, 0 for command
  // uses the two half byte writes to LCD
  

//send high 4 bits
  byte = data&0xf0;
  send_4(fdlcd, byte, cmd_data);

// send low 4 bits
  byte = data << 4 & 0xf0;
  send_4(fdlcd, byte, cmd_data);
}

//*************************************
void send_4(int fdlcd, unsigned char byte, int cmd_data)
{


//  set RS bit
  cmd_datac = cmd_data | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(65);

//set ENABLE bit
  cmd_datac= cmd_data |  ENABLE | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(65);

//send data
  cmd_datac = cmd_data | byte | ENABLE | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(400);

//clear ENABLE bit
  cmd_datac = (cmd_data & ~ENABLE) | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(500);
}

//*************************************
void send_8(int fdlcd, int byte, int cmd_data)   {


//set RS bit
  cmd_datac = cmd_data | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
//set ENABLE bit
  cmd_datac = cmd_data |  ENABLE | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(65);
//send data
  cmd_datac = cmd_data | byte | ENABLE | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(400);
//clear ENABLE bit
  cmd_datac = (cmd_data  & ~ENABLE) | parm[fdlcd].backl;
  write(fdlcd, &cmd_datac, 1);
  Delay_ns(500);
}

int displcount =0;

//*************************************
int i2copen(int controller, int device)
{

  if (displcount == (MAXDISPL))
      {printf("To many displays, only %d are supported\n", MAXDISPL); exit(1);}
  int file;
  char filename[20];

  snprintf(filename, 19, "/dev/i2c-%d", controller);

  file = open(filename, O_RDWR);
  if (file < 0) {
    printf("Invalid controller: %s\n", filename);

    exit(1);
    }
  if (ioctl(file, I2C_SLAVE, device) < 0) {
    printf("Invalid device: %d\n", I2C_SLAVE);
    }
  parm[displcount].i2caddr = device;
  displcount++;

  return(file);

}

//*************************************
// initialise display, not thread safe

void lcdinit(int fdlcd, int rows, int cols)   {

// Initialise display
  parm[fdlcd].free_disp=0;
  send_8(fdlcd, 0x30,  CMD);
  Delay_mls(5);
  send_8(fdlcd, 0x30,  CMD);
  Delay_mcs(110);
  send_8(fdlcd, 0x30,  CMD);
  Delay_mls(5);
  send_8(fdlcd, 0x20,  CMD);


  send_byte(fdlcd, 0x06, CMD); // Cursor move direction

  send_byte(fdlcd, 0x0C, CMD); // display On, cursor Off, blink off
  send_byte(fdlcd, 0x28, CMD); // Data length, number of lines, font size
  send_byte(fdlcd, 0x01, CMD); // Clear display
  Delay_mls(5); // wait to complete clear display

  parm[fdlcd].rows = rows;
  parm[fdlcd].cols = cols;
  parm[fdlcd].disp = 1;
  parm[fdlcd].cur = 0;
  parm[fdlcd].blin = 0;
  parm[fdlcd].backl = 1;
}

