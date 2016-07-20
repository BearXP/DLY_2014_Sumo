#include <avr/io.h>

#include "sumo_leds.h"
#include "sumo_ports.h"
#include "sumo_util.h"




void LED_Send_Line(uint16_t line)  //clock in 16 bits into segment driver and latch
{
	
	for(uint16_t i=0;i<16;i++) //LSB first
	{
		if((line & 0x01) > 0) //check MSB
		{
			LED_PORT2 |= _BV(LED_SIN);			
		}
		else
		{
			LED_PORT2 &= ~(_BV(LED_SIN));
		}
		line >>= 1;  //shift data right one	
		//NOP;
		
		//clock in bit
		LED_PORT2 |= _BV(LED_SCLK);  //set clock high
		NOP;
		LED_PORT2 &= ~(_BV(LED_SCLK)); //set clock low		
		
	}
	
	//latch in line
		LED_PORT2 |= _BV(LED_LATCH);  //set clock high
		NOP;
		LED_PORT2 &= ~(_BV(LED_LATCH)); //set clock low	
}


void LED_Set_Common(uint8_t com)	//set the common driver  Range 0-7
{
	LED_PORT2 |= _BV(LED_BLANK);  //Blank LEDs
				
		com <<= 4;   //shift so it matches bits on port
		LED_PORT1 &= ~(_BV(LED_CSEL0)|_BV(LED_CSEL1)|_BV(LED_CSEL2));  //clear CSEL bits
		LED_PORT1 |= com;  //set new CSEL values
		
	LED_PORT2 &= ~(_BV(LED_BLANK)); //Un-blank LEDS
}
