#ifndef NAVIGATE_H
#define NAVIGATE_H
//includes
#include "standard.h"		//for standard types
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
#include <stdlib.h>			//for abs()
#include <math.h>			//for sqrt()
#include "motor2.h"         //uses motor interface

const Word RING_SIZE = 1800;	//in mm
const DWord PERIMETER = RING_SIZE*RING_SIZE;
const Word NORTH = 0;
const Word SOUTH = WORD_MAX/2;
const DWord PI_16 = 205887;
const Word DEFAULT_WHEEL_DIAMETER = 50;	//mm
const Word DEFAULT_WHEEL_SPACING = 150;	//mm
//const Word TURN10_SPEED = 800;
const Word TURN90_SPEED = 2300;
//const Word TURN180_SPEED = Word(((long)TURN90_SPEED*1414)/1000);
const long TURN_ACCEL = 100000;
const long MAX_DEACCEL = 550000;
const SWord APPROACH_SPEED = 1500;
const Word APPROACH_INCREMENT = 5;
const SWord ATTACK_SPEED = 4000;
const Word ATTACK_INCREMENT = 100;
const SWord CREEP_SPEED = 1000;
const Byte MAX_TORQUE = 255;
const Byte HALF_TORQUE = 128;
const Word WHEEL_K = 212;

class Navigate
{
public:
	Navigate()
	{
		initialize();
	}
	Navigate(Motor * left, Motor * right)
	{
		lMotor = left;
		rMotor = right;
		initialize();
	}

	~Navigate()
	{
	}

	SWord turnSpeed(Byte angle, long accel)
	{
		long speed;
		if (angle <= 128)
		{
			speed = isqrt(WHEEL_K*(accel>>8)*angle);
		} else
		{
			speed = -isqrt(WHEEL_K*(accel>>8)*(256-angle));
		}
		return (SWord)speed;	
	}
	void saveState()
	{
		setlSpeed = lMotor->getTargetSpeed();
		setrSpeed = rMotor->getTargetSpeed();
		setlAccel = lMotor->getAcceleration();
		setrAccel = rMotor->getAcceleration();
		setlDeaccel = lMotor->getDeacceleration();
		setrDeaccel = rMotor->getDeacceleration();
	}
	void turn(Byte direction)
	{
		SWord ladjust = 0;
		SWord radjust = 0;
		if (turning)
		{
			return;
		}
		saveState();
		ladjust = turnSpeed(direction, TURN_ACCEL);
		radjust = -ladjust;
		/*switch (direction)
		{
			case 10:
			{
				ladjust = TURN10_SPEED;
				radjust = -TURN10_SPEED;
				break;
			}
			case 245:
			{
				ladjust = -TURN10_SPEED;
				radjust = TURN10_SPEED;
				break;
			}
			case 64:
			{
				ladjust = TURN90_SPEED;
				radjust = -TURN90_SPEED;
				break;
			}
			case 192:
			{
				ladjust = -TURN90_SPEED;
				radjust = TURN90_SPEED;
				break;
			}
			case 128:
			{
				ladjust = TURN180_SPEED;
				radjust = -TURN180_SPEED;
				break;
			}
			default:
			{
			}
		}*/
		lMotor->setAcceleration(TURN_ACCEL);
		rMotor->setAcceleration(TURN_ACCEL);
		lMotor->setDeacceleration(TURN_ACCEL);
		rMotor->setDeacceleration(TURN_ACCEL);
		lMotor->setTargetSpeed(lMotor->getCurrentSpeed()+ladjust);
		rMotor->setTargetSpeed(rMotor->getCurrentSpeed()+radjust);
		turning = true;
		turnup = true;
	}
	void stop()
	{
		lMotor->setTargetSpeed(0);
		rMotor->setTargetSpeed(0);
		turnup = false;
		turning = false;
	}
	void approach()
	{
		long lspeed = lMotor->getTargetSpeed() + APPROACH_INCREMENT;
		if (lspeed > APPROACH_SPEED)
		{
			lspeed = APPROACH_SPEED;
		}
		lMotor->setTargetSpeed(lspeed);
		rMotor->setTargetSpeed(lspeed);
	}
	void attack()
	{
		long lspeed = lMotor->getTargetSpeed() + ATTACK_INCREMENT;
		if (lspeed > ATTACK_SPEED)
		{
			lspeed = ATTACK_SPEED;
		}
		lMotor->setTargetSpeed(lspeed);
		rMotor->setTargetSpeed(lspeed);
	}
	void unapproach()
	{
		long lspeed = lMotor->getTargetSpeed() - APPROACH_INCREMENT;
		if (lspeed < 0)
		{
			lspeed = 0;
		}
		lMotor->setTargetSpeed(lspeed);
		rMotor->setTargetSpeed(lspeed);
	}
	void creepBack()
	{
		lMotor->setTargetSpeed(-CREEP_SPEED);
		rMotor->setTargetSpeed(-CREEP_SPEED);
	}
	void creepForward()
	{
		lMotor->setTargetSpeed(CREEP_SPEED);
		rMotor->setTargetSpeed(CREEP_SPEED);
	}
	void push(Word pushSpeed)
	{
		setTorque(MAX_TORQUE);
		setFullStepping(true);
		lMotor->setTargetSpeed(pushSpeed);
		rMotor->setTargetSpeed(pushSpeed);
	}
	void setTorque(Byte torque)
	{
		lMotor->setTorque(torque);
		rMotor->setTorque(torque);
	}
	void setFullStepping(bool fullStepping)
	{
		lMotor->setFullStepping(fullStepping);
		rMotor->setFullStepping(fullStepping);
	}
	void backoff(Word backoffSpeed)
	{
		setFullStepping(false);
		lMotor->setTargetSpeed(-backoffSpeed);
		rMotor->setTargetSpeed(-(backoffSpeed/2));
	}
	pos getPos()
	{
		//convert something
		return position;
	}

