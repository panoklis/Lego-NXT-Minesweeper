#include <stdlib.h>
#include <assert.h>
#include "AT91SAM7S256.h"
#include "hwinit.h"
#include "pit.h"
#include "aic.h"
#include "display.h"
#include "sound.h"
#include "i2c.h"
#include "input.h"
#include "button.h"
#include "output.h"
#include "led.h"
#include "aclock.h"
#include "timer.h"
#include "movement.h"
#include "arm2avr.h"
#include "colour_sight.h"

void sound_board(){
    ULONG pattern1[] = {0xFFFFFFFF}; // 1111 1111 1111 1111
    ULONG pattern2[] = {0xFFFF0000}; // 1111 1111 0000 0000
    ULONG pattern3[] = {0xF0F0F0F0}; // 1111 0000 1111 0000
    ULONG pattern4[] = {0xCCCCCCCC}; // 1100 1100 1100 1100
    ULONG pattern5[] = {0xAAAAAAAA}; // 1010 1010 1010 1010
    ULONG pattern6[] = {
                          0xFFFFFFFF, 0xFFFFFFFF}; // 16b ffff

    UBYTE volatile rate = 0x00;
    UBYTE volatile pattern = 0x00;
    enum button_t old_button = BUTTON_NONE;

    while(1) {
      I2CTransfer();
      enum button_t button = ButtonRead();
      if (old_button == button && button != BUTTON_NONE && button != BUTTON_ENTER) {
        continue;
      }
      switch(button) {
        case BUTTON_LEFT:
          if (rate == 0x00){
            rate = 0xFF;
          }
          else{
            rate--;
          }
          DisplayString(0,0,"Rate: ");
          DisplayChar(30,0,rate>99 ? '0' + rate/100 : ' ');
          DisplayChar(36,0,rate>9 ? '0' + rate/10 - rate/100*10 : ' ');
          DisplayChar(42,0,'0' + rate%10);
          DisplayUpdateSync();
          //I2CSleep(100);
          break;
        case BUTTON_ENTER:
          switch (pattern) {
            case 0x00:
              SoundSync(pattern1, sizeof(pattern1), rate, 1);
              break;
            case 0x01:
              SoundSync(pattern2, sizeof(pattern2), rate, 1);
              break;
            case 0x02:
              SoundSync(pattern3, sizeof(pattern3), rate, 1);
              break;
            case 0x03:
              SoundSync(pattern4, sizeof(pattern4), rate, 1);
              break;
            case 0x04:
              SoundSync(pattern5, sizeof(pattern5), rate, 1);
              break;
            case 0x05:
              SoundSync(pattern6, sizeof(pattern6), rate, 1);
              break;
          }
          break;
        case BUTTON_RIGHT:
          if (rate == 0xFF){
            rate = 0x00;
          }
          else{
            rate++;
          }
          DisplayString(0,0,"Rate: ");
          DisplayChar(30,0,rate>99 ? '0' + rate/100 : ' ');
          DisplayChar(36,0,rate>9 ? '0' + rate/10 - rate/100*10 : ' ');
          DisplayChar(42,0,'0' + rate%10);
          DisplayUpdateSync();
          //I2CSleep(100);
          break;
        case BUTTON_EXIT:
          if (pattern == 0x05){
            pattern = 0x00;
          }
          else{
            pattern++;
          }
          switch (pattern) {
            case 0x00:
              DisplayString(0,8,"PTRN1:FFFF-FFFF");
              break;
            case 0x01:
              DisplayString(0,8,"PTRN2:FFFF-0000");
              break;
            case 0x02:
              DisplayString(0,8,"PTRN3:F0F0-F0F0");
              break;
            case 0x03:
              DisplayString(0,8,"PTRN4:CCCC-CCCC");
              break;
            case 0x04:
              DisplayString(0,8,"PTRN5:AAAA-AAAA");
              break;
            case 0x05:
              DisplayString(0,8,"PTRN6:1B-0B");
              break;
          }
          DisplayUpdateSync();
          //I2CSleep(250);
          break;
        case BUTTON_NONE:
          break;
      }
      old_button = button;
    }
  }

  void motor_board(){
    SBYTE volatile speed = 0x00;
    UBYTE volatile tmp_speed = 0x00;
    enum button_t button = BUTTON_NONE;
    DisplayString(0,8,"DRIVE ME");
    DisplayUpdateSync();
    OutputInit();
    while(1){
      I2CTransfer();
      button = ButtonRead();
      switch (button){
        case BUTTON_LEFT:
         momentary_move(left , 50);
          break;
        case BUTTON_RIGHT:
          momentary_move(right , 50);
          break;
        case BUTTON_ENTER:
         momentary_move(ahead , 60);
          break;
        case BUTTON_EXIT:
          momentary_move(ahead , -50);
          break;
          case BUTTON_NONE:
          	momentary_move(ahead , 0);
      }
    }
  }

void light_board();

int main(void) {
  DisplayInit(); 
  DisplayUpdateSync();
  LedSwitchOn(2);
  HardwareInit(); // need this to init PIOA clock
  PITEnable();
  AICInit();
  SoundInit();
  I2CInit();
  InputInit();
  ButtonInit();
  OutputInit();
  StartTimer();
  LED(0,0);
  DisplayString(30,0,"LEFT: SoundBoard");
  DisplayString(30,16,"RIGHT: MotorBoard");
  DisplayString(30,32,"EXIT: LightBoard");
  DisplayUpdateSync();
  while(1){
    I2CTransfer();
    enum button_t button = ButtonRead();
    switch (button) {
      case BUTTON_LEFT:
        DisplayErase();
        DisplayUpdateSync();
        sound_board();
        break;
      case BUTTON_RIGHT:
        DisplayErase();
        DisplayUpdateSync();
        motor_board();
        break;
        case BUTTON_ENTER:
        DisplayErase();
        DisplayUpdateSync();
        motor_board();
        break;
        case BUTTON_EXIT:
        DisplayErase();
        DisplayUpdateSync();
        light_board();
        break;
    }
  }

  ButtonExit();
  InputExit();
  I2CExit();
  PITInterruptDisable();
  PITDisable();
  SoundExit();
  DisplayExit();

  return 0;
}

void light_board(){
	enum colour x;
	while(1){
		I2CTransfer();
		DisplayString(30,32,"ENTER: Store colour");				//store light value
		DisplayNum(10,10,IoFromAvr.AdValue[0]);
		DisplayUpdateSync();
		
		if (ButtonRead() == BUTTON_ENTER){
			store_colour(black, IoFromAvr.AdValue[0]);
			break;			
		}
	}
	DisplayErase();
	DisplayString(30,32,"ENTER: Search colour");
	DisplayUpdateSync();
	
	for(int i =0 ; i< 250 ;i ++) I2CTransfer(); //let ENTER button go
	
	while (1){
		I2CTransfer();
		if (ButtonRead() == BUTTON_ENTER) break;
	}
	DisplayErase();									//search 4 light value
	while(1){									//move straight till found
		I2CTransfer();
		DisplayString(30,32,"searching 4 colour");
		DisplayNum(10,10,IoFromAvr.AdValue[0]);
		DisplayUpdateSync();
		 x = get_colour();
		if(x!= unset) break;
		momentary_move(ahead , 60);
	}
	momentary_move(ahead , 0);
	I2CTransfer();
	I2CTransfer();
	
	DisplayErase();
	if(x== black)
		DisplayString(30,32,"black");
	DisplayUpdateSync();
		
	while(1);
	
}

