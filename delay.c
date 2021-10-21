/********************************************************************************
*   delay.c
*       This file is part of lcdi2c - RaspberryPi library
*       for LCD display with the HITACHI HD44780 and compatible controlleri,
*       I2C interface with PCF8574 or/and PCF8574A I/O expander
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************

*********************************************************************************
*    stepmotor is free software: you can redistribute it and/or modify
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
********************************************************************************/


#include <time.h>
#include <unistd.h>


struct timespec tim;

void Delay_sec(int sec)
{
 tim.tv_sec = sec;
 tim.tv_nsec = 0;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}

void Delay_mls(int milisec)
{
 tim.tv_sec = 0;
 tim.tv_nsec = milisec * 1000000;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}

void Delay_mcs(int microsec)
{
 tim.tv_sec = 0;
 tim.tv_nsec = microsec * 1000;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}

void Delay_ns(int nanosec)
{
 tim.tv_sec = 0;
 tim.tv_nsec = nanosec;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}
