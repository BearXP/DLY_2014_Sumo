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
state_search,  //looking for oposition
state_search_left,  //turn left until obstacle found in front sensors
state_search_right,  //turn right until obstacle found in front sensors
state_obstacle_front,  //oposition found in front
state_obstacle_left,  //oposition found in left
state_obstacle_right,  //oposition found in right
state_obstacle_back,  //oposition found in back
state_line_detected,  //just hit the line
state_rush,				//heading towards opposition, try to hit them at high speed		
state_push			//a slow steady push, higher torque
};

sumo_state state;

enum sumo_mode{ //different modes of behaviour, not used
mode_find,
mode_flee,
};

//distance sensor consts
const USHORT IR_front_threshold = 899;
const USHORT IR_side_threshold = 899;
const USHORT IR_back_threshold = 899;


//motor consts
const USHORT stop_acc = 3000;
const USHORT normal_acc = 300;
const USHORT top_speed = 600;
const USHORT search_speed = 200;
const USHORT push_speed =100;

const int reverse_delay = 35;
const int turn_delay = 70;

const int stop_timeout = 4000;

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

const bool enable_lines = true;


//countdown variables

const BYTE countdownsecs = 2;
//const BYTE countdownsecs = 5;
const SHORT countdownmsecs = 250;

BYTE countdown = countdownsecs;//starting point
SHORT countdowncounter =countdownmsecs;
bool countdownrunning = true;



//timer callback functions
void refreshDotMatrix()
{


	DotMatrix::instance()->incrementalRefresh();

	if(countdownrunning)
	{
		//display countdown
		DotMatrix::instance()->loadSprite(		
						sc_hexDigits_mini[countdown],
						SPRITE_ALPHANUMERIC_MINI_WIDTH,
						SPRITE_ALPHANUMERIC_MINI_HEIGHT,
						1,
						12
					);

		countdowncounter--;
	
		if(countdowncounter<=0)  //1 second
		{
			countdowncounter = countdownmsecs;
			countdown--; //decrement second
		}	

		if(countdown <= 0)//countdown complete
		{
			countdownrunning = false;

			//display 0
			DotMatrix::instance()->loadSprite(		
						sc_hexDigits_mini[countdown],
						SPRITE_ALPHANUMERIC_MINI_WIDTH,
						SPRITE_ALPHANUMERIC_MINI_HEIGHT,
						1,
						12
					);
		}
	}
}

void leftMotorUpdate()
{
	lmotor.step();	
}

void rightMotorUpdate()
{
	rmotor.step();	
}
//end timer callback functions

//Actions
void stop (USHORT acc)
{
	int motorcounter =0;
	lmotor.setAcceleration(acc);
	rmotor.setAcceleration(acc);

	lmotor.setTargetSpeed(0); //stop
	rmotor.setTargetSpeed(0);

	while ( rmotor.getCurrentSpeed()>1 && motorcounter<stop_timeout)
	{
		motorcounter++;
	}
}

void quickStop()
{
	stop(3000);
}

void forward(USHORT lspeed,USHORT rspeed)
{
	lmotor.setReverse(true);
	rmotor.setReverse(false);

	lmotor.setAcceleration(normal_acc);
	rmotor.setAcceleration(normal_acc);

	lmotor.setTargetSpeed(lspeed);
	rmotor.setTargetSpeed(rspeed);
}

void reverse(USHORT speed)  //straight back
{
	lmotor.setReverse(false);
	rmotor.setReverse(true);

	lmotor.setAcceleration(normal_acc);
	rmotor.setAcceleration(normal_acc);

	lmotor.setTargetSpeed(speed);
	rmotor.setTargetSpeed(speed);

	for(int del =0;del<20000;del++)
	{	for(int eel =0;eel<reverse_delay;eel++)
		{}
	}
}

void turn_right(bool inplace, USHORT lspeed, USHORT rspeed)
{

	lmotor.setAcceleration(normal_acc);
	rmotor.setAcceleration(normal_acc);

	lmotor.setTargetSpeed(lspeed); //stop
	rmotor.setTargetSpeed(rspeed);

	
	lmotor.setReverse(true);

	rmotor.setReverse(inplace);

	for(int del =0;del<20000;del++)
	{	for(int eel =0;eel<turn_delay;eel++)
		{}
	}
	
}

