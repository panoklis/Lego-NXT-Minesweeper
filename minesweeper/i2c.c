#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  <string.h>
#include  <assert.h>
#include  "i2c.h"
#include  "arm2avr.h"
#include "display.h"
#include "timer.h"

#define BYTES_TO_TX     sizeof(IOTOAVR)
#define BYTES_TO_RX     sizeof(IOFROMAVR)
#define TIMEOUT         2100
#define I2CClk          400000L
#define TIME400KHz      (((OSC/16L)/(I2CClk * 2)) + 1)
#define CLDIV           (((OSC/I2CClk)/2)-3)
#define DEVICE_ADR      0x01
#define COPYRIGHTSTRING "Let's samba nxt arm in arm, (c)LEGO System A/S"
#define COPYRIGHTSTRINGLENGTH 46
const   UBYTE CopyrightStr[] = {"\xCC"COPYRIGHTSTRING};
#define MAX(x,y) (((x)>(y))?(x):(y))
#define piir (*AT91C_PITC_PIIR & AT91C_PITC_CPIV)
#define pimr (*AT91C_PITC_PIMR & AT91C_PITC_CPIV)
#define DISABLEI2cIrqs *AT91C_TWI_IDR = 0x000001C7

// send/receive state machine variables
static enum state_t {HELLO, SEND, SENDING, RECEIVE, RECEIVING}; 
static enum state_t volatile State;
static enum state_t volatile nextState;

IOTOAVR volatile IoToAvr;
IOFROMAVR volatile IoFromAvr;

UBYTE volatile buffer[sizeof(IOFROMAVR) + sizeof(IOTOAVR) + COPYRIGHTSTRINGLENGTH];
UBYTE volatile buffer_index;
UBYTE buffer_len;
UBYTE volatile checksum;
UBYTE volatile firstByteFlag;
UBYTE tmp;

void DataTxInit(UBYTE *buf, UBYTE len) {
  memcpy((UBYTE*)buffer, (UBYTE*)buf, (UBYTE)len);
  buffer_len = len;
  buffer_index = 0;
  checksum = 0;
  checksum = checksum + buf[buffer_index];

  *AT91C_TWI_MMR &= ~AT91C_TWI_DADR;
  *AT91C_TWI_MMR |= (DEVICE_ADR << 16);
  *AT91C_TWI_MMR &= ~AT91C_TWI_IADRSZ;
  *AT91C_TWI_MMR |= AT91C_TWI_IADRSZ_NO;
  *AT91C_TWI_MMR &= ~AT91C_TWI_MREAD; 
  *AT91C_TWI_THR = buf[buffer_index++];
  *AT91C_TWI_CR |= AT91C_TWI_MSEN;
  *AT91C_TWI_IER |= AT91C_TWI_TXRDY;
  *AT91C_TWI_IER |= AT91C_TWI_TXCOMP;
  *AT91C_TWI_IER |= AT91C_TWI_NACK;
  *AT91C_TWI_CR |= AT91C_TWI_START;
  return;
}  

void DataRxInit(void) {
  buffer_len = BYTES_TO_RX;
  buffer_index = 0;
  checksum = 0;
  firstByteFlag = TRUE;

  *AT91C_TWI_MMR &= ~AT91C_TWI_DADR;
  *AT91C_TWI_MMR |= (DEVICE_ADR << 16);
  *AT91C_TWI_MMR &= ~AT91C_TWI_IADRSZ;
  *AT91C_TWI_MMR |= AT91C_TWI_IADRSZ_NO;
  *AT91C_TWI_MMR |= AT91C_TWI_MREAD;
  *AT91C_TWI_CR |= AT91C_TWI_MSEN;
  *AT91C_TWI_IER |= AT91C_TWI_RXRDY;
  *AT91C_TWI_IER |= AT91C_TWI_TXCOMP;
  *AT91C_TWI_IER |= AT91C_TWI_NACK;
  *AT91C_TWI_CR |= AT91C_TWI_START;
  return;
}


__ramfunc void I2cHandler(void) {

  if(State == SENDING){

    if(*AT91C_TWI_SR & AT91C_TWI_NACK){
      *AT91C_TWI_THR = buffer[buffer_index - 1];
      return;
    }

    if(buffer_index < buffer_len){                 
      checksum = checksum + buffer[buffer_index];
      *AT91C_TWI_THR = buffer[buffer_index++];
      
    }else if(buffer_index == buffer_len){         
      *AT91C_TWI_THR = ~checksum;
      buffer_index++;                                
      *AT91C_TWI_CR |= AT91C_TWI_STOP;
    }else{
      DISABLEI2cIrqs;
      while(!(*AT91C_TWI_SR & AT91C_TWI_TXCOMP));
      nextState = RECEIVE;
    }
  }

  if(State == RECEIVING){
    if(firstByteFlag){
      firstByteFlag = FALSE;
      tmp = *AT91C_TWI_RHR;
      return;
    }
    if(buffer_index < buffer_len-1){
      buffer[buffer_index] = *AT91C_TWI_RHR;
      checksum = checksum + buffer[buffer_index];
      buffer_index ++;

    }else if(buffer_index == buffer_len - 1){
      buffer[buffer_index] = *AT91C_TWI_RHR;
      checksum = checksum + buffer[buffer_index];
      buffer_index ++;
      *AT91C_TWI_CR |= AT91C_TWI_STOP;

    }else{
      if((checksum + *AT91C_TWI_RHR) != 0xFF){
        DisplayString(80, 55, (UBYTE*)"err");
        DisplayUpdateSync();
      }
      DISABLEI2cIrqs;
      while(!(*AT91C_TWI_SR & AT91C_TWI_TXCOMP));
      memcpy((UBYTE*)&IoFromAvr, (UBYTE*)buffer, (UBYTE)BYTES_TO_RX);
      nextState = SEND;
    }
  }

  return;

}

