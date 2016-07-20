/* 
 *PORT and bit definitions for VBS SUMO V1.0
 *
 *
 *
 */

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





//function definitions

void Setup_Sumo_Ports(void);  //setup direction, pullups and initial values of ports.
