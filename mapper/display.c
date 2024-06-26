#include "AT91SAM7S256.h"
#include "display.h"
#include "spi.h"

static struct {
  UBYTE   DataArray[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH];
} IOMapDisplay;

UBYTE DisplayInitCommands[] =
{
  0xEB, // LCD bias: 1/9=0xEB
  0x2F, // pump control: set build-in=0x2F
  0xA4, // all pixels: off=0xA4, on=0xA5
  0xA6, // inverse: off=0xA6, on=0xA7
  0x40, // set scroll line: 0=0x40-63=0x7F
  0x81, // set Vbias potentiometer (2-byte command): 0x81 
  0x5A, //      -"-         		             : on=0x5A, off=0x00
  0xC4, // LCD mapping: regular=0xC4, row-mirror=bit 2, col-mirror=bit 3, e.g. col-mirror=0xC2
  0x27, // set temp comp.: -0.20%/C=0x27
  0x29, // panel loading: <=15nF=0x28, >15nF=0x29
  0xA0, // framerate: 76fps=0xA0, 95fps=0xA1
  0x88, // RAM address control: no wrap around+no autoincremet=0x88
  0x23, // set multiplex rate: 1:65=0x23
  0xAF  // set display: on=0xAF, off=0xAE
};

typedef struct
{
  UBYTE   ItemPixelsX;
  UBYTE   ItemPixelsY;
  UBYTE   Data[];
} __attribute__((__packed__)) FONT, ICON;

