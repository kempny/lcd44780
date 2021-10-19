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
    strftime (buftime, 25, "%Y-%m-%d  %H:%M:%S", loctime);
    lcdLine(fdlcd,0,3,buftime);
    Delay_mls(500);
   }
}


/* S T A R T */
void main (argc, argv)
int argc;
char *argv[];

{


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

// Create thread to display date and time on display 1

  pthread_t thread_id;

  pthread_create (&thread_id, NULL, disptime, (void *)display1);

//Display animated text 
lcdRun(display0, 1, 5, 0, "To be or not to be, that is the question     ");
lcdRun(display1, 1, 5, 0, "A123456789B123456789                    ");
lcdRun(display2, 1, 9, 1, "To be or not to be, that is the question     ");
sleep(4);

//stop displaying animated text on display 0
lcdStop(display0);

//Display line of text on display 1
lcdLine(display1,0,1,"ABCDEFGHI");
sleep(2);

// Clear display 0
lcdClr(display0);
sleep(2);


//Display block of texy on display 0
lcdBlock(display0,5,1,"block 0123456789 0123456789 01234567890");
sleep(4);
lcdClr(display0);

//display single characters at  display 0
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

// Clear display2, display some text
lcdClr(display2);
lcdLine(display2,6,0,"Test");
sleep(2);

//Switch off display 2
  lcdSet(display2,0, 0, 0, 1); 
sleep(2);

//Switch on display 2
  lcdSet(display2,1, 0, 0, 1); 
sleep(2);

//Stop animation on display1
lcdStop(display1);

lcdClr(display0);
lcdClr(display1);
lcdClr(display2);
lcdLine(display0,5,1,"The End");
lcdLine(display1,6,1,"The End");
lcdLine(display2,5,0,"The End");

//  the time will be displayed by thread

sleep(3);

}                              




