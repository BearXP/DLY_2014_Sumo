#ifndef MOTOR_H
#define MOTOR_H
//includes
#include "standard.h"		//for standard types
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
#include <stdlib.h>			//for abs()
#include "sin.h"			//for sin, cos


class Motor
{
public:
	Motor():
	reversed(false),
	torque(0),
	fullStepping(0),
	pos(0),
	speed(0),
	accel(0),
	setAccel(0),
	setDeaccel(0),
	targetSpeed(0)
	{
		initialize();
	}
	~Motor()
	{
	}
	void update()
	{
		updateMotor(pos, speed, accel, targetSpeed, torque, phaseAOut, phaseBOut, fullTorque, fullStepping);
	}
	bool getReversed()
	{
		return reversed;
	}
	void setReversed(bool rev)
	{
		long speed = getTargetSpeed();
		reversed = rev;
		setTargetSpeed(speed);		//fix directions
	}
	long getCurrentSpeed()			//in microsteps/tick
	{
		long retSpeed;
		Byte sreg = SREG&0x80;
		cli();
		retSpeed = hiWord(speed);
		if (reversed)
		{
			retSpeed = -retSpeed;
		}
		SREG |= sreg;
		return retSpeed;
	}
	long getTargetSpeed()
	{
		long retSpeed;
		Byte sreg = SREG&0x80;
		cli();
		retSpeed = hiWord(targetSpeed);
		if (reversed)
		{
			retSpeed = -retSpeed;
		}
		SREG |= sreg;
		return retSpeed;
	}
	SWord getCurrentPosition()
	{
		SWord retPos;
		Byte sreg = SREG&0x80;
		cli();
		retPos = pos>>16;
		if (reversed)
		{
			retPos = -retPos;
		}
		SREG |= sreg;
		return retPos;
	}
	void setTargetSpeed(long setSpeed)	//in microsteps/tick
	{
		Byte sreg = SREG&0x80;
		cli();
		targetSpeed = setSpeed<<16;
		if (reversed)
		{
			targetSpeed = -targetSpeed;
		}
		if (abs(speed) < abs(targetSpeed))
		{
			//accelerating
			if (speed < targetSpeed)
			{
				accel = setAccel;
			} else
			{
				accel = -setAccel;
			}
		} else
		{
			if (speed < targetSpeed)
			{
				accel = setDeaccel;
			} else
			{
				accel = -setDeaccel;
			}
		}
		SREG |= sreg;
	}
	void setFullStepping(bool stepType)
	{
		Byte sreg = SREG&0x80;
		cli();
		fullStepping = stepType;
		SREG |= sreg;
	}
	bool getFullStepping()	//in microsteps/tick
	{
		return fullStepping;
	}
	long getAcceleration()
	{
		return setAccel;
	}
	long getDeacceleration()
	{
		return setDeaccel;
	}
	void setAcceleration(long a)  //in picosteps/tick/tick
	{
		Byte sreg = SREG&0x80;
		cli();
		setAccel = a;
		if (abs(speed) < abs(targetSpeed))
		{
			//accelerating
			if (speed < targetSpeed)
			{
				accel = setAccel;
			} else
			{
				accel = -setAccel;
			}
		} else
		{
			if (speed < targetSpeed)
			{
				accel = setDeaccel;
			} else
			{
				accel = -setDeaccel;
			}
		}
		SREG |= sreg;
	}
	void setDeacceleration(long a)  //in picosteps/tick/tick
	{
		Byte sreg = SREG&0x80;
		cli();
		setDeaccel = a;
		if (abs(speed) < abs(targetSpeed))
		{
			//accelerating
			if (speed < targetSpeed)
			{
				accel = setAccel;
			} else
			{
				accel = -setAccel;
			}
		} else
		{
			if (speed < targetSpeed)
			{
				accel = setDeaccel;
			} else
			{
				accel = -setDeaccel;
			}
		}
		SREG |= sreg;
	}
	void setTorque(Byte t)
	{
		const Word FULL_PLUS = 254;
		Byte sreg = SREG&0x80;
		cli();
		torque = t;
		fullTorque = hiByte(FULL_PLUS*(torque/2));
		SREG |= sreg;
	}
	Byte getTorque()
	{
		return torque;
	}

	void setPhaseRegisters(RegPointer a, RegPointer b)
	{
		Byte sreg = SREG&0x80;
		cli();
		phaseAOut = a;
		phaseBOut = b;
		SREG |= sreg;
	}
private:
	void initialize()
	{
	}
	bool reversed;
	RegPointer phaseAOut;		//pointer to PhaseA
	RegPointer phaseBOut;		//pointer to PhaseB
	Word torque;		//torque 0-255
	Byte fullTorque;
	bool fullStepping;	//full stepping
	long pos;			//current position in nanosteps
	long speed; 		//speed in picosteps/tick (256 picosteps = 1 nanostep, 256 nanosteps = 1 microstep, 256 microsteps = 1 step)
	long accel; 		//acceleration in picosteps/tick/tick
	long setAccel; 		//acceleration in picosteps/tick/tick
	long setDeaccel; 	//deacceleration in picosteps/tick/tick
	long targetSpeed;	//target speed (picosteps)

	static Byte* m_bufferPointer;
	static inline void updateMotor(long &pos, long &speed,
	                               long accel,long targetSpeed, Byte torque,
								   RegPointer phaseAOut, RegPointer phaseBOut,
								   Byte fullTorque, bool fullStepping)
	{
		const Byte HALFWAY = 128;
		long nextSpeed;
		DWord PhaseA, PhaseB;
		nextSpeed = speed + accel;		//accelerate
		if ((accel > 0) && (nextSpeed > targetSpeed))
		{
			speed = targetSpeed;		//reached target speed
		} else if ((accel < 0) && (nextSpeed < targetSpeed))
		{
			speed = targetSpeed;		//reached target speed
		} else
		{
			speed = nextSpeed;			//else update new speed 
		}
		pos += hiWord(speed);			//new position
		if (fullStepping)	//full stepping -> both phases always fully on
							//good for maximum torque at low speed
		{
			if (sinWord[hiByte(loWord(pos))]>0)
			{
				*phaseAOut = HALFWAY + fullTorque;
			} else
			{
				*phaseAOut = HALFWAY - fullTorque - 1;
			}
			if (cosWord[hiByte(loWord(pos))]>0)
			{
				*phaseBOut = HALFWAY + fullTorque;
			} else
			{
				*phaseBOut = HALFWAY - fullTorque - 1;
			}
		} else  //microstepping -> PhaseA^2 + PhaseB^2 is constant - constant torque
				//good for smooth, quiet stepping
		{
			PhaseA = sinWord[hiByte(loWord(pos))];
			PhaseA = (((PhaseA*torque)>>8)+255)/2;
			//PhaseA = (PhaseA+255)/2;
			*phaseAOut = loByte(PhaseA);
			PhaseB = cosWord[hiByte(loWord(pos))];
			PhaseB = (((PhaseB*torque)>>8)+255)/2;
			//PhaseB = (PhaseB+255)/2;
			*phaseBOut = loByte(PhaseB);
		}
	}

};
#endif // MOTOR_H
