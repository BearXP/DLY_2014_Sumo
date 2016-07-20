#include "TDT.h"
#include "sumo_motor.h"
#include "sumo_ports.h"
#include "sumo_dotmatrix.h"
#include "sprites.h"
#include "sumo_timer.h"
#include <avr/io.h>			//for SFRs
#include <avr/interrupt.h>	//for interrupt stuff



SumoMotor motor;
using namespace Sumo;


void refreshDotMatrix()
{
	DotMatrix::instance()->incrementalRefresh();	
}


ISR (TIMER5_OVF_vect)  //time
{
	static bool l = true;
	if(l == true)
	{
		
		motor.Motor1_Dir(-1);
		motor.Motor2_Dir(1);
		//MOTOR1_HIGH = MOTOR_TOP/2;
		//MOTOR1_LOW = MOTOR_TOP/2;		
		//MOTOR1_HIGH = (MOTOR_TOP/2)-TORQUE;
		//MOTOR1_LOW = (MOTOR_TOP/2)+TORQUE;
		l = false;
	}
	else
	{
		
		motor.Motor1_Dir(1);
		motor.Motor2_Dir(-1);
		//MOTOR1_HIGH = (MOTOR_TOP/2)+TORQUE;
		//MOTOR1_LOW = (MOTOR_TOP/2)-TORQUE;		
		l = true;
	}	
}

ISR (TIMER1_COMPA_vect)
{			
	motor.Motor_Step();		
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
	
	OCR1A = 0x061A;    	//0x1FFF = 30Hz  @ 1/8 prescaler
						//0x7A12 = 1Hz   @ 1/64 prescaler
						//0x186A = 5Hz   @ 1/64 prescaler
						//0x0C35 = 10Hz   @ 1/64 prescaler
						//0x061A = 20Hz   @ 1/64 prescaler
						//0x0411 = 30Hz   @ 1/64 prescaler
						//0x037C = 30Hz   @ 1/64 prescaler
						//0x030D = 40Hz   @ 1/64 prescaler

	/*Timer 5 Normal mode */
    TCCR5A = 0;
	
    /*
     * Start timer 5.
     */
    TCCR5B = _BV(CS52)|_BV(CS50); //  1/64 prescaler    
	//TCCR5B |= _BV(WGM52); //Clear timer on Compare match with OCR1A

    /* Enable timer 5 overflow interrupt. */
    TIMSK5 = _BV(TOIE5);
	
	OCR5A = 0x7A12;    	//0x1FFF = 30Hz  @ 1/8 prescaler
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
	
	//motor 1 OCRs
	TCCR4A= _BV(WGM40)|_BV(WGM41)|_BV(COM4C1)|_BV(COM4B1); //Phase correct PWM, ORC4A is TOP
	TCCR4B = _BV(WGM43)|_BV(CS40);//|_BV(CS41); //   prescaler = 1
	
	OCR4A = MOTOR_TOP; //0x00FF=8bit,0x01FF=9bit
	OCR4C = MOTOR_TOP/2;
	OCR4B =MOTOR_TOP/2;
	
	//motor 2 OCRs
	TCCR3A= _BV(WGM30)|_BV(WGM31)|_BV(COM3C1)|_BV(COM3B1); //Phase correct PWM, ORC3A is TOP    
	TCCR3B = _BV(WGM33)|_BV(CS30);//|_BV(CS31); //   prescaler = 1
	
	OCR3A = MOTOR_TOP; //0x00FF=8bit,0x01FF=9bit
	OCR3C = MOTOR_TOP/2;
	OCR3B = MOTOR_TOP/2;

	
	
    sei ();
}

int main()
{

	Setup_Sumo_Ports();
	ioinit ();
	// enable interrupts
	sei();

	// create timer that executes refreshDotMatrix() at 4000 hertz
	Timer::instance(0)->setFrequency(4000);
	Timer::instance(0)->setFunctionPtr(refreshDotMatrix);
	Timer::instance(0)->start();

	motor.SetTorque(255);
	motor.Motor1_Dir(-1);
	motor.Motor2_Dir(1);

	DotMatrix::instance()->loadSprite(
			//sc_hexDigits_mini[(voltage / 100) % 10],
			sc_hexDigits_mini[(2) % 10],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);

	while (true)
	{
		
	}
	return int(0);
}
