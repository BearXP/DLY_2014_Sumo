/* 
 *PORT and bit definitions for VBS SUMO V1.0
 *
 *
 *
 */

#define NOP							asm volatile ("nop\n\t" ::)

//switches
	#define SW_PORT PINH
	
//LED Matrix
	#define LED_PORT1   PORTD
	#define LED_PORT2   PORTL
	#define LED_SCLK	PL4
	#define LED_SIN		PL5
	#define LED_LATCH 	PL6 //active low
	#define LED_BLANK	PL7
	#define LED_CSEL0	PD4
	#define LED_CSEL1	PD5
	#define LED_CSEL2	PD6
	#define LED_DSEL 	PD7 //active low
	
//Mouse1 and Mouse2
	#define MOUSE_PORT1 PINJ
	#define MOUSE_PORT2 PINH
	#define MOUSE1_X1	PJ0
	#define MOUSE1_X2	PH0
	#define MOUSE1_Y1	PJ1
	#define MOUSE1_Y2	PH1
	#define MOUSE2_X1	PJ2
	#define MOUSE2_X2	PH2
	#define MOUSE2_Y1	PJ3
	#define MOUSE2_Y2	PH3

//Voltage control
	#define V3Enable_Port PORTJ
	#define V3Enable	PJ7
	
//Line sensors
	#define LINE_PORT1 	PINJ
	#define LINE_PORT2 	PINK
	#define LINE_1		PJ4
	#define LINE_2		PJ5
	#define LINE_3		PJ6
	#define LINE_4		PK7
//Motors
	#define LEFT_PHASE_A OCR3BL
	#define LEFT_PHASE_B OCR3CL
	#define RIGHT_PHASE_A OCR4BL
	#define RIGHT_PHASE_B OCR4CL




//function definitions

void Setup_Sumo_Ports(void);  //setup direction, pullups and initial values of ports.
Byte getSwitch(Byte);         //returns >0 if the switch "sw" is high, 0 otherwise.
							  //sw = 1 is PH6, sw = 2 is PH7
bool leftFront();
bool rightFront();
bool leftBack();
bool rightBack();



