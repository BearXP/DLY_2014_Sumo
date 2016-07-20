#ifndef TARGETING_H
#define TARGETING_H
//includes
#include "standard.h"		//for standard types
#include "sumo_irsensor.h"	//for IRSensor interface
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
#include <stdlib.h>			//for abs()

using namespace Sumo;
const Byte NUM_SENSORS = 5;
//sensor positions relative to the center of the axle between the two wheels (or wherever the center of rotation is)
const SWord FRONT_X_OFFSET = 50;	//mm
const SWord FRONT_Y_OFFSET = 95;	//mm
const SWord SIDE_X_OFFSET = 95;		//mm
const SWord SIDE_Y_OFFSET = 70;		//mm
const SWord BACK_X_OFFSET = 0;		//mm
const SWord BACK_Y_OFFSET = 100;	//mm
const SWord TARGETING_DIST = 800;
const SWord NEAR_TARGET = 1000;
const SWord PUSH_DISTANCE = 400;
const SWord READING_ERROR = 400;

class Targeting
{
public:
	Targeting()
	{
	}
	Targeting(IRSensor * FrontLeft, IRSensor * FrontRight,
	          IRSensor * Left, IRSensor * Right, IRSensor * Back)
	{
		sensors[fLeft] = FrontLeft;
		sensors[fRight] = FrontRight;
		sensors[left] = Left;
		sensors[right] = Right;
		sensors[back] = Back;
	}

	~Targeting()
	{
	}
	void readSensors()
	{
		for (Byte i = 0; i < NUM_SENSORS; i++)
		{
			sensors[i]->ReadSensorAD();
		}
	}
	pos getTargetPos()				//returns relative x,y position of target in mm
	{
		pos retPos = {0,0};
		Word dist;		//distance in mm
		Byte closest = 0;
		Word value = sensors[closest]->getADValue();
		for (Byte i = 1; i < NUM_SENSORS; i++)
		{
			if (sensors[i]->getADValue()>value)
			{
				closest = i;
				value = sensors[closest]->getADValue();
			}
		}
		dist = sensors[closest]->ReadSensorDist();
		switch (closest)
		{
		  case fLeft:
		  {
			retPos.xpos = -FRONT_X_OFFSET;
			retPos.ypos = FRONT_Y_OFFSET+dist;
			break;
		  }
		  case fRight:
		  {
			retPos.xpos = FRONT_X_OFFSET;
			retPos.ypos = FRONT_Y_OFFSET+dist;
			break;
		  }
		  case left:
		  {
			retPos.xpos = -SIDE_X_OFFSET-dist;
			retPos.ypos = SIDE_Y_OFFSET;
			break;
		  }
 		  case right:
		  {
			retPos.xpos = SIDE_X_OFFSET+dist;
			retPos.ypos = SIDE_Y_OFFSET;
			break;
		  }
 		  case back:
		  {
			retPos.xpos = BACK_X_OFFSET;
			retPos.ypos = -BACK_Y_OFFSET-dist;
			break;
		  }
          default:  //something strange
		  {
		  }
		}
		return retPos;
	}
	bool targeted()
	{
		return (abs(sensors[fLeft]->ReadSensorDist())<TARGETING_DIST)&&
				(abs(sensors[fRight]->ReadSensorDist())<TARGETING_DIST);
    }
private:
	enum Sensors
	{
	  fLeft,
	  fRight,
	  left,
	  right,
	  back
	};
	IRSensor * sensors[NUM_SENSORS];
};
#endif // TARGETING_H
