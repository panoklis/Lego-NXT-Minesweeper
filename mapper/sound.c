#include <stdconst.h>
#include "AT91SAM7S256.h"
#include "sound.h"
#include "aic.h"

void SoundInit(void){
  SoundIntDisable();
  SoundDisable();
  *AT91C_PMC_PCER   = (1L << AT91C_ID_SSC); /* Enable MCK clock   */
  *AT91C_PIOA_ODR   = AT91C_PA17_TD;
  *AT91C_PIOA_OWDR  = AT91C_PA17_TD;
  *AT91C_PIOA_MDDR  = AT91C_PA17_TD;
  *AT91C_PIOA_PPUDR = AT91C_PA17_TD;
  *AT91C_PIOA_IFDR  = AT91C_PA17_TD;
  *AT91C_PIOA_CODR  = AT91C_PA17_TD;
  *AT91C_PIOA_IDR   = AT91C_PA17_TD;
  *AT91C_SSC_CR   = AT91C_SSC_SWRST;
  *AT91C_SSC_TCMR = AT91C_SSC_CKS_DIV + 
                    AT91C_SSC_CKO_CONTINOUS + AT91C_SSC_START_CONTINOUS;
  *AT91C_SSC_TFMR = (-1)+(((sizeof(ULONG)*8) & 0xF) << 8) + AT91C_SSC_MSBF;
  *AT91C_SSC_CR   = AT91C_SSC_TXEN;         /* TX enable */
}

void SoundSync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration) {
  /* sound frequency will be (OSC/CMR*2)
     with CMR divisor values of 12 bits we can get a frequency range of 6KHz to 24MHz
     use rate values 0..255 to represent the range of 6KHz..24KHz, so div values 1K..4K
     for rate=0 => div=4K => freq ~= 6KHz and 
     for rate=255 => div=1K => freq ~= 24KHz
     *AT91C_SSC_CMR   = ((OSC / (2*sizeof(ULONG)*8)) / rate) + 1; 
   */
  UBYTE i=0;
  SoundEnable();
  *AT91C_SSC_CMR = (4095 - 12 * rate);
  while(duration-- > 0){
    for(i=0; i<length; i++){
      while(!(*AT91C_SSC_SR & AT91C_SSC_TXRDY)){/* wait */;};
      *AT91C_SSC_THR = pattern[i];
    }
  }
  SoundDisable();
  return;
}

ULONG *async_pattern;
UBYTE async_length;
UBYTE async_index;
UWORD async_duration;

void SoundASyncHandler(void){
  if(async_index < async_length){
    *AT91C_SSC_THR = async_pattern[async_index++];
  }
  else{
    async_index = 0;
    async_duration--;
    if(async_duration == 0){
      SoundDisable();
      SoundIntDisable();
      return;
    }
    *AT91C_SSC_THR = async_pattern[async_index++];
  }
  return;
}

void SoundASync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration){
  async_pattern = pattern;
  async_length = length;
  async_index = 1;
  async_duration = duration;
  *AT91C_SSC_CMR = (4095 - 12 * rate);
  *AT91C_SSC_THR = pattern[0];
  SoundEnable();
  SoundIntEnable(SoundASyncHandler);
  /*
  char buf[9];
        
        AT91C_SSC_IER
        AT91C_PIOA_IER
        AT91C_AIC_IECR
        AT91C_BASE_AIC->AIC_SMR[2]
        AT91C_BASE_AIC->AIC_SVR[2]

        print their values in hex
        
        ULONG registers[4];

        registers[0] = *AT91C_SSC_IMR;
        registers[1] = *AT91C_PIOA_IMR;
        registers[2] = AT91C_BASE_AIC->AIC_SVR[2];
        registers[3] = SoundASyncHandler;

        char *regnames[4] = {"SSC IMR", "PIO IMR", "AIC SVR", "handler"};
        DisplayErase();
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 8; i++) {
                buf[i] = (registers[j] >> (4 * (7 - i))) & 0xF;
                if (buf[i] < 10) {
                    buf[i] += 0x30;
                } else {
                    buf[i] += 0x37;
                }
            }
            buf[8] = 0x00;
            DisplayString(0x01,0x0A*j,regnames[j]);
            DisplayString(0x30,0x0A*j,buf);
        }
        DisplayUpdateSync();
        */

  return;
}

void SoundEnable(void){
  *AT91C_PIOA_PER |= AT91C_PA17_TD;
}
void SoundDisable(void){
  *AT91C_PIOA_PDR |= AT91C_PA17_TD;
}

void SoundIntEnable(void(*handler)(void)){
  *AT91C_SSC_IDR = 0x00000000;
  *AT91C_SSC_IER = AT91C_SSC_TXRDY;
  *AT91C_PIOA_IER = AT91C_PA17_TD;
  AICInterruptEnable(AT91C_PIO_PA3,handler);
}

void SoundIntDisable(void){
  *AT91C_SSC_IDR = 0xFFFFFFFF;
  *AT91C_PIOA_IDR = AT91C_PA17_TD;
  AICInterruptDisable(AT91C_PIO_PA3);
}
