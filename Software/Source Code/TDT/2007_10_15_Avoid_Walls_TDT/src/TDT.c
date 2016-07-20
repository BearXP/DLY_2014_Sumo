#include "TDT.h"
#include "sumo_motor_fs.h"
#include "sumo_ports.h"
#include "sumo_dotmatrix.h"
#include "sprites.h"
#include "sumo_timer.h"
#include "sumo_adc.h"
#include "sumo_irsensor.h"
#include "sumo_ircalib.h"
#include "sumo_line.h"
#include "sumo_readvolt.h"
#include <avr/io.h>			//for SFRs
#include <avr/interrupt.h>	//for interrupt stuff


enum sumo_state{  //different states in machine
state_search,
state_obstacle,
state_turn_left,
state_turn_right
};

sumo_state state;

enum sumo_mode{ //different modes of behaviour
mode_find,
mode_flee,
};

const USHORT IR_THRESHOLD = 280;

using namespace Sumo;

Motor lmotor;
Motor rmotor;

ReadVoltages volts;

bool line_triggered;
BYTE dir = 1;  //0=back,1=stop,2=forward
USHORT step = 0x031A;
USHORT numSteps = 0;  //run for maxSteps
USHORT maxSteps = 2100;

LineSensors lines(line_falling);


void refreshDotMatrix()
{
	DotMatrix::instance()->incrementalRefresh();	
}

void motorUpdate()
{
	lmotor.step();
	rmotor.step();	
	
}
void quickStop()
{
	lmotor.setTargetSpeed(0);
	rmotor.setTargetSpeed(0);
/*
	if(dir ==2) // reverse
	{
		motor.Motor1_Dir(1);  //reverse (stop quick)
		motor.Motor2_Dir(-1);
		motor.SetTorque(255);
		dir =0;
			
	}
	
	

	for(int del =0;del<10000;del++) //pause
	{	for(int eel =0;eel<10;eel++)
		{}
	}

	motor.SetTorque(0); //then stop
	*/
}


void checkLines()
{
	
	//read ports in case there is another change
	//int8_t current_line = LINE_PORT1;	
	//int8_t pin_change = (previous_port_j^current_line)&(previous_port_j^interrupt_edge_port_j);  //mask the single bit
	/*
	if(!(lines.GetLineDetected(1)))
	{
		DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);
	}
	else
	{
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[1],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);
		quickStop();
	}
	
	if(!(lines.GetLineDetected(2)))
	{
		DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);		

	}
	else
	{
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[2],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);
		quickStop();
	}
	
	if(!(lines.GetLineDetected(3)))
	{
		DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);
	}
	else
	{
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[3],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);
		quickStop();
	}
	if(!(lines.GetLineDetected(4)))
	{
		DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			12
		);
	}
	else
	{
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[4],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			12
		);
		quickStop();
	}*/
}

void displayInt(UINT number)  //display a number up to 4 digits.
{
	UBYTE adcIndex = UBYTE(number % 10); //1s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			12
		);	
		adcIndex = UBYTE((number/10) % 10); //10s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);
		adcIndex = UBYTE((number/100) % 10); //100s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);
		adcIndex = UBYTE((number/1000) % 10); //1000s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);
}

void displayFloat(FLOAT number,UBYTE number_places)  //display a float with up to 4 decimal places
{

	USHORT factor = 1;

	for(UBYTE i=0;i<number_places;i++)
	{
		factor = factor*10;
	}


	UBYTE adcIndex = UBYTE(UINT(number*factor) % 10); //1s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			12
		);	
		adcIndex = UBYTE(UINT(number*factor/10) % 10); //10s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);
		adcIndex = UBYTE(UINT(number*factor/100) % 10); //100s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);
		adcIndex = UBYTE(UINT(number*factor/1000) % 10); //1000s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);

		//decimal place
		USHORT dot = 1;
		DotMatrix::instance()->loadSprite(&dot,1,1,5,7);


}


ISR (TIMER5_OVF_vect)  //time
{
/*	static bool l = true;
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
		motor.Motor2_Dir(1);
		//MOTOR1_HIGH = (MOTOR_TOP/2)+TORQUE;
		//MOTOR1_LOW = (MOTOR_TOP/2)-TORQUE;		
		l = true;
	}	*/
}

/*
ISR (TIMER1_COMPA_vect)
{			
	//OCR1A = step;
	//motor.Motor_Step();	
	//numSteps++;
	//if(numSteps>maxSteps)
	//	quickStop();	
	
}
*/

ISR (PCINT1_vect)
{
	
	lines.CheckLines();
	//checkLines();	
	
}

ISR (PCINT2_vect)
{
	
	lines.CheckLines();
	//checkLines();
	
}