	vel getVel()
	{
		//convert from microsteps/tick to mm/sec
		return velocity;
	}
	Byte getDirection()
	{
		DWord retDir;
		Byte sreg = SREG&0x80;
		cli();
		retDir = direction<<8;
		SREG |= sreg;
		//convert from to 0..255
		return retDir/TwoPiW;
	}
	void updatePos()
	{
		//long dd;	//relative distance moved
		long dl, dr;//relative rotation of each wheel
		//long dx, dy;//relative x, y movement cw. straight ahead
		long lpos = lMotor->getCurrentPosition();
		long rpos = rMotor->getCurrentPosition();
		dl = lpos - lastLpos;						//relative movement of both wheels since last update
		dr = rpos - lastRpos;
		lastLpos = lpos;							//remember position for next time
		lastRpos = rpos;
		direction += dl - dr;						//update direction
		if (direction > TwoPiW)						//limit rotation
		{
			direction -= TwoPiW;
		} else if (direction < -TwoPiW)
		{
			direction += TwoPiW;
		}

		//dd = (dl+dr+1)>>1;							//straight line distance
		//update position
		//dx = (dl*dl-dr*dr)/W;						//X deviation from straight ahead
		//dy = dd - (dx/W+1)>>1;						//Y straight ahead movement

	}
	void update()
	{
		const Byte UPDATE_COUNT = 20;
		static Byte positionUpdate = 0;
		static bool left = true;
		static bool lFull;
		static bool rFull;
		static Byte lTorque;
		static Byte rTorque;
		static long lDeaccel;
		static long rDeaccel;
		//check line sensors
		bool offLine = true;
		if (leftFront())
		{
			offLine = false;
			if (!onLine&&!referencing)	//then do something
			{
				lFull = lMotor->getFullStepping();
				rFull = rMotor->getFullStepping();
				lTorque = lMotor->getTorque();
				rTorque = rMotor->getTorque();
				lDeaccel = lMotor->getDeacceleration();
				rDeaccel = rMotor->getDeacceleration();
				setFullStepping(true);
				setTorque(MAX_TORQUE);
				lMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setTargetSpeed(rMotor->getCurrentSpeed()-TURN90_SPEED);
				lMotor->setTargetSpeed(0);
			}
		}
		if (rightFront())
		{
			offLine = false;
			if (!onLine&&!referencing)	//then do something
			{
				lFull = lMotor->getFullStepping();
				rFull = rMotor->getFullStepping();
				lTorque = lMotor->getTorque();
				rTorque = rMotor->getTorque();
				lDeaccel = lMotor->getDeacceleration();
				rDeaccel = rMotor->getDeacceleration();
				setFullStepping(true);
				setTorque(MAX_TORQUE);
				lMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setDeacceleration(MAX_DEACCEL);
				lMotor->setTargetSpeed(lMotor->getCurrentSpeed()-TURN90_SPEED);
				rMotor->setTargetSpeed(0);
			}
		}
		if (leftBack()&&offLine)	//can't have front and back sensors on
		{
			offLine = false;
			if (!onLine&&!referencing)	//then do something
			{
				lFull = lMotor->getFullStepping();
				rFull = rMotor->getFullStepping();
				lTorque = lMotor->getTorque();
				rTorque = rMotor->getTorque();
				lDeaccel = lMotor->getDeacceleration();
				rDeaccel = rMotor->getDeacceleration();
				setFullStepping(true);
				setTorque(MAX_TORQUE);
				lMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setTargetSpeed(rMotor->getCurrentSpeed()+TURN90_SPEED);
				lMotor->setTargetSpeed(0);
			}
		}
		if (rightBack()&&offLine)	//can't have front and back sensors on
									//and just rotate if both back sensors on
		{
			offLine = false;
			if (!onLine && !referencing)	//then do something
			{
				lFull = lMotor->getFullStepping();
				rFull = rMotor->getFullStepping();
				lTorque = lMotor->getTorque();
				rTorque = rMotor->getTorque();
				lDeaccel = lMotor->getDeacceleration();
				rDeaccel = rMotor->getDeacceleration();
				setFullStepping(true);
				setTorque(MAX_TORQUE);
				lMotor->setDeacceleration(MAX_DEACCEL);
				rMotor->setDeacceleration(MAX_DEACCEL);
				lMotor->setTargetSpeed(lMotor->getCurrentSpeed()+TURN90_SPEED);
				rMotor->setTargetSpeed(0);
			}
		}


		if (onLine && offLine && !referencing)	//now off line - emergency over - restore defaults
		{
			lMotor->setFullStepping(lFull);
			rMotor->setFullStepping(rFull);
			lMotor->setTorque(lTorque);
			rMotor->setTorque(rTorque);
			lMotor->setDeacceleration(lDeaccel);
			rMotor->setDeacceleration(rDeaccel);
			lMotor->setTargetSpeed(0);
			rMotor->setTargetSpeed(0);
			onLine = false;
		} else if (!offLine)
		{
			onLine = true;
		}
		//end check line sensors
		//move motors
		if (left)
		{
			lMotor->update();
		} else
		{
			rMotor->update();
		}
		left = !left;
		//end move motors
		//update position
		positionUpdate++;
		if (positionUpdate == UPDATE_COUNT)
		{
			updatePos();
			//turn
			if (turnup)
			{
				if (lMotor->getTargetSpeed()==lMotor->getCurrentSpeed())
				{
					lMotor->setTargetSpeed(setlSpeed);
					rMotor->setTargetSpeed(setrSpeed);
					turnup = false;
					turndown = true;
				}
			}
			if (turndown)
			{
				if (lMotor->getTargetSpeed()==lMotor->getCurrentSpeed())
				{
					lMotor->setAcceleration(setlAccel);
					rMotor->setAcceleration(setrAccel);
					lMotor->setDeacceleration(setlDeaccel);
					rMotor->setDeacceleration(setrDeaccel);
					turndown = false;
					turning = false;;
				}
			}
			positionUpdate = 0;
		}
		//end update position
	}
	bool stopped()
	{
		return 	(lMotor->getCurrentSpeed()==0)&&(lMotor->getCurrentSpeed()==0);
	}

