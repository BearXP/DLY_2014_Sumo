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
	DDRH = _BV(PH4)|_BV(PH5);

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