#include"sound_com.h"
#include"movement.h"
#include"i2c.h"
#include"arm2avr.h"
#include"display.h"
#include"timer.h"
#include"sound.h"
#include "button.h"

#define NOISE_MARGIN 5

static UWORD silence=0;


UWORD median_sound(UWORD * arr , int length);

void print_msg(enum Com_cmd * msg){
	
	char str_msg[100]={0};
	
	for(int i =0 ; i < 99; i ++){
		str_msg[i]= '0'+ msg[i];
	}	
	DisplayString(0,0, str_msg);
	DisplayUpdateSync();

}

enum Com_cmd sound_get_cmd(){
	unsigned int frst_bit , snd_bit;
		frst_bit=2;
		while(frst_bit==2) frst_bit = sound_to_bit2();
		
		snd_bit=2;
		while(snd_bit==2)snd_bit=sound_to_bit2();
			
		return (frst_bit | (snd_bit << 1));
		
}

enum Com_cmd * listen(){			
	
	


	enum Com_cmd * msg = malloc(100 * sizeof (enum Com_cmd));
	msg[99]=com_go;

	UWORD sound;
	long long int sum=0;
	unsigned int frst_bit ,snd_bit ,i=0;
	
	enum Com_cmd current_cmd=com_right;
	
	for(int j =0; j <10 ; j++){	//set the sound of relative silence
		I2CTransfer();
		sound=IoFromAvr.AdValue[MIC];
		sum+=sound;
	}
	silence = sum/10;
	sum=0; 
	
	//while(1) {DisplayErase();DisplayNum(10,10 ,sound_to_bit2()+1);DisplayNum(10,30 ,sum++);DisplayUpdateSync();}
	

	while(1){
		current_cmd = sound_get_cmd();
		msg[i++] = current_cmd;
		if(current_cmd== com_go || i > 98) {print_msg(msg); return msg;}
	}	
}

//spin untill somebody starts talking
void listen_till_no_silence(){
		int sum=0;
		UWORD sound;
	
	do{	
		sum=0;	
		for(int i =0 ;i <10 ; i ++){
			I2CTransfer();
			sound=IoFromAvr.AdValue[MIC];
			sum+= sound;
			}
		
	}while(sum/10 + NOISE_MARGIN >= silence);

}





unsigned int sound_to_bit2(){	//500 ms click is 0 1000 ms click is 1
	int sum=0;
	UWORD sound;
	UWORD sample[BIT_DURATION/2];
	
	listen_till_no_silence();
		
	//something has been heard	
	for(int i =0 ;i <BIT_DURATION/2 -10 ; i ++){
		I2CTransfer();
		sound=IoFromAvr.AdValue[MIC];
		sample[i]= sound;
	}	
	//500ms have passed , was it just noise? 
	if( median_sound(sample, sizeof(sample)) + NOISE_MARGIN >= silence) return 2;	// silence

     	//so it has been ticking for 500ms
     	sum=0;
      	for(int i =0 ;i <BIT_DURATION/2 ; i ++){
		I2CTransfer();
		sound=IoFromAvr.AdValue[MIC];
		sample[i]= sound;
	}	
	//1000ms have passed now 
	if( median_sound(sample, sizeof(sample)) + NOISE_MARGIN >= silence ) 
		return 0;						//500ms click then 500ms silence
     	else	
     		return 1;						//1000ms click		
		
		
	
}


void execute_msg(enum Com_cmd * msg){
	enum Com_cmd curr= msg[0];
	int i=1;
	while(i!= com_go && i <=3){unit_move ((enum Movement) sound_get_cmd()); i++;}
	free(msg);
	}


void send_cmd(enum Com_cmd cmd){
	
	/*
	while(1) {send_bit(0); Sleep(500);send_bit(1); Sleep(500);}

	send_bit(0);Sleep(500);send_bit(0);send_bit(0);Sleep(100);send_bit(0);send_bit(0);Sleep(100);send_bit(0);send_bit(0);Sleep(100);send_bit(0);send_bit(0);Sleep(100);send_bit(0);
	send_bit(0);send_bit(0);send_bit(0);send_bit(0);send_bit(0);send_bit(0);
	send_bit(0);send_bit(0);send_bit(0);send_bit(0);send_bit(0);send_bit(0);
	send_bit(1);send_bit(0);send_bit(1);send_bit(0);send_bit(1);send_bit(0);
	send_bit(1);send_bit(0);send_bit(1);send_bit(0);send_bit(1);send_bit(0);
	send_bit(1);send_bit(0);send_bit(1);send_bit(0);send_bit(1);send_bit(0);			
	return;
	*/
	
	
	
	switch(cmd){
		case com_ahead :
			send_bit(com_ahead & 0x1);
			Sleep(500);
			send_bit( (com_ahead & 0x2) !=0);
			break;
		case com_right :
			send_bit(com_right & 0x1);
			Sleep(500);
			send_bit( (com_right & 0x2) !=0 );
			break;
		case com_left :
			send_bit(com_left & 0x1);
			Sleep(500);
			send_bit( (com_left & 0x2) != 0);
			break;
		case com_go :
			send_bit(com_go & 0x1);
			Sleep(500);
			send_bit( (com_go & 0x2) != 0);
			break;	
		default : return;
	}

}
		//rough estimates by feel
void send_bit(unsigned short b){
	ULONG pattern[] = {0xFFFF0000};
	
	#define sound_sync_1_s 180
	
	switch (b) {
		case 0 :
				//500ms click
			for(int i =0 ; i < sound_sync_1_s/2 ; i ++){
				SoundSync(pattern, sizeof(pattern), 255, 1);	//SoundASync doesnt work
				I2CTransfer();
			}
				//500ms silence
				Sleep(500);
			break;
		case 1 :
			for(int i =0 ; i < sound_sync_1_s ; i ++){
				SoundSync(pattern, sizeof(pattern), 255, 1);	//SoundASync doesnt work
				I2CTransfer();
			}
			break;
		default : Sleep(1000);
	}
	
	


}



int comp (const void * elem1, const void * elem2) 
{
    UWORD f = *((UWORD*)elem1);
    UWORD s = *((UWORD*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
}
				   //v wtf, this is not length
UWORD median_sound(UWORD * arr , int n){
	qsort(arr, n/sizeof(UWORD), sizeof(UWORD), comp);
	return arr[BIT_DURATION/4];
}

void controller();
void slave();

void sound_com_demo(enum Com_demo role){
	switch(role){
		case sound_controller :
			controller();
			break;
		case sound_slave :
			slave();
			break;
	}
}

void controller(){
  while(1){
    I2CTransfer();
    enum button_t button = ButtonRead();
    switch (button) {
      case BUTTON_LEFT:
        send_cmd(com_left);
        Sleep(500);
        continue;
      case BUTTON_RIGHT:
         send_cmd(com_right);
         Sleep(500);
         continue;
      case BUTTON_ENTER:
        send_cmd(com_ahead);
        Sleep(500);
        continue;
        default:
        continue;
    }
  }
}
void slave(){
	enum Movement i;
	long long int sum=0;
	UWORD sound;
	
	for(int j =0; j <10 ; j++){	//set the sound of relative silence
		I2CTransfer();
		sound=IoFromAvr.AdValue[MIC];
		sum+=sound;
	}
	silence = sum/10;
	
	while(1) unit_move(sound_get_cmd());
}

