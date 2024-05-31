#include"movement.h"
#include"output.h"
#include "i2c.h"


//	made some measurements and this seems to be how many momentary moves
//	push the brick ahead for 1 time it's length with speed of UNIT_SPEED
//			         v
#define AHEAD_UNIT_MOVES	950
#define ROTATE_UNIT_MOVES	565
#define UNIT_SPEED		60

void momentary_move(enum Movement c , SBYTE speed){

	static SBYTE moving=0;
	
	
	
	switch(c){
		case ahead :
			OutputSetSpeed(RWHEEL, speed);
          		OutputSetSpeed(LWHEEL, speed);
          		moving=speed;
          		break;
		case right :
			if(moving) speed = moving>0? 100 : -100;
			OutputSetSpeed(LWHEEL, speed);
			break;
		case left  :
          		if(moving) speed = moving>0? 100 : -100;
          		OutputSetSpeed(RWHEEL, speed);
			break;
	}
}

void unit_move(enum Movement c){

	switch(c){
		case ahead :
			OutputSetSpeed(LWHEEL, UNIT_SPEED);
          		OutputSetSpeed(RWHEEL, UNIT_SPEED+2);		//my right motor is a bit weaker
			for(int i =0; i < AHEAD_UNIT_MOVES;i++) I2CTransfer();
          		break;
		case right :
			OutputSetSpeed(LWHEEL, UNIT_SPEED);
          		OutputSetSpeed(RWHEEL, -UNIT_SPEED);
			for(int i =0; i < ROTATE_UNIT_MOVES;i++)I2CTransfer();
			break;
		case left  :
			OutputSetSpeed(RWHEEL, UNIT_SPEED);
          		OutputSetSpeed(LWHEEL, -UNIT_SPEED);
			for(int i =0; i < ROTATE_UNIT_MOVES;i++) I2CTransfer();
          		break;
	}
	OutputSetSpeed(RWHEEL, 0);
        OutputSetSpeed(LWHEEL, 0);
	I2CTransfer();
	I2CTransfer();
}