void ioinit (void)			/* Note [6] */
{
    /* Normal mode */
    //TCCR1A = 0;
	
    /*
     * Start timer 1.
     */
   // TCCR1B = _BV(CS11)|_BV(CS10); //  1/64 prescaler    
//	TCCR1B |= _BV(WGM12); //Clear timer on Compare match with OCR1A

    /* Enable timer 1 overflow interrupt. */
//    TIMSK1 = _BV (OCIE1A);
	
//	OCR1A = 0x081A;    	//0x1FFF = 30Hz  @ 1/8 prescaler
						//0x7A12 = 1Hz   @ 1/64 prescaler
						//0x186A = 5Hz   @ 1/64 prescaler
						//0x0C35 = 10Hz   @ 1/64 prescaler
						//0x061A = 20Hz   @ 1/64 prescaler
						//0x0411 = 30Hz   @ 1/64 prescaler
						//0x037C = 30Hz   @ 1/64 prescaler
						//0x030D = 40Hz   @ 1/64 prescaler

	/*Timer 5 Normal mode */
    //TCCR5A = 0;
	
    /*
     * Start timer 5.
     */
    //TCCR5B = _BV(CS52)|_BV(CS50); //  1/64 prescaler    
	//TCCR5B |= _BV(WGM52); //Clear timer on Compare match with OCR1A

    /* Enable timer 5 overflow interrupt. */
    //TIMSK5 = _BV(TOIE5);
	
	//OCR5A = 0x7A12;    	//0x1FFF = 30Hz  @ 1/8 prescaler
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
	state = state_search;
	USHORT speed =200;
	
	Setup_Sumo_Ports();
	line_triggered=false;
	ioinit ();
	// enable interrupts
	sei();

	// create timer that executes refreshDotMatrix() at 4000 hertz
	Timer::instance(0)->setFrequency(1000);
	Timer::instance(0)->setFunctionPtr(refreshDotMatrix);
	Timer::instance(0)->start();

	lmotor.setPhaseRegisters(&LEFT_PHASE_A, &LEFT_PHASE_B);
	rmotor.setPhaseRegisters(&RIGHT_PHASE_A, &RIGHT_PHASE_B);

	lmotor.setTimerNumber(1);
	rmotor.setTimerNumber(1);

	lmotor.setTargetSpeed(speed);
	rmotor.setTargetSpeed(speed);

	lmotor.setTorque(255);
	rmotor.setTorque(255);

	lmotor.setAcceleration(200);
	rmotor.setAcceleration(200);

	lmotor.setReverse(true);
	rmotor.setReverse(false);

	Timer::instance(1)->setFrequency(200);
	Timer::instance(1)->setFunctionPtr(motorUpdate);
	Timer::instance(1)->start();
	
	
	// enable the adc
	A2DConverter::enable();
	

	IRSensor IRL(UBYTE(14));  	//left  
	IRSensor IRB(UBYTE(13));  	//back
	IRSensor IRR(UBYTE(12));	//right
	IRSensor IRFR(UBYTE(11));	//front right
	IRSensor IRFL(UBYTE(0));	//frong left

	IRSensor * rangeSensors[5];
	rangeSensors[0] = &IRFL;
	rangeSensors[1] = &IRFR;
	rangeSensors[2] = &IRR;
	rangeSensors[3] = &IRB;
	rangeSensors[4] = &IRL;




	//lines.SetFunctionPtr(checkLines);
	//lines.EnableFunction();
		
	USHORT counter =0;

	
	speed =800;
	//displayInt(speed);

	lmotor.setTargetSpeed(speed);
	rmotor.setTargetSpeed(speed);
	
	while (true)
	{	
		if(counter > 100)
		{
			double d = volts.GetVoltageBatt();
			displayFloat(d,2);
			counter =0;
		}
		counter++;	
	
		switch (state)
		{
			case state_search:
			{
				//displayInt(0);
				USHORT fl,fr,r,b,l;			//read IR sensors
				fl = IRFL.ReadSensorDist();
				fr = IRFR.ReadSensorDist();
				r = IRR.ReadSensorDist();
				b = IRB.ReadSensorDist();
				l = IRL.ReadSensorDist();

				if(fl<IR_THRESHOLD||fr<IR_THRESHOLD) //obstacle in front
				{
					if(fl+l>fr+r)
					{
						state = state_turn_left;
					}	
					else
					{
						state = state_turn_right;
					}

					lmotor.setAcceleration(2000);
					rmotor.setAcceleration(2000);

					lmotor.setTargetSpeed(0); //stop
					rmotor.setTargetSpeed(0);
					
				}

				

				break;
			}
			case state_obstacle:
			{
				break;
			}
			case state_turn_left:
			{
				while ( rmotor.getCurrentSpeed()>1)
				{}
				//displayInt(1);
				lmotor.setAcceleration(200);
				rmotor.setAcceleration(200);

				lmotor.setTargetSpeed(200); 
				rmotor.setTargetSpeed(200);

				lmotor.setReverse(false);
				rmotor.setReverse(false);

				while ( rmotor.getCurrentSpeed()<200)
				{}

				for(int del =0;del<20000;del++)
				{	for(int eel =0;eel<70;eel++)
					{}
				}

				//lmotor.setTargetSpeed(200);  //go straight

				state = state_search;
				lmotor.setReverse(true);
				rmotor.setReverse(false);

				lmotor.setTargetSpeed(speed);
				rmotor.setTargetSpeed(speed);
				break;
			}
			case state_turn_right:
			{
				while ( rmotor.getCurrentSpeed()>1)
				{}
				//displayInt(2);
				lmotor.setAcceleration(200);
				rmotor.setAcceleration(200);

				lmotor.setTargetSpeed(200); //stop
				rmotor.setTargetSpeed(200);

				lmotor.setReverse(true);
				rmotor.setReverse(true);

				while ( rmotor.getCurrentSpeed()<200)
				{}

				for(int del =0;del<20000;del++)
				{	for(int eel =0;eel<70;eel++)
					{}
				}

				state = state_search;
				lmotor.setReverse(true);
				rmotor.setReverse(false);

				lmotor.setTargetSpeed(speed);
				rmotor.setTargetSpeed(speed);
				break;
			}
		}


	}
	return int(0);
}
