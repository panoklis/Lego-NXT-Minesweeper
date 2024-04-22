#include"movement.h"
#include"output.h"

#define RWHEEL 2
#define LWHEEL 3

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