	bool isOnLine()
	{
		return onLine;
	}
	bool isTurning()
	{
		return turning;
	}
	void refBack()
	{
		position.xpos = 0;
		position.ypos = (-RING_SIZE/2)<<8;
		direction = NORTH;
		referencing = false;
	}
	void refForward()
	{
		position.xpos = 0;
		position.ypos = (RING_SIZE/2)<<8;
		direction = NORTH;
		referencing = false;
	}
private:
	bool referencing;
	volatile bool onLine;
	long setlSpeed;
	long setrSpeed;
	long setlAccel;
	long setrAccel;
	long setlDeaccel;
	long setrDeaccel;

	bool turnup;			//accelerating into turn
	bool turndown;			//deaccelerating from turn
	volatile bool turning;	//set true if turning
	long TwoPiW;			//2*Pi*Wheel Width
	Motor * lMotor;			//motors
	Motor * rMotor;			//motors
	pos position;			//position in mm/256
	long lastLpos;			//position in microsteps
	long lastRpos;			//position in microsteps
	vel velocity;			//speed in mm/sec/256
	Word direction;			//direction - 16384 = 90 degrees
	DWord gearing;			//microsteps/meter
	Word W;					//wheel spacing in mm
	Word CF;				//conversion factor 2*PI*W*255
	void initialize()
	{
		referencing = true;
		onLine = false;
		turning = false;
		turnup = false;
		turndown = false;
		lastLpos = 0;
		lastRpos = 0;
		position.xpos = 0;
		position.ypos = 0;
		velocity.xvel = 0;
		velocity.yvel = 0;
		direction = 0;
		W = DEFAULT_WHEEL_SPACING;
		TwoPiW = (PI_16*W)>>7;
		CF = (PI_16*W)>>7;
	}
};
#endif // NAVIGATE_H
