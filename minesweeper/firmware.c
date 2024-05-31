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
#include "sound_com.h"

void sound_board() {
	ULONG pattern1[] = {
		0xFFFFFFFF
	}; // 1111 1111 1111 1111
	ULONG pattern2[] = {
		0xFFFF0000
	}; // 1111 1111 0000 0000
	ULONG pattern3[] = {
		0xF0F0F0F0
	}; // 1111 0000 1111 0000
	ULONG pattern4[] = {
		0xCCCCCCCC
	}; // 1100 1100 1100 1100
	ULONG pattern5[] = {
		0xAAAAAAAA
	}; // 1010 1010 1010 1010
	ULONG pattern6[] = {
		0xFFFFFFFF,
		0xFFFFFFFF
	}; // 16b ffff

	UBYTE volatile rate = 0x00;
	UBYTE volatile pattern = 0x00;
	enum button_t old_button = BUTTON_NONE;
	int exitflag = 0;
	while (1) {
		I2CTransfer();
		enum button_t button = ButtonRead();
		if (old_button == button && button != BUTTON_NONE && button != BUTTON_ENTER) {
			continue;
		}
		switch (button) {
			case BUTTON_LEFT:
				if (pattern == 0x05) {
					pattern = 0x00;
				} else {
					pattern++;
				}
				switch (pattern) {
					case 0x00:
						DisplayString(0, 8, "PTRN1:FFFF-FFFF");
						break;
					case 0x01:
						DisplayString(0, 8, "PTRN2:FFFF-0000");
						break;
					case 0x02:
						DisplayString(0, 8, "PTRN3:F0F0-F0F0");
						break;
					case 0x03:
						DisplayString(0, 8, "PTRN4:CCCC-CCCC");
						break;
					case 0x04:
						DisplayString(0, 8, "PTRN5:AAAA-AAAA");
						break;
					case 0x05:
						DisplayString(0, 8, "PTRN6:1B-0B");
						break;
				}
				DisplayUpdateSync();
				//I2CSleep(250);
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
				if (rate == 0xFF) {
					rate = 0x00;
				} else {
					rate++;
				}
				DisplayString(0, 0, "Rate: ");
				DisplayChar(30, 0, rate > 99 ? '0' + rate / 100 : ' ');
				DisplayChar(36, 0, rate > 9 ? '0' + rate / 10 - rate / 100 * 10 : ' ');
				DisplayChar(42, 0, '0' + rate % 10);
				DisplayUpdateSync();
				//I2CSleep(100);
				break;
			case BUTTON_EXIT:
				DisplayErase();
				DisplayUpdateSync();
				exitflag = 1;
				//I2CSleep(250);
				break;
			case BUTTON_NONE:
				break;
		}
		old_button = button;
		if (exitflag == 1) break;
	}
}

