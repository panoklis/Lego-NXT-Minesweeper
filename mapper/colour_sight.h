#ifndef _COLOUR_H_
#define _COLOUR_H_

#define NO_OF_COLOURS 2

#include <stdconst.h>

//assume light sensor connected to port '1' (port 0) of the brick

enum colour {unset=-1, black ,red}; //extend as needed

static UWORD stored_colour_values[NO_OF_COLOURS];

void colour_sight_init();

void store_colour(enum colour colour, UWORD sensor_value);

enum colour get_colour();

#endif
