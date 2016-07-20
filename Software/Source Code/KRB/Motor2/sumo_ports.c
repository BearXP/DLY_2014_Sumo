#include "standard.h"
#include <avr/io.h>
#include "sumo_ports.h"





void Setup_Sumo_Ports(void)
{
	//PORT A
	//SRAM Address bits.  Low Byte

	//PORT B
	//SPI BUS

	//PORT C
	//SRAM Addres bits.  High Byte

	//PORT D
	//I2C.  USART2.  LED Matrix CSEL and DSEL
	DDRD = _BV(PD4)|_BV(PD5)|_BV(PD6)|_BV(PD7);
	PORTD &= 0x0F;

	//PORT E
	//USART1.  SD Card inputs.  Motor2 PWM.  Sound PWM

	//PORT F
	//Extra ADC.  JTAG

	//PORT G
	//SRAM control.  SRAM A16 bit.  Timing oscillator

	//PORT H
	//Mouse (non interrupt).  Motor1 PWM.  Switch inputs


	//PORT J
	//Mouse (interrupts).  Line inputs (interrupts).  3v3 enable.
	DDRJ = _BV(PJ7);
	PORTJ |= _BV(PJ7);   //turn on 3.3V

	//PORT K
	//Voltage sense.  IR range ADC.  Line 4 input (interrupt).

	//PORT L
	//Bump inputs.  LED Matrix control
	DDRL = _BV(PD4)|_BV(PD5)|_BV(PD6)|_BV(PD7);
	PORTL &= 0x0F;


}

Byte getSwitch(Byte sw)
{
	Byte swin = SW_PORT;
	return (swin & (sw<<6));
}
bool leftFront()
{
	const Byte MAXCOUNT = 8;
	static Byte counts = 0;
	Byte port = LINE_PORT1;
	if (port&(1<<LINE_1))
	{
		counts++;
		if (counts>MAXCOUNT)
		{
			counts = MAXCOUNT;
		}
	} else if (counts != 0)
	{
		counts--;
	}
	return counts<(MAXCOUNT/2);
}
bool rightFront()
{
	const Byte MAXCOUNT = 8;
	static Byte counts = 0;
	Byte port = LINE_PORT1;
	if (port&(1<<LINE_2))
	{
		counts++;
		if (counts>MAXCOUNT)
		{
			counts = MAXCOUNT;
		}
	} else if (counts != 0)
	{
		counts--;
	}
	return counts<(MAXCOUNT/2);
}
bool leftBack()
{
	const Byte MAXCOUNT = 8;
	static Byte counts = 0;
	Byte port = LINE_PORT1;
	if (port&(1<<LINE_3))
	{
		counts++;
		if (counts>MAXCOUNT)
		{
			counts = MAXCOUNT;
		}
	} else if (counts != 0)
	{
		counts--;
	}
	return counts<(MAXCOUNT/2);
}
bool rightBack()
{
	const Byte MAXCOUNT = 8;
	static Byte counts = 0;
	Byte port = LINE_PORT2;
	if (port&(1<<LINE_4))
	{
		counts++;
		if (counts>MAXCOUNT)
		{
			counts = MAXCOUNT;
		}
	} else if (counts != 0)
	{
		counts--;
	}
	//DEBUG 
	return false;
	//return counts<(MAXCOUNT/2);
}
