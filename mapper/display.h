#ifndef   DISPLAY
#define   DISPLAY

#include  "spi.h"
#include <stdconst.h>
#include <stdlib.h>
#include <math.h>

#define   DISPLAY_HEIGHT      64        // Y pixels
#define   DISPLAY_WIDTH       100       // X pixels

#define	  CMD		0
#define	  DAT		1
#define   DISP_LINES    (DISPLAY_HEIGHT/8)

UBYTE     DisplayWrite(UBYTE Type, UBYTE * pData, UWORD Length);
void      DisplayInit(void);
void      DisplayExit(void);
void      DisplayErase(void);
void      DisplayUpdateSync(void);
void      DisplayChar(UBYTE X,UBYTE Y,UBYTE Char);
void      DisplayNum(UBYTE X,UBYTE Y,ULONG Num);
void      DisplayString(UBYTE X,UBYTE Y,UBYTE *pString);
void      DisplaySetPixel(UBYTE X,UBYTE Y);
void      DisplayClrPixel(UBYTE X,UBYTE Y);
void      DisplayLineX(UBYTE X1,UBYTE X2,UBYTE Y);
void      DisplayLineY(UBYTE X,UBYTE Y1,UBYTE Y2);
//Clocks
void printtime_digital(UBYTE hh, UBYTE mm, UBYTE ss, UBYTE xx, UBYTE yy);

#endif
