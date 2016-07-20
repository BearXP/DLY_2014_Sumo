/*	Sumo Full Stepping motor class
 *  Based on Kens Motor class from 10-10-2007
 *  
 *  Stripped out the sine wave generation to only have it full step.
 *  It also changes the timer interval to change the velocity instead of 
 *  using a constant tick method that advances the position of the sine wave.
 */


#ifndef MOTOR_H
#define MOTOR_H
//includes
#include "standard.h"		//for standard types
#include "sumo_common.h"		//for standard types
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
#include <stdlib.h>			//for abs()
#include "sumo_timer.h"		//used to trigger the next step and set the speed
#include "sumo_dotmatrix.h"
//#include "sin.h"			//for sin, cos

//steps in full stepping sequence
const USHORT phaseASteps[4] = {0,0,1,1};
const USHORT phaseBSteps[4] = {0,1,1,0};

#define MOTOR_TOP	0x00FF  //for phase correct PWM frequency = 16MHz/2/MOTOR_TOP/Prescaler
#define ACC_RES		256;
using namespace Sumo;

class Motor
{
public:
	Motor():
	torque(0),
	pos(0),
	speed(0),
	accel(0),
	targetSpeed(0),
	accel_count(1),
	reverse(1)	
	{
		initialize();
	}
	~Motor()
	{
	}
	void step()
	{
		updateMotor(pos, speed, accel_count, accel, targetSpeed, torque, reverse, phaseAOut, phaseBOut,timerNumber);
	}
	long getCurrentSpeed()			//in microsteps/tick
	{
		long retSpeed;
		Byte sreg = SREG&0x80;
		cli();
		retSpeed = speed;
		SREG |= sreg;
		return retSpeed;
	}
	long getTargetSpeed()
	{
		long retSpeed;
		Byte sreg = SREG&0x80;
		cli();
		retSpeed = hiWord(targetSpeed);
		SREG |= sreg;
		return retSpeed;
	}
	SWord getCurrentPosition()
	{
		SWord retPos;
		Byte sreg = SREG&0x80;
		cli();
		retPos = pos>>16;
		SREG |= sreg;
		return retPos;
	}

	UBYTE getTimerNumber()
	{				
		return timerNumber;
	}

	void setTimerNumber(UBYTE theTimer)
	{
		timerNumber = theTimer;
	}
	void setTargetSpeed(long setSpeed)	//in steps/sec
	{
		Byte sreg = SREG&0x80;
		cli();
		targetSpeed = setSpeed;
		if (speed < targetSpeed)
		{
			accel = abs(accel);
		} else
		{
			accel = -abs(accel);
		}		
		SREG |= sreg;
	}	
	long getAcceleration()
	{
		return abs(accel);
	}
	void setAcceleration(long a)  //in steps/sec/sec
	{
		Byte sreg = SREG&0x80;
		cli();
		if (speed < targetSpeed)
		{
			accel = a;
		} else
		{
			accel = -a;
		}
		SREG |= sreg;
	}
	void setTorque(Byte t)
	{
		Byte sreg = SREG&0x80;
		cli();
		torque = t;
		SREG |= sreg;
	}
	void setPhaseRegisters(RegPointer16 a, RegPointer16 b)
	{
		Byte sreg = SREG&0x80;
		cli();
		phaseAOut = a;
		phaseBOut = b;
		SREG |= sreg;
	}
	void setReverse(bool rev)
	{
		if(rev)
			reverse = -1;
		else
			reverse = 1;
	}
private:
	void initialize()
	{
	}
	RegPointer16 phaseAOut;		//pointer to PhaseA
	RegPointer16 phaseBOut;		//pointer to PhaseB
	Byte torque;		//torque 0-255
	//bool fullStepping;	//always full stepping
	long pos;			//current position in the step array
	long speed; 		//speed in steps/second
	long accel; 		//acceleration in steps/sec/sec
	UBYTE accel_count;  //when accelerating slowly, count steps before incrementing speed.
	long targetSpeed;	//target speed (steps/sec)
	UBYTE timerNumber;  //which timer to use?
	BYTE reverse;		//step in opposite direction?

	

	//static Byte* m_bufferPointer;
static inline void updateMotor(long &pos, long &speed,UBYTE &accel_count,
	                               long accel,long targetSpeed, Byte torque, BYTE reverse,
								   RegPointer16 phaseAOut, RegPointer16 phaseBOut,
								   UBYTE timerNumber)
	{
		const Word FULL_PLUS = 254;
		const Byte HALFWAY = 128;
		long nextSpeed;
		long diff;
		Byte fullTorque;
		//SDWord PhaseA, PhaseB;
		long s = speed;
		UBYTE a = accel_count;
		
		if(speed >0)
		{
			diff = (accel*accel_count)/speed;  //relative to interval between steps
		}
		else
		{
			diff = accel; //otherwise just increment speed 
		}

		

		if(diff!=0)
		{
			accel_count = 1;

			nextSpeed = speed + diff;
			if ((accel > 0) && (nextSpeed > targetSpeed))
			{
				speed = targetSpeed;
			} else if ((accel < 0) && (nextSpeed < targetSpeed))
			{
				speed = targetSpeed;
			} else
			{
				speed = nextSpeed;
			}

			Timer::instance(timerNumber)->setFrequency(speed);
		}
		else
		{
			accel_count++;
		}
	


		pos += reverse;
		pos = pos&0x03;
		if(pos<0)
			pos = 3;
	
		fullTorque = hiByte(FULL_PLUS*(torque/2));


		if (phaseASteps[pos]==1)
		{
			*phaseAOut = fullTorque+HALFWAY;
		} 
		else
		{
			*phaseAOut = HALFWAY-fullTorque-1;
		}
	
		if (phaseBSteps[pos]==1)
		{
			*phaseBOut = fullTorque+HALFWAY;
		} 
		else
		{
			*phaseBOut = HALFWAY-fullTorque-1;
		}
		
		
	}

};
#endif // MOTOR_H
