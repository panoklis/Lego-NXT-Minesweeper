#include "output.h"
#include "arm2avr.h"
#include <stdconst.h>


void OutputInit(void){
    IoToAvr.Power = 0x0;            
    IoToAvr.PwmFreq = 8;            
    
    OutputSetSpeed(1,0);   
    OutputSetSpeed(2,0);
    OutputSetSpeed(3,0);
    OutputSetSpeed(4,0);  
    return;
}

void OutputExit(void){
    OutputSetSpeed(1,0);   
    OutputSetSpeed(2,0);
    OutputSetSpeed(3,0);
    OutputSetSpeed(4,0);  
    return;
}

void OutputSetSpeed (UBYTE MotorNr, SBYTE Speed){
    if(MotorNr < 1 || MotorNr > 4)return;
    if(Speed > MAX_SPEED_FW)Speed = MAX_SPEED_FW;
    if(Speed < MAX_SPEED_RW)Speed = MAX_SPEED_RW;
    
    IoToAvr.OutputMode = (Speed==0)?1:0;
    IoToAvr.PwmValue[MotorNr-1] = Speed;

    return;
}
