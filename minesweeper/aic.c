#include "aic.h"

void AICInit(void){
	*AT91C_AIC_IDCR = 0xFFFFFFFF;
}

void AICInterruptEnable(int which, void(*handler)(void)){
	AT91C_BASE_AIC->AIC_SMR[which] = 0x20;//edge triggered with lowest priority
	*AT91C_AIC_IECR |= 0x1 << which;
	AT91C_BASE_AIC->AIC_SVR[which] = handler;
}

void AICInterruptDisable(int which){
	*AT91C_AIC_ICCR |= 0x1 << which;
	AT91C_BASE_AIC->AIC_SVR[which] = 0x00000000;
	*AT91C_AIC_IDCR |= 0x1 << which;
}
