#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>



int main(void)
{
	cli();

	DDRD = 255;	
	PORTD = 0;
	
	DDRH = 0;	
	
	char sw = 0;


	while(1)
	{
		PORTD = 0x00;
		sw = PINH;
		sw = sw & 0xc0;
		if(sw>0) PORTD = 0x10;
		else PORTD = 0x00;
			
			
	}

}