const ICON Font = {
  // each character is 6x8 pixels represented as 6 bytes, where each byte is a "column" of 8 pixels
  0x06,      // Graphics Width
  0x08,      // Graphics Height
  {/* 32 first non-printable characters */
  0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,
    /* rest printable characters */
  0x00,0x00,0x00,0x00,0x00,0x00,	//ASCII order, starting from SPACE [32 - 128]
  0x00,0x06,0x5F,0x06,0x00,0x00,
  0x07,0x03,0x00,0x07,0x03,0x00,
  0x24,0x7E,0x24,0x7E,0x24,0x00,
  0x24,0x2B,0x6A,0x12,0x00,0x00,
  0x63,0x13,0x08,0x64,0x63,0x00,
  0x30,0x4C,0x52,0x22,0x50,0x00,
  0x00,0x07,0x03,0x00,0x00,0x00,
  0x00,0x3E,0x41,0x00,0x00,0x00,
  0x00,0x41,0x3E,0x00,0x00,0x00,
  0x08,0x3E,0x1C,0x3E,0x08,0x00,
  0x08,0x08,0x3E,0x08,0x08,0x00,
  0x80,0x60,0x60,0x00,0x00,0x00,
  0x08,0x08,0x08,0x08,0x08,0x00,
  0x00,0x60,0x60,0x00,0x00,0x00,
  0x20,0x10,0x08,0x04,0x02,0x00,
  0x3E,0x51,0x49,0x45,0x3E,0x00,
  0x00,0x42,0x7F,0x40,0x00,0x00,
  0x62,0x51,0x49,0x49,0x46,0x00,
  0x22,0x49,0x49,0x49,0x36,0x00,
  0x18,0x14,0x12,0x7F,0x10,0x00,
  0x2F,0x49,0x49,0x49,0x31,0x00,
  0x3C,0x4A,0x49,0x49,0x30,0x00,
  0x01,0x71,0x09,0x05,0x03,0x00,
  0x36,0x49,0x49,0x49,0x36,0x00,
  0x06,0x49,0x49,0x29,0x1E,0x00,
  0x00,0x6C,0x6C,0x00,0x00,0x00,
  0x00,0xEC,0x6C,0x00,0x00,0x00,
  0x08,0x14,0x22,0x41,0x00,0x00,
  0x24,0x24,0x24,0x24,0x24,0x00,
  0x00,0x41,0x22,0x14,0x08,0x00,
  0x02,0x01,0x59,0x09,0x06,0x00,
  0x3E,0x41,0x5D,0x55,0x1E,0x00,
  0x7E,0x11,0x11,0x11,0x7E,0x00,
  0x7F,0x49,0x49,0x49,0x36,0x00,
  0x3E,0x41,0x41,0x41,0x22,0x00,
  0x7F,0x41,0x41,0x41,0x3E,0x00,
  0x7F,0x49,0x49,0x49,0x41,0x00,
  0x7F,0x09,0x09,0x09,0x01,0x00,
  0x3E,0x41,0x49,0x49,0x7A,0x00,
  0x7F,0x08,0x08,0x08,0x7F,0x00,
  0x00,0x41,0x7F,0x41,0x00,0x00,
  0x30,0x40,0x40,0x40,0x3F,0x00,
  0x7F,0x08,0x14,0x22,0x41,0x00,
  0x7F,0x40,0x40,0x40,0x40,0x00,
  0x7F,0x02,0x04,0x02,0x7F,0x00,
  0x7F,0x02,0x04,0x08,0x7F,0x00,
  0x3E,0x41,0x41,0x41,0x3E,0x00,
  0x7F,0x09,0x09,0x09,0x06,0x00,
  0x3E,0x41,0x51,0x21,0x5E,0x00,
  0x7F,0x09,0x09,0x19,0x66,0x00,
  0x26,0x49,0x49,0x49,0x32,0x00,
  0x01,0x01,0x7F,0x01,0x01,0x00,
  0x3F,0x40,0x40,0x40,0x3F,0x00,
  0x1F,0x20,0x40,0x20,0x1F,0x00,
  0x3F,0x40,0x3C,0x40,0x3F,0x00,
  0x63,0x14,0x08,0x14,0x63,0x00,
  0x07,0x08,0x70,0x08,0x07,0x00,
  0x71,0x49,0x45,0x43,0x00,0x00,
  0x00,0x7F,0x41,0x41,0x00,0x00,
  0x02,0x04,0x08,0x10,0x20,0x00,
  0x00,0x41,0x41,0x7F,0x00,0x00,
  0x04,0x02,0x01,0x02,0x04,0x00,
  0x80,0x80,0x80,0x80,0x80,0x00,
  0x00,0x02,0x05,0x02,0x00,0x00,
  0x20,0x54,0x54,0x54,0x78,0x00,
  0x7F,0x44,0x44,0x44,0x38,0x00,
  0x38,0x44,0x44,0x44,0x28,0x00,
  0x38,0x44,0x44,0x44,0x7F,0x00,
  0x38,0x54,0x54,0x54,0x08,0x00,
  0x08,0x7E,0x09,0x09,0x00,0x00,
  0x18,0x24,0xA4,0xA4,0xFC,0x00,
  0x7F,0x04,0x04,0x78,0x00,0x00,
  0x00,0x00,0x7D,0x40,0x00,0x00,
  0x40,0x80,0x84,0x7D,0x00,0x00,
  0x7F,0x10,0x28,0x44,0x00,0x00,
  0x00,0x00,0x7F,0x40,0x00,0x00,
  0x7C,0x04,0x18,0x04,0x78,0x00,
  0x7C,0x04,0x04,0x78,0x00,0x00,
  0x38,0x44,0x44,0x44,0x38,0x00,
  0xFC,0x44,0x44,0x44,0x38,0x00,
  0x38,0x44,0x44,0x44,0xFC,0x00,
  0x44,0x78,0x44,0x04,0x08,0x00,
  0x08,0x54,0x54,0x54,0x20,0x00,
  0x04,0x3E,0x44,0x24,0x00,0x00,
  0x3C,0x40,0x20,0x7C,0x00,0x00,
  0x1C,0x20,0x40,0x20,0x1C,0x00,
  0x3C,0x60,0x30,0x60,0x3C,0x00,
  0x6C,0x10,0x10,0x6C,0x00,0x00,
  0x9C,0xA0,0x60,0x3C,0x00,0x00,
  0x64,0x54,0x54,0x4C,0x00,0x00,
  0x08,0x3E,0x41,0x41,0x00,0x00,
  0x00,0x00,0x77,0x00,0x00,0x00,
  0x00,0x41,0x41,0x3E,0x08,0x00,
  0x02,0x01,0x02,0x01,0x00,0x00,
  0x10,0x20,0x40,0x38,0x07,0x00
  }
};

UBYTE     DisplayLineString[DISP_LINES][3] =
{
  { 0xB0,0x10,0x00 },
  { 0xB1,0x10,0x00 },
  { 0xB2,0x10,0x00 },
  { 0xB3,0x10,0x00 },
  { 0xB4,0x10,0x00 },
  { 0xB5,0x10,0x00 },
  { 0xB6,0x10,0x00 },
  { 0xB7,0x10,0x00 }
};

UBYTE DisplayWrite(UBYTE Type, UBYTE * pData, UWORD Length) {
		UBYTE Result = FALSE;

		if (( * AT91C_SPI_SR & AT91C_SPI_TXEMPTY)) {
			if (Type) {
				SPIPIOSetData();
			} 
      else {
				SPIPIOClearData();
			}
			SPIWriteDMA(pData, Length);
			Result = TRUE;
		}
    return Result;
}

void DisplayInit(void){
  SPIInit();
  return;
}

