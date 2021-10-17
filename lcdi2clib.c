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

#define DATA  1 // Mode - Sending data
#define CMD  0 // Mode - Sending command
#define ENABLE  0b00000100 // Enable bit
#define MAXDISPL 16  // how many displays we handle

int konvline[4]={0x80, 0xC0, 0x94, 0xd4};

unsigned char byte;

struct dispdef parm[MAXDISPL];

int free_disp;

void typeln(int fdlcd, const char *s);

unsigned char cmd_datac;



//*****************************************
// Display one line of text, truncated on the right if necessary 

void lcdLine(int fdlcd, int x, int y, char* text)
{
static int i, j; 

 for(j=0; j<100; j++)
  {
   if(free_disp == 0) 
    break;
   Delay_mls(100);
  }
 
   if(free_disp == 1) 
     {
      printf("Display busy\n");
      return;
     }


  free_disp = 1;
  lcdLoc (fdlcd, x, y) ; 

j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
  for(i=0;i<j;i++)
    lcdChar(fdlcd, text[i]);
  free_disp=0;
}

//*****************************************
// Display block of text, wrap lines, truncated on the 
// end of display if necessary

void lcdBlock(int fdlcd, int x, int y, char* text)
{
static int i, j;

 for(j=0; j<100; j++)
  {
   if(free_disp == 0)
    break;
   Delay_mls(100);
  }

   if(free_disp == 1)
     {
      printf("Display busy\n");
      return;
     }


  free_disp = 1;

j = (strlen(text) + x + (parm[fdlcd].cols * y ) < parm[fdlcd].cols * parm[fdlcd].rows) ? strlen(text) : (parm[fdlcd].cols * parm[fdlcd].rows - (y * parm[fdlcd].cols) - x);


  for(i=0;i<j;i++)
   {
    lcdLoc(fdlcd, (x + i) % parm[fdlcd].cols, ((x + (y * parm[fdlcd].cols) + i) / parm[fdlcd].cols));
    lcdChar(fdlcd, text[i]);
   }
  free_disp=0;
}



//*************************************
// Clear screen, cursor home
// Not thread safe

void lcdClr(int fdlcd)   {
  send_byte(fdlcd, 0x01, CMD);
  Delay_mls(5);  // clear screen takes long time to complete
  send_byte(fdlcd, 0x02, CMD);
}



//*************************************
// Set cursor to location row/column on LCD

void lcdLoc(int fdlcd, int col, int line)   {

  if(line < 2)
    send_byte(fdlcd, konvline[line] + col, CMD);
  else
    send_byte(fdlcd, konvline[line] + col - (20 - parm[fdlcd].cols), CMD);

}

//*************************************
// Display char at curor position

void lcdChar(int fdlcd, char val)   {

  send_byte(fdlcd, val, DATA);
}

//*************************************
// Set display on/off, cursor on/off blinking on/off, backlight on/off
// 1 - ON, 0 - OFF, 99 - unchanged

void lcdSet(int fdlcd, int disp, int cur, int blin, int back) // display, cursor, blink,
                                                    //  backlight on/off,
{
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
void lcdinit(int fdlcd, int rows, int cols)   {

// Initialise display
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

