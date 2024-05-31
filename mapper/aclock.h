#ifndef _ACLOCK_H_
#define _ACLOCK_H_
#include <stdconst.h>
#include "display.h"

#define RADIUS 25

void AclockDisplayFrame(UBYTE cx, UBYTE cy, UBYTE r);
void AclockDisplayFrameSymbol(UBYTE cx, UBYTE cy, UBYTE r, UWORD hh);
void AclockDisplayHand(UBYTE cx, UBYTE cy, UBYTE r, UWORD hh, double thickness , double height);

#endif