void DisplayUpdateSync(void){
  UBYTE i;
  while(! DisplayWrite(CMD, &DisplayInitCommands[0], 14));
  for(i = 0; i < DISP_LINES; i++){
    while (! DisplayWrite(CMD, &DisplayLineString[i][0], 3));
    while (! DisplayWrite(DAT, &IOMapDisplay.DataArray[i][0], DISPLAY_WIDTH));
  }
  return;
}

void DisplaySetPixel(UBYTE X,UBYTE Y){
  UBYTE page = Y/8;
  UBYTE bit = Y%8;
  IOMapDisplay.DataArray[page][X] |= 1<<bit;
  return; 
}

void DisplayClrPixel(UBYTE X,UBYTE Y){
  UBYTE page = Y/8;
  UBYTE bit = Y%8;
  IOMapDisplay.DataArray[page][X] &= ~(1 << bit);
  return; 
}

void DisplayLineX(UBYTE X1, UBYTE X2, UBYTE Y){
  UBYTE i;
  if(X2 >= X1){
    for(i = X1; i<=X2; i++)
      DisplaySetPixel(i, Y);
  }
  else{
    for(i = X2; i<=X1; i++)
      DisplaySetPixel(i, Y);
  }
  return;
}

void DisplayLineY(UBYTE X, UBYTE Y1, UBYTE Y2){
  UBYTE i;
  if(Y2 >= Y1){
    for(i = Y1; i<=Y2; i++)
      DisplaySetPixel(X, i);
  }
  else{
    for(i = Y2; i<=Y1; i++)
      DisplaySetPixel(X, i);
  }
  return;
}

void DisplayErase(void){
	int i,j;
	for(i=0;i<DISPLAY_HEIGHT/8;i++){
		for(j=0;j<DISPLAY_WIDTH;j++)
			IOMapDisplay.DataArray[i][j] = 0x00;
	}
}

void DisplayChar(UBYTE X,UBYTE Y,UBYTE Char){
	UBYTE byte_i;
	for(int i=0;i<Font.ItemPixelsX;i++){
		byte_i = Font.Data[(int)(Char)*Font.ItemPixelsX + i];
		for(int j=0;j<Font.ItemPixelsY;j++){
			if(byte_i & (1<<j))
				DisplaySetPixel(X + i,Y + j);
			else
				DisplayClrPixel(X + i,Y + j);
		}
	}
}

void DisplayString(UBYTE X,UBYTE Y,UBYTE *pString){
	UBYTE x_i = X;
	UBYTE y_i = Y;
	unsigned len = 0;
  for(len=0;pString[len];len++); //strlen(pString)
	for(unsigned i=0;i<len;i++){
		if(x_i <= (DISPLAY_WIDTH-6) && y_i <= (DISPLAY_HEIGHT-8)){
			DisplayChar(x_i,y_i,pString[i]);
			x_i += Font.ItemPixelsX;
		}
		else if (x_i > (DISPLAY_WIDTH-6)){
			i--;
			x_i = 0;
			y_i += Font.ItemPixelsY;
			if(y_i > (DISPLAY_HEIGHT-8))
				y_i = 0;
		}
	}
}

void DisplayNum(UBYTE X,UBYTE Y,ULONG Num){
	int len = 0;
	ULONG Num_t = Num;
	while(Num_t){
		len++;
		Num_t/=10;
	}
	char *pString = malloc(sizeof(len+1));
	pString[len] = 0x00;
	for(int i=0;i<len;i++){
		pString[i] = (Num%10) + '0';
		Num/=10;
	}
	for(int i=0;i<(len/2);i++){
		char t_c = pString[i];
		pString[i] = pString[len-i-1];
		pString[len-i-1] = t_c;
	}
	DisplayString(X,Y,(UBYTE*) pString);
}

void printtime_digital(UBYTE hh, UBYTE mm, UBYTE ss, UBYTE xx, UBYTE yy) {
    char time_str[9];

    //Hour
    time_str[0] = ((int)(hh) / 10) + '0';
    time_str[1] = ((int)(hh) % 10) + '0';
    //Minute
    time_str[3] = ((int)(mm) / 10) + '0';
    time_str[4] = ((int)(mm) % 10) + '0';
    //Second
    time_str[6] = ((int)(ss) / 10) + '0';
    time_str[7] = ((int)(ss) % 10) + '0';
    //Tenth of second
    

    time_str[2] = ':';
    time_str[5] = ':';
    time_str[8] = '\0';
    DisplayString(xx, yy, (UBYTE*) &time_str);
    //DisplayNum(0x1,10*ds+5,10,ds);
}
