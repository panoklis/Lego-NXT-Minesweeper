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
    OutputInit();
    while(1){
      I2CTransfer();
      button = ButtonRead();
      switch (button){
        case BUTTON_LEFT:
          if (speed == -100){
            speed = 100;
          }
          else{
            speed--;
          }
          DisplayString(0,0,"Speed: ");
          DisplayChar(48,0,speed<0 ? '-' : ' ');
          if (speed < 0) {tmp_speed = -speed;}
          else {tmp_speed = speed;}
          DisplayChar(54,0,tmp_speed>99 ? '1' : '0');
          DisplayChar(60,0,tmp_speed>9 ? '0' + (tmp_speed/10 - tmp_speed/100*10) : ' ');
          DisplayChar(66,0,'0' + tmp_speed%10);
          DisplayUpdateSync();
          break;
        case BUTTON_RIGHT:
          if (speed == 100){
            speed = -100;
          }
          else{
            speed++;
          }
          DisplayString(0,0,"Speed: ");
          DisplayChar(48,0,speed<0 ? '-' : ' ');
          if (speed < 0) {tmp_speed = -speed;}
          else {tmp_speed = speed;}
          DisplayChar(54,0,tmp_speed>99 ? '1' : '0');
          DisplayChar(60,0,tmp_speed>9 ? '0' + (tmp_speed/10 - tmp_speed/100*10) : '0');
          DisplayChar(66,0,'0' + tmp_speed%10);
          DisplayUpdateSync();
          break;
        case BUTTON_ENTER:
          OutputSetSpeed(1, speed);
          OutputSetSpeed(2, speed);
          OutputSetSpeed(3, speed);
          OutputSetSpeed(4, speed);
          break;
        case BUTTON_EXIT:
          OutputSetSpeed(1, 0);
          OutputSetSpeed(2, 0);
          OutputSetSpeed(3, 0);
          OutputSetSpeed(4, 0);
          break;
      }
    }
  }

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

  DisplayString(30,16,"LEFT: SoundBoard");
  DisplayString(30,32,"RIGHT: MotorBoard");
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