void motor_board() {
	int exitflag = 0;
	SBYTE volatile speed = 0x00;
	UBYTE volatile tmp_speed = 0x00;
	enum button_t button = BUTTON_NONE;
	OutputInit();
	int pressedflag = 0;
	while (1) {
		I2CTransfer();
		button = ButtonRead();
		switch (button) {
			case BUTTON_LEFT:
				LedBlink(1);
				if (speed == -100) {
					speed = 100;
				} else {
					speed--;
				}
				DisplayString(0, 0, "Speed: ");
				DisplayChar(48, 0, speed < 0 ? '-' : ' ');
				if (speed < 0) {
					tmp_speed = -speed;
				} else {
					tmp_speed = speed;
				}
				DisplayChar(54, 0, tmp_speed > 99 ? '1' : '0');
				DisplayChar(60, 0, tmp_speed > 9 ? '0' + (tmp_speed / 10 - tmp_speed / 100 * 10) : ' ');
				DisplayChar(66, 0, '0' + tmp_speed % 10);
				DisplayUpdateSync();

				break;
			case BUTTON_RIGHT:
				LedBlink(1);
				if (speed == 100) {
					speed = -100;
				} else {
					speed++;
				}
				DisplayString(0, 0, "Speed: ");
				DisplayChar(48, 0, speed < 0 ? '-' : ' ');
				if (speed < 0) {
					tmp_speed = -speed;
				} else {
					tmp_speed = speed;
				}
				DisplayChar(54, 0, tmp_speed > 99 ? '1' : '0');
				DisplayChar(60, 0, tmp_speed > 9 ? '0' + (tmp_speed / 10 - tmp_speed / 100 * 10) : '0');
				DisplayChar(66, 0, '0' + tmp_speed % 10);
				DisplayUpdateSync();
				break;
			case BUTTON_ENTER:
				LedBlink(1);
				//	  if(pressedflag==1)break;
				pressedflag = 1;
				OutputSetSpeed(1, speed);
				OutputSetSpeed(2, speed);
				OutputSetSpeed(3, speed);
				OutputSetSpeed(4, speed);

				break;
			case BUTTON_EXIT:
				LedBlink(1);
				OutputSetSpeed(1, 0);
				OutputSetSpeed(2, 0);
				OutputSetSpeed(3, 0);
				OutputSetSpeed(4, 0);
				//	  I2CTransfer();
				exitflag = 1;
				break;
			case BUTTON_NONE:
				//	  pressedflag=0;
				//	 // I2CTransfer();
				break;
		}
		if (exitflag == 1) {
			break;
		}
	}
}
void showInputs() {
	UWORD volatile value;
	enum button_t button;

	while (1) {
		I2CTransfer();
		button = ButtonRead();
		if (button == BUTTON_EXIT) {
			LedBlink(1);
			break;
		}
		DisplayErase();
		I2CTransfer();
		DisplayString(0, 0, "Touch: ");
		getTouch( & value);
		DisplayNum(0, 7, value);
		DisplayString(0, 14, "Ultrasonic 4: ");
		getUltrasonic( & value, 4);
		DisplayNum(0, 21, value);
		DisplayString(0, 28, "Light: ");
		getLight( & value);
		DisplayNum(0, 35, value);
		DisplayString(0, 42, "Ultrasonic 2: ");
		getUltrasonic( & value, 2);
		DisplayNum(0, 49, value);
		DisplayUpdateSync();
	}
}
 int I2CSleepWithUv(int ms){
	volatile int tempms = 0;
	volatile UWORD uvValue=0;
	volatile UWORD thirdbutton;
	while(tempms<ms){
		I2CSleep(24);
		tempms+=24;
		getLight(&uvValue);
		
			
								if (uvValue == 1) {
									OutputSetSpeed(1, 0);
									OutputSetSpeed(2, 0);
                  							OutputSetSpeed(3, 0);
									I2CTransfer();
									volatile int lightFlag = 0;
									I2CSleep(1000);
									while (1) {
										I2CTransfer();
										getLight( & uvValue);
										thirdbutton = ButtonRead();
										if (thirdbutton == BUTTON_EXIT) return 1;
										if (uvValue == 0) lightFlag = 1;
										if (lightFlag = 1 && uvValue == 1)break;

									}
								}
		
		
	}
	return 0;
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
	DisplayString(0, 0, "THIS TIME IT WORKS!!");
	DisplayUpdateSync();
	Sleep(1000);
	LedSwitchOff(2);
	int pressedFlag = 0;
	//int pressedFlag2=0;
	int exitflag = 0;
	enum button_t button;
	enum button_t secondbutton;
	enum button_t thirdbutton;
	UWORD volatile value_u1 = 0, value_u2 = 0;
	UWORD volatile value2 = 0;
	volatile int count_u1 =0,count_u2=0,prev_state=0;
	while (1) {
		DisplayErase();
		DisplayString(10, 16, "LEFT: SoundCommands");
		DisplayString(10, 32, "RIGHT: MotorBoard");
		DisplayString(10, 48, "ORANGE: ShowInputs");
		DisplayUpdateSync();

		I2CTransfer();
		// logInputs("sensorlog.txt");
		button = ButtonRead();
		switch (button) {
			case BUTTON_LEFT:
				if (pressedFlag) break;
				LedBlink(2);
				//LedSwitchOn(0);
				//I2CSleep(300);
				//LedSwitchOff(0);

				pressedFlag = 1;
				DisplayErase();
				DisplayUpdateSync();
				//sound_board();
        sound_com_demo(0); //controller
				break;
			case BUTTON_RIGHT:
				if (pressedFlag) break;
				LedBlink(2);
				pressedFlag = 1;
				DisplayErase();
				DisplayUpdateSync();
				motor_board();
				break;
			case BUTTON_ENTER:
				if (pressedFlag) break;
				LedBlink(2);
				pressedFlag = 1;
				DisplayErase();
				DisplayUpdateSync();
				showInputs();
				break;
			case BUTTON_EXIT:
				if (pressedFlag) break;
				LedBlink(2);
				exitflag = 0;
				pressedFlag = 1;
				//pressedFlag2=1;
				while (1) {
					DisplayErase();
					DisplayString(10, 32, "GREY: Shut Down");
					DisplayString(10, 48, "ORANGE: Reprogram Mode");
					DisplayString(10, 16, "LEFT : Back ");
					DisplayUpdateSync();

					I2CTransfer();
					secondbutton = ButtonRead();
					switch (secondbutton) {
						case BUTTON_LEFT:
							if (pressedFlag) break;
							LedBlink(2);
							pressedFlag = 1;
							DisplayErase();
							DisplayUpdateSync();
							exitflag = 1;
							break;
						case BUTTON_ENTER:
							if (pressedFlag) break;
							LedBlink(2);
							pressedFlag = 1;
							DisplayErase();
							DisplayUpdateSync();
							I2CCtrl(REPROGRAM);
							break;
						case BUTTON_EXIT:
							if (pressedFlag) break;
							LedBlink(2);
							pressedFlag = 1;
							DisplayErase();
							DisplayUpdateSync();
							I2CCtrl(POWERDOWN);
							break;
						case BUTTON_RIGHT:
							if (pressedFlag) break;
							pressedFlag = 1;
							volatile int mappingflag =0;
							int breakflag = 0;

							DisplayErase();
							DisplayString(1, 1, "Running autopilot!!");
							DisplayUpdateSync();

							//value2=0;
              UWORD volatile touch_val = 0;
							while (1) {
								I2CSleep(24);
                getTouch(&touch_val);
								thirdbutton = ButtonRead();
								getLight( & value2);
								if (value2 == 1) {
									if(mappingflag==0){
										send_cmd(com_ahead);}
									mappingflag=1;
									
									OutputSetSpeed(1, 0);
									OutputSetSpeed(2, 0);
                  OutputSetSpeed(3, 0);
									I2CTransfer();
									int lightFlag = 0;
									I2CSleep(1000);
									while (1) {
										I2CTransfer();
										getLight( & value2);
										thirdbutton = ButtonRead();
										if (thirdbutton == BUTTON_EXIT) break;
										if (value2 == 0) lightFlag = 1;
										if (lightFlag = 1 && value2 == 1) break;

									}
								}
								if (thirdbutton == BUTTON_EXIT ) {
									OutputSetSpeed(1, 0);
									OutputSetSpeed(2, 0);
                  							OutputSetSpeed(3, 0);
                  							I2CTransfer();
									break;
								}

                						getTouch(&touch_val);
								if(touch_val == 1){
									OutputSetSpeed(3,-80);
									OutputSetSpeed(1,80);
									OutputSetSpeed(2,80);
									I2CTransfer();
									send_bit(1);
									if(I2CSleepWithUv(500))break;
									//OutputSetSpeed(3,-80);
	
								}
								I2CTransfer();
								getUltrasonic( & value_u1, 4);
								getUltrasonic( & value_u2, 2);
								//if(value == 1){
							        count_u1 = 0, count_u2 = 0, prev_state = 0;
								for (int i = 0; i < 50; i++) {
									I2CTransfer();
									getUltrasonic( & value_u1, 4);
									getUltrasonic( & value_u2, 2);
									if (value_u1) count_u1++;
									if (value_u2) count_u2++;
								}
								if (count_u1 >= 37 || count_u2 >= 37) {
									if (count_u1 > count_u2) {
										OutputSetSpeed(1, 80);
										OutputSetSpeed(2, 80);
										OutputSetSpeed(3, 80);

										if(I2CSleepWithUv(1700))break;

										if(mappingflag){
										OutputSetSpeed(1,0);
										OutputSetSpeed(2,0);
										OutputSetSpeed(3,0);

										I2CTransfer();
										send_bit(0);
										}
										OutputSetSpeed(1,-80);
										OutputSetSpeed(2,-80);
										OutputSetSpeed(3,-80);

                    //prev_state = 1;
                    I2CTransfer();
									} else {
										OutputSetSpeed(1, 80);
										OutputSetSpeed(2, 80);
										OutputSetSpeed(3,-80);
										if(I2CSleepWithUv(1700))break;
										if(mappingflag){
										OutputSetSpeed(1,0);
										OutputSetSpeed(2,0);
										OutputSetSpeed(3,0);
										I2CTransfer();
										send_bit(1);
										}
										OutputSetSpeed(1,-80);
										OutputSetSpeed(2,-80);
										OutputSetSpeed(3,80);
                    prev_state = -1;
                    I2CTransfer();
									}
									//I2CSleep(1000);
								} else {
									
									OutputSetSpeed(1, -78); //73
									OutputSetSpeed(2, -78); //73
                 // if(prev_state == 1) {
                   // OutputSetSpeed(3, 80);
		   // send_cmd(com_left);
		   // I2CSleep(200);
                    //prev_state = 0;
                 // } else if(prev_state == -1) {
                  //  OutputSetSpeed(3, -80);
		  //  send_cmd(com_right);
		  //  I2CSleep(200);
                    //prev_state = 0;
                  //} //else 
		  // prev_state = 0;
                    //OutputSetSpeed(3, 0);
                  I2CTransfer();
									//I2CSleep(500);
								}
								//}
								//else{
								//	OutputSetSpeed(1,73);
								//	OutputSetSpeed(2,73);
								//	//I2CSleep(500);
								//}

							}
							break;
						case BUTTON_NONE:
							pressedFlag = 0;
					}
					if (exitflag == 1) break;
				}
				break;
			case BUTTON_NONE:
				pressedFlag = 0;
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
