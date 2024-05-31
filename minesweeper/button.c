#include "button.h"
#include <stdconst.h>
#include "arm2avr.h"
#include "display.h"
#include "pit.h"
#include "i2c.h"

enum button_t ButtonRead(void){
    UWORD b = IoFromAvr.Buttons;
    if(b == 2047)return BUTTON_ENTER;
    if(b >= 380 && b <= 406)return BUTTON_RIGHT;
    if(b >= 90 && b <= 127)return BUTTON_LEFT;
    if(b >= 750 && b <= 1023)return BUTTON_EXIT;
    if(b == 0)return BUTTON_NONE;
}

void ButtonExit(void){
    IoFromAvr.Buttons = 0;
    return;
}

void ButtonInit(void){
    IoFromAvr.Buttons = 0;
    return;
}
