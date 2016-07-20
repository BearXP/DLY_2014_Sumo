#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>

#include "main.h"
#include "sumo_ports.h"
#include "sumo_util.h"
#include "sumo_leds.h"

volatile uint8_t led_on;
volatile uint8_t mouse_move;
volatile int16_t xdif;
volatile int16_t ydif;
int16_t Upchar[8] = {0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
int16_t Rightchar[8] = {0x8080,0x8080,0x8080,0x8080,0x8080,0x8080,0x8080,0x8080};
int16_t Downchar[8] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xFF00};
int16_t Leftchar[8] = {0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101};

enum { UP,RIGHT,DOWN,LEFT };

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
ISR (PCINT1_vect)
{
	mouse_move =1;  //so display will preserve last mouse move.
	
	//read ports in case there is another change
	int8_t currentm1 = MOUSE_PORT1;
	int8_t currentm2 = MOUSE_PORT2;
	int8_t pin_change = (previous_mouse^currentm1)&(previous_mouse^interrupt_edge_mouse);
	
	if(pin_change==_BV(MOUSE2_X1))
	{
		if(currentm2&_BV(MOUSE2_X2)) //moving left
			xdif--;
		else
			xdif++;
	}
	
	if(pin_change==_BV(MOUSE2_Y1))
	{
		if(currentm2&_BV(MOUSE2_Y2)) //moving left
			ydif--;
		else
			ydif++;
	}
		
	
	previous_mouse = currentm1;
	
	
}

ISR (TIMER1_COMPA_vect)		
{
	//static int8_t count =0;
	static int8_t comcount =0;
	static int16_t line;
	static int16_t *p;
	
	
	
	/*
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
		
	}*/
	
	
	if(mouse_move&&comcount==0)
	{
		if(abs(xdif)>abs(ydif))
		{
			if(xdif>=0)
				p = Rightchar;
			else
				p = Leftchar;
		}
		else
		{
			if(ydif>=0)
				p = Upchar;
			else
				p = Downchar;
		}
		
		mouse_move = 0;
		xdif=0;
		ydif=0;
	}
	//p=Rightchar;
	line = p[comcount];
	
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
						
	//Setup Pin Change Inerrupt
	PCICR = _BV(PCIE1);   //enable Pin Change int 1 for PCINT 8-15  (11 for X1)
	PCMSK1 = _BV(PCINT9)|_BV(PCINT10)|_BV(PCINT11)|_BV(PCINT12);  //PCINT11
	
    sei ();
}


int main(void)
{			
	char sw = 0;
	led_on = 1;
	mouse_move = 1;
	Setup_Sumo_Ports();
	ioinit ();
	
	//setup pin change edges
	interrupt_edge_mouse = 0x0F; //first four bits
	previous_mouse = 0x00;

	while(1)
	{		
		if( (Get_Switch(1)|Get_Switch(2) )) led_on = 1;
		else led_on = 0;					
	}
}
