#ifndef _MOVEMENT_H_
#define _MOVEMENT_H_

#include <stdconst.h>

/* 			tank controls
 * It is assumed that the right wheel motor is connected to output B
 * And the left one to output C
 *
 * motors wont move after flashing
 * but they will after a single reset (?)
 */
enum Movement{ahead=0,right=1,left=2};

//simply writes the IOToAvr struct 
//use every i2c transfer for continuous movement
void momentary_move(enum Movement c, SBYTE speed);

//void move_1_unit(enum movement c);

//void movement_board();

#endif
