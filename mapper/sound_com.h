#ifndef _SOUND_COM_H_
#define _SOUND_COM_H_

//one way sound communication betwwen the two lego bricks


//the listener is assumed to be executing listen()
//when the speaker is executing send_msg()
//each time an enum Com_cmd is sent, 1 bit at a time, over sound
//BIT_DURATION ms click is 0
//2 * BIT_DURATION ms click is 1


//mic should be connected on port '1'
#define MIC 0
//500 ms click is bit 0
//1000 ms click is bit 1	
#define BIT_DURATION 1000
			//500


enum Com_cmd {com_ahead=0 ,com_right=1,com_left=2, com_go=3};

//litens over i2c and returns an array with the commands recieved 
//after a single com_go command is recieved
enum Com_cmd * listen();

void execute_msg(enum Com_cmd msg[100]);

//spins on I2C untill it listens to a bit
//might return 2 if it listens to noise
//generally consistent :/
unsigned int sound_to_bit2();

//should be called as such : send_bit(x); Sleep(500); send_bit(x); etc
//makes appropriate sound for 1 second
void send_bit(unsigned short b);

//sends appropriate bits
//not consistent if less than 500ms stand between 2 send_cmd(x) use like sound_to_bit2() 
void send_cmd(enum Com_cmd cmd);

enum Com_demo {sound_controller=0 , sound_slave=1};

void sound_com_demo(enum Com_demo role);
#endif