void I2CTransfer(void) {
  I2CWait();
  switch(State){
  case HELLO:
    State = SENDING;
    nextState = State;
    DataTxInit((UBYTE*)CopyrightStr, COPYRIGHTSTRINGLENGTH);
    break;

  case SEND:
    State = SENDING;
    nextState = State;
    DataTxInit((UBYTE*)&IoToAvr, BYTES_TO_TX);
    break;

  case SENDING:
    State = nextState;
    break;

  case RECEIVE: 
    State = RECEIVING;
    nextState = State;
    DataRxInit();
    break;

  case RECEIVING:
    State = nextState;
    break;  
    
  default:
    break;
  }
  return;
}

void I2CCtrl (enum power_t p) {
  switch(p){
    case POWERDOWN:
      IoToAvr.Power = 0x5A;
      IoToAvr.PwmFreq = 0x00;
      break;
    case REPROGRAM:
      IoToAvr.Power = 0xA5;
      IoToAvr.PwmFreq = 0x5A;
      break;
  }
  return;
}

#define WAITClk(t) {\
	  ULONG pit = piir + (t);\
          if (pit >= pimr) pit -= pimr;\
          while (piir < pit){;}\
        }

void I2CInit(void) { 
  //
  // disable I2C on PIO
  // this is called also during an error, so interrupts etc may be enabled
  //
  *AT91C_AIC_IDCR = (1L<<AT91C_ID_TWI);			/* disable AIC irq  */
  DISABLEI2cIrqs;                      			/* disable TWI irq  */
  *AT91C_PMC_PCER  = (1L<<AT91C_ID_TWI);		/* enable TWI Clock */
  *AT91C_PIOA_OER  = AT91C_PA4_TWCK;  		  	/* SCL is output    */
  *AT91C_PIOA_ODR  = AT91C_PA3_TWD;			/* SDA is input     */
  *AT91C_PIOA_MDER = (AT91C_PA4_TWCK | AT91C_PA3_TWD);  /* open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  // 
  // synch I2C clocks using PIO
  // generate a 400KHz pulse on SCK and wait until both SCK and SDA are high, 
  // which means the slave ticks with this clock
  //
  *AT91C_PIOA_PER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* enable PIO control for these pins */
  while(((*AT91C_PIOA_PDSR & AT91C_PA3_TWD) == 0) || ((*AT91C_PIOA_PDSR & AT91C_PA4_TWCK) == 0)){
      *AT91C_PIOA_CODR = AT91C_PA4_TWCK; /* drive SCL Low  */
      WAITClk(TIME400KHz);
      *AT91C_PIOA_SODR = AT91C_PA4_TWCK; /* drive SCL High */
      WAITClk(TIME400KHz);
  }
  // 
  // init I2C on PIO
  //
  *AT91C_TWI_CR    =  AT91C_TWI_SWRST;			/* this has to happen before the rest */
  *AT91C_PIOA_PDR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* disable PIO control for these pins */
  *AT91C_PIOA_ASR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* select peripheral A = TWI */
  *AT91C_TWI_CWGR   = (CLDIV | (CLDIV << 8));           /* 400KHz clock    */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* clear AIC irq   */
  AT91C_AIC_SVR[AT91C_ID_TWI] = (unsigned int)I2cHandler;
  AT91C_AIC_SMR[AT91C_ID_TWI] = ((AT91C_AIC_PRIOR_HIGHEST) | (AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED));
  *AT91C_AIC_IECR   = (1L<<AT91C_ID_TWI);               /* Enables AIC irq */

  IoToAvr.Power     = 0;
  State = HELLO;

  return;
}

void I2CExit(void) {
  DISABLEI2cIrqs;
  *AT91C_AIC_IDCR   = (1L<<AT91C_ID_TWI);               /* Disable AIC irq  */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* Clear AIC irq    */
  *AT91C_PMC_PCDR   = (1L<<AT91C_ID_TWI);               /* Disable clock    */
  *AT91C_PIOA_MDER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  *AT91C_PIOA_PER   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Disable peripheal*/
}
