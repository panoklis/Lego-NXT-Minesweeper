#ifndef   _INPUT_H
#define   _INPUT_H
#include <stdconst.h>

void InputInit(void);
void InputExit(void);
void InputGetSensorValue(UWORD *value, UBYTE port);
void getMic(UWORD*);
void getUltrasonic(UWORD*,UBYTE);
void getLight(UWORD*);
void getTouch(UWORD*);
void logInputs(char*);
#endif
