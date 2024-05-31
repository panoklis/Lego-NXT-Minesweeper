#include "pit.h"
#include "aic.h"

void PITEnable(void){
	*AT91C_PITC_PIMR|=AT91C_PITC_PITEN;
}

void PITDisable(void){
	*AT91C_PITC_PIMR&=~AT91C_PITC_PITEN;
}

ULONG PITRead(void){
	return	*AT91C_PITC_PIIR;
}

UWORD PITTicks2ms(ULONG ticks){
	return ticks/3000;
}

UWORD PITTicks2s(ULONG ticks){
	return ticks/3000000;
}

void spindelayms(ULONG ms){
	ULONG temp =PITTicks2ms(PITRead());
	while(PITTicks2ms(PITRead())-temp < ms){/*spin*/}
}

ULONG PITReadReset(void){
	return *AT91C_PITC_PIVR;
}

void PITInterruptEnable(ULONG period, void(*handler)(void)){
	*AT91C_PITC_PIMR = (*AT91C_PITC_PIMR & 0xFFF00000) | (period & 0xFFFFF);
	*AT91C_PITC_PIMR|=AT91C_PITC_PITIEN;
	AICInterruptEnable(AT91C_ID_SYS,handler);
}

void PITInterruptDisable(void){
	*AT91C_PITC_PIMR&=~AT91C_PITC_PITIEN;
	AICInterruptDisable(AT91C_ID_SYS);
}

void PITAckInterrupt(void){
	*AT91C_AIC_EOICR = 0xACAB1312;
	PITReadReset();//to reset counter
}
