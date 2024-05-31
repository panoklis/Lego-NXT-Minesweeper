#include "colour_sight.h"
#include "arm2avr.h"

#define ERROR_MARGIN  20

void store_colour(enum colour colour, UWORD sensor_value){
	stored_colour_values[colour]=sensor_value;	
	return;
}

enum colour get_colour(){
	UWORD value = IoFromAvr.AdValue[0];
	
	for (enum colour colour = unset+1 ;colour <NO_OF_COLOURS;colour++){
		int colour_diff = value - stored_colour_values[colour];
		if(colour_diff <= ERROR_MARGIN && colour_diff >= -ERROR_MARGIN )return colour;
	}	
	return unset;
	}

void colour_sight_init(){

	for (enum colour colour = unset+1 ;colour <NO_OF_COLOURS;colour++)
		stored_colour_values[colour]=unset;
}
