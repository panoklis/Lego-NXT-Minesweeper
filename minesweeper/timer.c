#include "timer.h"
#include "pit.h"
#include <stdconst.h>
#include "display.h"
#include "i2c.h"
#define TICKS_IN_A_MS 3000
ULONG volatile msTemp;

void time_handler(void){
    msTemp = msTemp + 1;
    PITAckInterrupt();
    return;
}

void StartTimer(void){
    msTemp = 0;
    PITEnable();
    PITInterruptEnable(TICKS_IN_A_MS, time_handler);
    return;
}

void StopTimer(void){
    PITInterruptDisable();
    PITDisable();
    return;
}

void ResetTimer(void){
    StopTimer();
    StartTimer();
    return;
}

ULONG GetMs(void){
    return msTemp;
}

void Sleep(ULONG ms_to_wait){
    ULONG x = msTemp + ms_to_wait;
    while(msTemp <= x);
    //    I2CTransfer();
    return;
}

void I2CSleep(ULONG ms_to_wait){
    ULONG x = msTemp + ms_to_wait;
    while(msTemp <= x){I2CTransfer();}
    return;
}

void I2CWait(void){
    ULONG x = msTemp + 2;   // wait 2 ms
    while(msTemp <= x);
    return;
}
