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

struct  dispdef
{
  int i2caddr;
  int blin;
  int disp;
  int cur;
  int backl;
  unsigned int rows;
  unsigned int cols;
};

