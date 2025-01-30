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
#include "sound_com.h"

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


enum MenuOptions{MAZERUNNER=0,REMOTECONTROLLED=1,REMOTECONTROLLER=2,COLOURCONFIG=3};
void light_board();
void controlled_board();
void controller_board();
void maze_board();
void clear_cursor(enum MenuOptions option){  DisplayString(90, 16*option," "); }
void print_menu(enum MenuOptions option){
 
  DisplayString(0,0,"Maze Runner");
  DisplayString(0,16,"Rmt Cntrled");
  DisplayString(0,32,"Remote Cntrler");
  DisplayString(0,48,"Colour Config");
  DisplayString(90, 16*option,"<");
  DisplayUpdateSync();
}
void dance();
//void beep(){for(int i =0; i<150;i++) SoundSync(0xFFFFFFFF, sizeof(ULONG),0x1A, 1);}
int main(void) {
  enum MenuOptions option=MAZERUNNER;
  void (*options[4])(void);	
  options[MAZERUNNER]=maze_board;//maze_board;
  options[REMOTECONTROLLED]=controlled_board;
  options[REMOTECONTROLLER]=controller_board;
  options[COLOURCONFIG]=light_board;
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
  print_menu(option); 
while(1){
    I2CTransfer();
    enum button_t button = ButtonRead();
    switch (button) {
      case BUTTON_LEFT:
	clear_cursor(option);
      	option= --option % 4;
	print_menu(option);	
        I2CSleep(250);
	break;
      case BUTTON_RIGHT:
	clear_cursor(option);
      	option= ++option % 4;
	print_menu(option);
        I2CSleep(250);
	break;
      case BUTTON_ENTER:
        DisplayErase();
        DisplayUpdateSync();
        I2CSleep(250);
        options[option]();
        DisplayErase();
        DisplayUpdateSync();	
	print_menu(option);
        break;
      case BUTTON_EXIT:
	break;
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

void controller_board(){
	while(1){
		I2CTransfer();
		switch(ButtonRead()){
			case BUTTON_RIGHT :
				send_bit(1);
				break;
			case BUTTON_LEFT :
				send_bit(0);
				break;
			case BUTTON_ENTER :
				break;
			case BUTTON_EXIT :
        			I2CSleep(250);
				return;
		}
	}
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
        DisplayUpdateSync(); 
        I2CSleep(250);
	return;
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

void listen_board(){ sound_com_demo(sound_slave);}
void display_next_movement_cmd(enum Movement c, int ordinal);
void maze_board(){
	enum Movement commands[100];
	int i=0,c=0;
	while(!c){
		I2CTransfer();
		switch(ButtonRead()){
			case BUTTON_RIGHT :
				display_next_movement_cmd(left, i);
        			commands[i++]=left;
				I2CSleep(700);
				break;
			case BUTTON_LEFT :
				display_next_movement_cmd(right, i);
        			commands[i++]=right;
				I2CSleep(700);	
				break;
			case BUTTON_ENTER :
				display_next_movement_cmd(ahead, i);
        			commands[i++]=ahead;
        			I2CSleep(700);
				break;
			case BUTTON_EXIT :
				c=1;
				
		}

	}
	
	for(int j =0;j<i;j++)
		if (! unit_move(commands[j],0)) {
			send_bit(0);
			return;
		}
	return;
}

void display_next_movement_cmd(enum Movement c, int ordinal){
	switch(c){
		case right :
		  DisplayString((ordinal%12)*8, 8*(ordinal/12), "<, ");
			break;
		case left:
		  DisplayString((ordinal%12)*8, 8*(ordinal/12), ">, ");
			break;
		case ahead :
		  DisplayString((ordinal%12)*8, 8*(ordinal/12), "^, ");	
			break;
	}	
	DisplayUpdateSync();	
}
void controlled_board(){sound_com_demo(sound_slave);}
void dance(){

#define SMALL_SCALE_F 0
//unit_move(ahead , SMALL_SCALE_F); return;


unit_move(right, SMALL_SCALE_F);

unit_move(left , SMALL_SCALE_F);


while(1){
unit_move(ahead , SMALL_SCALE_F);

unit_move(right, SMALL_SCALE_F); 
unit_move(right, SMALL_SCALE_F);

unit_move(ahead, SMALL_SCALE_F);

unit_move(left, SMALL_SCALE_F);
unit_move(left, SMALL_SCALE_F);

}
}
