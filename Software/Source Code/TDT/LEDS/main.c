#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "sumo_ports.h"
#include "sumo_util.h"
#include "sumo_leds.h"

volatile uint8_t led_dir;

enum { UP, DOWN };

/*ISR (TIMER1_OVF_vect)		
{
	static int8_t count =0;
	static int8_t comcount =0;
	count++;
	
	
	//send line
	if(count ==1)
		LED_Send_Line(0x0F0F);
	else
	{
		LED_Send_Line(0xF0F0);
		count = 0;
	}
	
	//increment common driver
	
	LED_Set_Common(comcount);
	comcount++;
	
	if(comcount > 7)
		comcount = 0; //go back to first line


      
	
}*/

ISR (TIMER1_COMPA_vect)		
{
	//static int8_t count =0;
	static int8_t comcount =0;
	static int16_t line;
	
	
	switch (comcount)		//which line?
	{
		case 0:
			line = 0xBCBC;
			break;
		case 1:
			line = 0x80A4;
			break;
			
		case 2:
			line = 0xBCBC;
			break;
			
		case 3:
			line = 0xA080;
			break;
			
		case 4:
			line = 0xBCBC;
			break;
		case 5:
			line = 0x8084;
			break;
		case 6:
			line = 0xB9BC;
			break;
		case 7:
			line = 0xAF84;
			break;
		
	}
	
	
	//send line
	LED_Send_Line(line);
	
	
	//increment common driver
	
	LED_Set_Common(comcount);
	comcount++;
	
	if(comcount > 7)
		comcount = 0; //go back to first line


      
	
}

void ioinit (void)			/* Note [6] */
{
    /* Normal mode */
    TCCR1A = 0;
	
    /*
     * Start timer 1.
     */
    TCCR1B = _BV(CS11)|_BV(CS10); //  1/64 prescaler    
	TCCR1B |= _BV(WGM12); //Clear timer on Compare match with OCR1A

    /* Enable timer 1 overflow interrupt. */
    TIMSK1 = _BV (OCIE1A);
	
	OCR1A = 0x030D;    	//0x1FFF = 30Hz  @ 1/8 prescaler
						//0x7A12 = 1Hz   @ 1/64 prescaler
						//0x186A = 5Hz   @ 1/64 prescaler
						//0x0C35 = 10Hz   @ 1/64 prescaler
						//0x061A = 20Hz   @ 1/64 prescaler
						//0x0411 = 30Hz   @ 1/64 prescaler
						//0x037C = 30Hz   @ 1/64 prescaler
						//0x030D = 40Hz   @ 1/64 prescaler
	
    sei ();
}


int main(void)
{			
	char sw = 0;
	led_dir = UP;
	Setup_Sumo_Ports();
	ioinit ();

	while(1)
	{		
		if( (Get_Switch(1)|Get_Switch(2) )) led_dir = UP;
		else led_dir = DOWN;					
	}
}
