#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_

#include <stdconst.h>

/* 			tank controls
 * It is assumed that the right wheel motor is connected to output B
 * And the left one to output C
 *
 * motors wont move after flashing
 * but they will after a single reset (?)
 * brick is 21 cm in length
 */
		//2
		//3
#define RWHEEL 2 
#define LWHEEL 3

enum Movement{ahead=0,right=1,left=2};

//simply writes the IOToAvr struct 
//every i2c transfer will make the last momentary move
void momentary_move(enum Movement c, SBYTE speed);

//synchronised
//this function makes i2c transfers to move the brick
//90 degrees left/right or 1 unit ahead
//then returns
//if small_scale_mode is not 0 then ahead move is shorter
void unit_move(enum Movement c , int small_scale_mode);

//void movement_board();

#endif
