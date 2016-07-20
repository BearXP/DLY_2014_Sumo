#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "sumo_ports.h"
#include "sumo_util.h"

volatile uint8_t led_dir;

enum { UP, DOWN };

ISR (TIMER1_OVF_vect)		
{
    static int8_t count =0;  
	//static uint8_t ledon;
	
	switch (count)		/* next LED */
	{
		case 1:
			PORTD = 0x10;
			break;
			
		case 2:
			PORTD = 0x20;
			break;
			
		case 3:
			PORTD = 0x40;
			break;
			
		case 4:
			PORTD = 0x80;
			break;
		
	}
	
	
	if(led_dir == UP) //counting up.
	{
		count++;		
		if(count>=5)
			count =1;
	}
	else
	{
		count--;	
		if(count<=0)
			count =4;
	}
}

void ioinit (void)			/* Note [6] */
{
    /* Normal mode */
    TCCR1A = 0;
	
    /*
     * Start timer 1.
     */
    TCCR1B = _BV(CS11)|_BV(CS10); //  1/64 prescaler    


    /* Enable timer 1 overflow interrupt. */
    TIMSK1 = _BV (TOIE1);
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
