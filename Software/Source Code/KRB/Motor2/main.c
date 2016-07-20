
#include "motor2.h"
#include "inttimer.h"
#include "sumo_ports.h"
#include "dotmatrix.h"
#include "pwm.h"
#include "adpcm.h"
#include "data.h"
#include "sprites.h"
#include "navigate.h"
#include "targeting.h"
#include "sumo_adc.h"
#include "sumo_irsensor.h"
#include "sumo_readvolt.h"


using namespace Sumo;

enum States
{
	INITIAL,
	COUNTDOWN,
	FIND_EDGE,
	GO_FORWARD,
	SEARCH_TARGET,
	FOUND_TARGET,
	PUSHING,
	BACKOFF,
	DEBUG
};

const Word ONE_SECOND = 1000;
const Word FIVE_SECONDS = 5*ONE_SECOND;
const Word THREE_SECONDS = 3*ONE_SECOND;
const Word SIX_SECONDS = 6*ONE_SECOND;

A2DConverter ADCs;
ReadVoltages volts;
IRSensor IRFrontLeft(14);  //create IRSensor to use ADC channel 14 and ircalib_1
IRSensor IRFrontRight(13);
IRSensor IRLeft(12);
IRSensor IRRight(11);
IRSensor IRBack(0);
Motor leftMotor;
Motor rightMotor;
DotMatrix matrix;
PWM pwms;
IntTimer timer(0);
ADPCM sound;
Navigate navigator(&leftMotor, &rightMotor);
Targeting target(&IRFrontLeft, &IRFrontRight, &IRLeft, &IRRight, &IRBack);


void updates()			//run state machines
{
	static Byte count = 0;
	navigator.update();
	if (count == 16)
	{
		matrix.update();
		count = 0;
	}
	//sound.update();
	count++;
}

int main()
{
	States state = INITIAL;
	pos targetPos;
	//setup
	Setup_Sumo_Ports();
	timer.setFrequency(6000);
	timer.setFunctionCall(updates);
	timer.start();
	leftMotor.setPhaseRegisters(&LEFT_PHASE_A, &LEFT_PHASE_B);
	rightMotor.setPhaseRegisters(&RIGHT_PHASE_A, &RIGHT_PHASE_B);
	rightMotor.setReversed(true);
	A2DConverter::enable();
	//sound.setDataPointer(packed, sizeof(packed));
	//sound.fillBuffer();
	//sound.go();
	sei();	//let's start

	do
	{
		//do background tasks
		//sound.pumpBuffer();
		target.readSensors();
		volts.readVolts();
		//end do background tasks

		if (getSwitch(1))		//switch to DEBUG and back
		{
			if (state != DEBUG)
			{
				navigator.stop();
				matrix.clear();
				state = DEBUG;
			}
		} else if (state == DEBUG)
		{
			matrix.clear();
			state = INITIAL;
		}
		//do state machine
		switch (state)
		{
			case INITIAL:
			{
				timer.setCountDownTime(FIVE_SECONDS);		//initial countdown time
				matrix.clear();
				state = COUNTDOWN;
				break;
			}
			case COUNTDOWN:
			{
				matrix.showHex((timer.getCountDownTime()/ONE_SECOND)+1);
				if (timer.getCountDownTime()==0)
				{
					matrix.clear();
					navigator.setTorque(HALF_TORQUE);
					leftMotor.setAcceleration(300000); //picosteps/tick/tick
					leftMotor.setDeacceleration(500000); //picosteps/tick/tick
					rightMotor.setAcceleration(300000);
					rightMotor.setDeacceleration(500000); //picosteps/tick/tick
					state = SEARCH_TARGET;
				}
				break;
			}
			case FIND_EDGE:
			{
				navigator.creepBack();
				if (navigator.isOnLine())
				{
					navigator.creepForward();
					state = GO_FORWARD;
				}
				break;
			}
			case GO_FORWARD:
			{
				if (!navigator.isOnLine())
				{
					navigator.refBack();
					navigator.approach();
					navigator.setTorque(HALF_TORQUE);
					state = SEARCH_TARGET;
				}
				break;
			}
			case SEARCH_TARGET:
			{
				matrix.showInt(abs(leftMotor.getCurrentSpeed())>>8);  //DEBUG
				if (!target.targeted())
				{
				  targetPos = target.getTargetPos();		  //get relative target position
				  if (((abs(targetPos.xpos) < NEAR_TARGET) || (abs(targetPos.ypos) < NEAR_TARGET))
				     && !navigator.isTurning())
				  { 
					if (targetPos.ypos < 0)				//behind you
					{
						navigator.turn(128);
					} else if (targetPos.xpos < -200)	//left
					{
						navigator.turn(192);
					} else if (targetPos.xpos > 200)	//right
					{
						navigator.turn(64);
					} else if (targetPos.xpos < 0)		//front left
					{
						navigator.turn(245);
					} else								//front right
					{	
					    //navigator.approach();
						navigator.turn(10);
					}
				  }
				} else	//found something
				{
					navigator.creepForward();
					state = FOUND_TARGET;
				}
				break;
			}
			case FOUND_TARGET:
			{
				targetPos = target.getTargetPos();		  //get relative target position
			    if ((targetPos.ypos > (TARGETING_DIST+READING_ERROR))||(targetPos.ypos < 0)
				    ||(abs(targetPos.xpos) > 100))	//lost target
				{
					navigator.setTorque(HALF_TORQUE);
					navigator.unapproach();
					state = SEARCH_TARGET;
				} else if ((targetPos.ypos < PUSH_DISTANCE)&&(abs(targetPos.xpos) < 100)
				        &&(targetPos.ypos>0))  //close enough to push
				{
					navigator.setTorque(MAX_TORQUE);
					timer.setCountDownTime(SIX_SECONDS);
					navigator.push(3000);
					state = PUSHING;
				}
				break;
			}
			case PUSHING:
			{
				targetPos = target.getTargetPos();		  //get relative target position
				//make sure target is still there
				if ((timer.getCountDownTime()==0)||
				    ((abs(targetPos.ypos) > (SWord)TARGETING_DIST)||(abs(targetPos.xpos) > 100)
				        ||(targetPos.ypos<0)))
				{
					navigator.backoff(4000);
					timer.setCountDownTime(ONE_SECOND);
					state = BACKOFF;
				} else if (target.targeted())
				{
					timer.setCountDownTime(max((DWord)THREE_SECONDS,timer.getCountDownTime()));  //still pushing
				}
				break;
			}
			case BACKOFF:
			{
				if (timer.getCountDownTime()==0)
				{
					navigator.stop();
					navigator.setTorque(HALF_TORQUE);
					state = SEARCH_TARGET;
				}
				break;
			}
			case DEBUG:
			{
				if (getSwitch(2))
				{
					matrix.showFloat(volts.GetVoltageBatt(),2);
				} else
				{
					matrix.showHex(leftFront()+2*rightFront()+4*leftBack()+8*rightBack());
				}
				break;
			}
		}
	}
	while (true);
	return int(0);
}
