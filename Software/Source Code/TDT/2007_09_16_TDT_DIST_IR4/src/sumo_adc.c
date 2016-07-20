#include <avr/io.h>
#include <avr/interrupt.h>
#include "sumo_adc.h"

using namespace Sumo;

void A2DConverter::enable()
{
	// set reference and alignment info
	ADMUX =
		(0<<REFS1) | (1<<REFS0) |		// use AVCC as reference
		(0<<ADLAR);						// do not left-align sample data

	
	//set prescaler to 128 = 125kHz ADC clock
	ADCSRA = _BV(ADPS0)|_BV(ADPS1)|_BV(ADPS2);  
	
	// enable the ADC
	ADCSRA |= _BV(ADEN); 

	ADCSRB = 0x00;

	// disable digital input pins for ADC0 - ADC3
	DIDR0 |= (_BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D));

	// disable digital input pins for ADC11 - ADC14
	DIDR2 |= (_BV(ADC11D) | _BV(ADC12D) | _BV(ADC13D) | _BV(ADC14D));
}

void A2DConverter::disable()
{
	// set mux address to 0
	ADMUX = 0x00;

	// enable digital input pins for ADC0 - ADC3
	DIDR0 &= ~(_BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D) | _BV(ADC3D));

	// enable digital input pins for ADC0 - ADC3
	DIDR2 &= ~(_BV(ADC11D) | _BV(ADC12D) | _BV(ADC13D) | _BV(ADC14D));

	// disable the ADC
	ADCSRA = 0x00;
	ADCSRB = 0x00;
}

USHORT A2DConverter::getSample(UBYTE input)
{
	// select the mux value
	//limited to 0-3 and 11-14
	if(input<=3)
	{
		ADMUX = (ADMUX & ~(0x1F<<MUX0)) | ((input & 0x1F)<<MUX0); //clear first 5 bits and then set input
		ADCSRB = (ADCSRB & ~(_BV(MUX5)));
	}

	else if(input>=11&&input<=14)
	{
		//clear previous AD selection
		ADMUX = (ADMUX & ~(0x1F<<MUX0));
		ADCSRB = (ADCSRB & ~(_BV(MUX5)));

		//select new one
		UBYTE adnew = (input-8);
		ADMUX |= ((adnew & 0x1F)<<MUX0); //use first 5 bits for ADMUX
		ADCSRB |= _BV(MUX5);//set bit
	}
	else
	{
		return 0;
	}

	// start a conversion
	ADCSRA |= _BV(ADSC);

	// wait until conversion is complete
	while (ADCSRA & _BV(ADSC))
	{
		asm volatile ("nop\n\t" ::);
	}
	
	// get sample data
	UBYTE lowByte = ADCL;
	UBYTE highByte = ADCH;

	// return sample value
	return ((USHORT)highByte << 8) | lowByte;
}
