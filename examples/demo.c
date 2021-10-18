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


#define I2C_ADDR0   0x26 // I2C device address
#define I2C_ADDR1   0x27 // I2C device address
#define I2C_ADDR2   0x23 // I2C device address



char buftime[25];


int free_disp;
int display0;
int display1;
int display2;

int n, m;
time_t czas1;
struct tm *loctime;


/* S T A R T */
void main (argc, argv)
int argc;
char *argv[];

{
display0=i2copen(1, I2C_ADDR0);
lcdinit(display0, 4, 16); // setup LCD
display1=i2copen(1, I2C_ADDR1);
lcdinit(display1, 4, 20); // setup LCD
display2=i2copen(1, I2C_ADDR2);
lcdinit(display2, 2, 16); // setup LCD




lcdSet(display0,1, 1, 0, 0); 
lcdSet(display1,1, 1, 0, 0); 
sleep(1);
lcdSet(display0,1, 0, 0, 1); 
lcdSet(display1,1, 0, 0, 1); 
lcdSet(display2,1, 0, 0, 1); 

lcdClr(display0);
lcdClr(display1);
lcdClr(display2);
sleep(1);

time (&czas1);            
loctime = localtime (&czas1);
strftime (buftime, 25, "%Y-%m-%d  %H:%M:%S", loctime);

lcdLine(display0,0,0,"P234567890");
lcdLine(display0,10,1,"D23456789K");
lcdLine(display0,10,2,"T23");

lcdLine(display2,0,0,"P234567890");
lcdLine(display2,0,1,"D23456789K");

lcdLine(display1,0,0,"P234567890");
lcdLine(display1,10,1,"D123456789K");
lcdLine(display1,10,2,"T23");
sleep(1);

lcdClr(display0);
lcdClr(display1);
lcdClr(display2);
lcdBlock(display0,5,2,"block01234567890123456789");
lcdBlock(display1,5,2,"block01234567890123456789");
lcdBlock(display2,5,1,"block01234567890123456789");

sleep(3);
lcdLine(display1,0,3,buftime);
lcdClr(display0);
lcdLoc(display0, 0,0);
lcdChar(display0, 'A');
lcdLoc(display0, 5,1);
lcdChar(display0, 'B');
lcdLoc(display0, 10,2);
lcdChar(display0, 'C');
lcdLoc(display0, 15,3);
lcdChar(display0, 'D');

}                               // od main