void turn_left(bool inplace, USHORT lspeed, USHORT rspeed)
{

	lmotor.setAcceleration(normal_acc);
	rmotor.setAcceleration(normal_acc);

	lmotor.setTargetSpeed(lspeed); //stop
	rmotor.setTargetSpeed(rspeed);

	
	lmotor.setReverse(!inplace);

	rmotor.setReverse(false);

	for(int del =0;del<20000;del++)
	{	for(int eel =0;eel<turn_delay;eel++)
		{}
	}
	
}

//End Actions


void checkLines()
{
	
	//read ports in case there is another change
	//int8_t current_line = LINE_PORT1;	
	//int8_t pin_change = (previous_port_j^current_line)&(previous_port_j^interrupt_edge_port_j);  //mask the single bit
	
	if(!(lines.GetLineDetected(1)))
	{
		/*DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);*/
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
		state = state_line_detected;
	}
	
	if(!(lines.GetLineDetected(2)))
	{
		/*DotMatrix::instance()->loadSprite(		
			sc_sprite_miniClear,
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);*/		

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
		state = state_line_detected;
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
		state = state_line_detected;
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
		state = state_line_detected;
	}
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
	//USHORT speed =search_speed;
	
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
	rmotor.setTimerNumber(5);

	lmotor.setTargetSpeed(0);
	rmotor.setTargetSpeed(0);

	lmotor.setTorque(255);
	rmotor.setTorque(255);

	lmotor.setAcceleration(normal_acc);
	rmotor.setAcceleration(normal_acc);

	lmotor.setReverse(true);
	rmotor.setReverse(false);

	while(countdownrunning)//wait for countdown
	{}

	Timer::instance(1)->setFrequency(200);
	Timer::instance(1)->setFunctionPtr(leftMotorUpdate);
	Timer::instance(1)->start();

	Timer::instance(5)->setFrequency(200);
	Timer::instance(5)->setFunctionPtr(rightMotorUpdate);
	Timer::instance(5)->start();
	
	
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




	lines.SetFunctionPtr(checkLines);

	if(enable_lines)
		lines.EnableFunction();
		
	USHORT counter =0;
	
	//displayInt(speed);

	
	
		
	while (true)
	{
		
		if(counter == 2000)
		{
			double d = volts.GetVoltageBatt();
			displayFloat(d,2);
			counter = 0;		
		}
		counter++;
		switch (state)
		{
			case state_search:
			{
				
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[1],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);

				forward(search_speed,search_speed);
				
				USHORT fl =0;
				USHORT fr=0;
				USHORT r=0;
				USHORT b=0;
				USHORT l=0;			//read IR sensors x avg times
				
				SHORT avg = 10;

				for(SHORT c = 0;c<avg;c++)
				{

					fl += IRFL.ReadSensorDist();
					fr += IRFR.ReadSensorDist();
					r += IRR.ReadSensorDist();
					b += IRB.ReadSensorDist();
					l += IRL.ReadSensorDist();
				}

				fl = fl/avg;
				fr = fr/avg;
				r = r/avg;
				b = b/avg;
				l = l/avg;

				if(fl<IR_front_threshold||fr<IR_front_threshold) //obstacle in front
				{				
					state = state_obstacle_front;
				}
				else if(r < IR_side_threshold)
				{
					state = state_obstacle_right;
				}
				else if(l < IR_side_threshold)
				{
					state = state_obstacle_left;
				}
				else if(b < IR_back_threshold)
				{
					state = state_obstacle_back;
				}

				if(lines.GetFunctionEnabled())
				{
					lines.CheckLines();
					checkLines();				
				}

				break;
			}
			case state_obstacle_front:
			{
				USHORT dot = 1;
				DotMatrix::instance()->loadSprite(&dot,1,1,0,0);				
				DotMatrix::instance()->loadSprite(&dot,1,1,7,0);
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[2],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);

				USHORT fl,fr;			//read IR sensors
				fl = IRFL.ReadSensorDist();
				fr = IRFR.ReadSensorDist();
				static SHORT rmotor_diff = 0;

				if(fl>IR_front_threshold&&fr>IR_front_threshold) //obstacle in not front
				{				
					state = state_search;  //go back to looking
					rmotor_diff = 0;

					//clear dots
					dot =0;
					DotMatrix::instance()->loadSprite(&dot,1,1,0,0);				
					DotMatrix::instance()->loadSprite(&dot,1,1,7,0);
				}



				if(fl < fr)  //veer left
				{
					if(rmotor_diff<100)
						rmotor_diff += 12;

					dot =0; //dot on left, clear right
					DotMatrix::instance()->loadSprite(&dot,1,1,0,0);
					dot =1;				
					DotMatrix::instance()->loadSprite(&dot,1,1,7,0);
				}
				else //veer right
				{
					if(rmotor_diff > -100)
						rmotor_diff -= 12;

					dot =1; //dot on left, clear right
					DotMatrix::instance()->loadSprite(&dot,1,1,0,0);
					dot =0;				
					DotMatrix::instance()->loadSprite(&dot,1,1,7,0);
				}

				forward(search_speed,search_speed + rmotor_diff);
				break;
			}	
			case state_obstacle_right:
			{
				
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[3],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);
				turn_right(true,search_speed,search_speed);
				state = state_search;
				break;
			}	
			case state_obstacle_left:
			{
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[4],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);
				turn_left(true,search_speed,search_speed);
				state = state_search;
				break;
			}	
			case state_obstacle_back:
			{
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[5],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);
				turn_right(true,search_speed,search_speed);
				state = state_search;
				break;
			}			
			case state_line_detected:
			{
				/*
					line1 = front right
					line2 = back right
					line3 = back left
					line4 = front left
				*/
				int timeout = 0;
				DotMatrix::instance()->loadSprite(		
					sc_hexDigits_mini[6],
					SPRITE_ALPHANUMERIC_MINI_WIDTH,
					SPRITE_ALPHANUMERIC_MINI_HEIGHT,
					1,
					12
				);

				if(lines.GetLineDetected(2) || lines.GetLineDetected(3)) //line at rear, go forward
				{
					state = state_search;
				}
				else //reverse and turn
				{
					stop(3000);
					bool line_left = true;
					if(lines.GetLineDetected(1))  //which side is the line on?
						line_left = false;


					reverse(search_speed);

					if(!line_left)  //turn away from line
						turn_left(true,search_speed,search_speed);
					else
						turn_right(true,search_speed,search_speed);	

					stop(3000);
					state = state_search;
				}
				break;
			}
			case state_rush:
			{
				state = state_search;
				break;
			}
			case state_push:
			{
				state = state_search;
				break;
			}
			case state_search_left:
			{
				state = state_search;
				break;
			}
			case state_search_right:
			{
				state = state_search;
				break;
			}
		}


	}
	/*	while (true)
	{
		//USHORT adcValue = A2DConverter::getSample(14);
		//adcValue++;
		UBYTE numavg = 30;
		UINT adcValue = 0;

		for(UBYTE i=0;i< numavg;i++)
		{
			adcValue += IRB.ReadSensorAD();
		}
		adcValue = adcValue/numavg; //average numavg points 
		

		UBYTE adcIndex = UBYTE(adcValue % 10); //1s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			12
		);	
		adcIndex = UBYTE((adcValue/10) % 10); //10s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			8
		);
		adcIndex = UBYTE((adcValue/100) % 10); //100s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			4
		);
		adcIndex = UBYTE((adcValue/1000) % 10); //1000s
		DotMatrix::instance()->loadSprite(		
			sc_hexDigits_mini[adcIndex],
			SPRITE_ALPHANUMERIC_MINI_WIDTH,
			SPRITE_ALPHANUMERIC_MINI_HEIGHT,
			1,
			0
		);

		for(int del =0;del<10000;del++)
		{	for(int eel =0;eel<70;eel++)
			{}
		}

	}	*/

	

	return int(0);
}
