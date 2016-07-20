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
		
			
			
			PORTD = 0x10;
	
			for(long i=0; i<900000; i++)
			{
			  
			}
			
		
			PORTD=0x20;
	
			for(long i=0; i<900000; i++)
			{}
			
			PORTD=0x40;
	
			for(long i=0; i<900000; i++)
			{}
			
			PORTD=0x80;
	
			for(long i=0; i<900000; i++)
			{}
			
			PORTD=0;
	
			for(long i=0; i<900000; i++)
			{}
		
		
			
	}

}
