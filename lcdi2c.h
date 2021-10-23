/********************************************************************************
* lcdi2c.h:
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
*********************************************************************************/

void Delay_sec(int seconds);
void Delay_mcs(int microsec);
void Delay_mls(int milisec);
void Delay_ns(int nanosec);
void send_byte(int fdlcd, int bits, int cmd_data);
void send_8(int fdlcd, int byte, int cmd_data);
void send_4(int fdlcd, unsigned char byte, int cmd_data);
void lcdSet(int fdlcd, int disp, int cur, int blin, int back); //display, cursor, blink on/off,
int i2copen(int controller, int device);
void lcdLine(int fdlcd, int x, int y, char* text);
void lcdClr(int fdlcd);
void lcdinit(int fdlcd, int rows, int cols);
void lcdLoc(int fdlcd, int col, int line);
void lcdChar(int fdlcd, char val); 
void lcdBlock(int fdlcd, int x, int y, char* text);
void lcdRun(int fdlcd, int mode, int speed, int line, char* text);
void lcdStop(int fdlcd);
void lcdDefc(int fdlcd, int addr, unsigned char* matrix);


#define ANIMLEN 200

struct  dispdef
{
  int i2caddr;
  int blin;
  int disp;
  int cur;
  int backl;
  unsigned int rows;
  unsigned int cols;
  int free_disp;
  int move;
  int speed;
  int line;
  unsigned int pos;
  int move_mode;
  char text[ANIMLEN];
};

