#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdlib.h>

#include "main.h"
#include "sumo_ports.h"
#include "sumo_util.h"
#include "sumo_leds.h"

volatile uint8_t led_on;
volatile uint8_t line_triggered;
volatile int16_t xdif;
volatile int16_t ydif;
char sw = 0;

int16_t Upchar[8] = {0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
int16_t Rightchar[8] = {0x8080,0x8080,0x8080,0x8080,0x8080,0x8080,0x8080,0x8080};
int16_t Downchar[8] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xFF00};
int16_t Leftchar[8] = {0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101,0x0101};
int16_t Stopchar[8] = {0x3C3C,0x7E7E,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0x7E7E,0x3C3C};

enum { UP,RIGHT,DOWN,LEFT };


ISR (PCINT1_vect)
{
	line_triggered =0;  //so display will preserve last mouse move.
	
	//read ports in case there is another change
	int8_t current_line = LINE_PORT1;	
	int8_t pin_change = (previous_port_j^current_line)&(previous_port_j^interrupt_edge_port_j);  //mask the single bit
	
	if(pin_change==_BV(LINE_1)&&(current_line&_BV(LINE_1)))
	{
		line_triggered =1;
	}
	
	if(pin_change==_BV(LINE_2))
	{
		line_triggered =1;
	}
	
	if(pin_change==_BV(LINE_3))
	{
		line_triggered =1;
	}
		
	
	previous_port_j = current_line;	
	
}

		

//void Step(void)
ISR (TIMER1_COMPA_vect)
{
	//static int8_t count =0;
	static int8_t comcount =0;
	static int16_t line;
	static int16_t *p;
	
		
	if(sw ==0)
	{
		line = 0x00FF;
	}
	else 
	{
		line = 0xF0F0;
	}
	
		//send line
		LED_Send_Line(line);
		
		
		//increment common driver
		
		LED_Set_Common(comcount);
		comcount++;
		
	switch (comcount)		
    {
        case 1:
            PORTH = _BV(PH4)|_BV(PH5);
            break;
			
		case 2:
            PORTH = _BV(PH4);
            break;
			
		case 3:
            PORTH = 0;
            break;
			
		case 4:
            PORTH = _BV(PH5);
            break;
			
		case 5:
            PORTH = _BV(PH4)|_BV(PH5);
            break;
			
		case 6:
            PORTH = _BV(PH4);
            break;
			
		case 7:
            PORTH = 0;
            break;
			
		case 8:
            PORTH = _BV(PH5);
            break;

        
    }
	
	if(comcount > 7)
		comcount = 0; //go back to first line	
	
}
/*
ISR (TIMER1_COMPA_vect){
	//static int8_t count =0;
	static int8_t comcount =0;
	static int16_t line;
	static int16_t *p;
	
		
	if(sw ==0)
	{
		line = 0x00FF;
	}
	else 
	{
		line = 0xF0F0;
	}
	
		//send line
		LED_Send_Line(line);
		
		
		//increment common driver
		
		LED_Set_Common(comcount);
		comcount++;
		
	switch (comcount)		
    {
        case 1:
            OCR4C = 0x0001;
			OCR4B = 0x0001;
            break;
			
		case 2:
            OCR4C = 0xFFFE;
			OCR4B = 0x0001;
            break;
			
		case 3:
            OCR4C = 0xFFFE;
			OCR4B = 0xFFFE;
            break;
			
		case 4:
            OCR4C = 0x0001;
			OCR4B = 0xFFFE;
            break;
			
		case 5:
            OCR4C = 0x0001;
			OCR4B = 0x0001;
            break;
			
		case 6:
            OCR4C = 0xFFFE;
			OCR4B = 0x0001;
            break;
			
		case 7:
            OCR4C = 0xFFFE;
			OCR4B = 0xFFFF;
            break;
			
		case 8:
            OCR4C = 0x0001;
			OCR4B = 0xFFFE;
            break;

        
    }
	
	if(comcount > 7)
		comcount = 0; //go back to first line	
	
}*/



void ioinit (void)			/* Note [6] */
{
    /* Normal mode */
    TCCR1A = 0;
	
    /*
     * Start timer 1.
     */
    TCCR1B = _BV(CS11);//|_BV(CS10); //  1/64 prescaler    
	TCCR1B |= _BV(WGM12); //Clear timer on Compare match with OCR1A

    /* Enable timer 1 overflow interrupt. */
    TIMSK1 = _BV (OCIE1A);
	
	OCR1A = 0xaFFF;    	//0x1FFF = 30Hz  @ 1/8 prescaler
						//0x7A12 = 1Hz   @ 1/64 prescaler
						//0x186A = 5Hz   @ 1/64 prescaler
						//0x0C35 = 10Hz   @ 1/64 prescaler
						//0x061A = 20Hz   @ 1/64 prescaler
						//0x0411 = 30Hz   @ 1/64 prescaler
						//0x037C = 30Hz   @ 1/64 prescaler
						//0x030D = 40Hz   @ 1/64 prescaler
						
	//Setup Pin Change Inerrupt
	//PCICR = _BV(PCIE1);   //enable Pin Change int 1 for PCINT 8-15  (11 for X1)
	//PCMSK1 = _BV(PCINT9)|_BV(PCINT10)|_BV(PCINT11)|_BV(PCINT12)|_BV(PCINT13)|_BV(PCINT14)|_BV(PCINT15);  //mouse and line sensors
	
	//motor OCRs
	//TCCR4A= _BV(WGM41)|_BV(COM4C1)|_BV(COM4B1);
	
	
	//TCCR4B = _BV(CS41)|_BV(WGM42);//|_BV(CS40); //   prescaler
	
	OCR4C = 0;
	OCR4B =0;
	
    sei ();
}


int main(void)
{			
	char line = 0;
	char tmp =0;
	
	led_on = 1;
	line_triggered = 1;
	Setup_Sumo_Ports();
	ioinit ();
	
	//setup pin change edges
	interrupt_edge_port_j = 0x7F; //first seven bits for mouse and 3xline
	previous_port_j = 0x00;

	while(1)
	{	
		for(uint32_t j=0;j<4000000000;j++)
		{
			for(uint32_t i=0;i<4000000000;i++)
			{
				tmp =1;
			}
		}
		//Step();
		/*sw = PINH;
		sw = sw & 0xc0;
		if(sw==0)
		{	 
			OCR4C = 0xFF;
			OCR5C = 0xFF;
		}
		else 
		{	 
			OCR4C = 0x00;
			OCR5C = 0x00;
		}*/
	}
}
