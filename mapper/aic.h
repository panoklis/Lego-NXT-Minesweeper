#ifndef   AIC
#define   AIC

#include <stdconst.h>
#include "AT91SAM7S256.h"

void      AICInit(void);
void      AICInterruptEnable(int which, void (*handler)(void));
void      AICInterruptDisable(int which);

#endif
