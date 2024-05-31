#include "input.h"
#include <stdconst.h>
#include "arm2avr.h"
#include "display.h"
#include <stdio.h>
#include <stdlib.h>
void InputInit(void){
    for(UBYTE i=0; i<NOS_OF_AVR_INPUTS; i++)IoFromAvr.AdValue[i] = 1023;
    IoFromAvr.Buttons = 0x00;
    IoFromAvr.Battery = 0x00;
    return;
}

void InputExit(void){
    for(UBYTE i=0; i<NOS_OF_AVR_INPUTS; i++)IoFromAvr.AdValue[i] = 0x00;
    IoFromAvr.Buttons = 0x00;
    IoFromAvr.Battery = 0x00;
    return;
}

void InputGetSensorValue(UWORD *value, UBYTE port){
    *value=IoFromAvr.AdValue[port-1];
    return;
}

void getTouch(UWORD *value){
    InputGetSensorValue(value,1);
    *value = (*value > 512)? 0 : 1;
    return;
}

void getLight(UWORD *value){
	InputGetSensorValue(value,3);
	*value = *value < 500?1:0;
	return;
}

void getMic(UWORD *value){
	InputGetSensorValue(value,2);
	*value = 100 - (float)*value/(float)10;
	return;
}

void getUltrasonic(UWORD *value,UBYTE port){
	InputGetSensorValue(value,port);
	//*value = 110 - (float)*value/(float)10;
	//if(port == 2 && *value>20) {*value = 1; return;}
	//if(port == 2){*value = 0; return;}
	*value = *value <1000 ? 1 : 0;
	return;
}

void logInputs(char* filename){
	FILE *fptr;
	fptr = fopen(filename,"w");
	if(fptr == NULL)exit(-1);
	fprintf(fptr,"a");
	fclose(fptr);
}
