#ifndef PWM_H
#define PWM_H
//includes
#include "standard.h"		//for standard types
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()

class PWM
{
public:
	PWM()
	{
	//motor 1 OCRs
	TCCR4A= _BV(WGM40)|_BV(COM4C1)|_BV(COM4B1); // 8bit Phase correct PWM, 0xFF is TOP, non inverted mode
	TCCR4B =_BV(CS40);							// prescaler = 1
	OCR4BL = 0x80;  							// motor 1 set to zero torque
	OCR4CL = 0x80;
	DDRH = _BV(PH4)|_BV(PH5);					// set OC4C (PH5) as output, set OC4B (PH4) as output
	
	//motor 2 OCRs
	TCCR3A= _BV(WGM30)|_BV(COM3C1)|_BV(COM3B1); // 8bit Phase correct PWM, 0xFF is TOP, non inverted mode   
	TCCR3B = _BV(CS30);							// prescaler = 1
	OCR3BL = 0x80; 								// motor 2 set to zero torque
	OCR3CL = 0x80;
	DDRE = _BV(PE4)|_BV(PE5);					// set OC3C (PE5) as output, set OC3B (PE4) as output
	}
	~PWM()
	{
	}
private:

};
#endif // PWM_H
