#include "standard.h"
#include "sumo_irsensor.h"
#include "sumo_adc.h"
#include "sumo_ircalib.h"

using namespace Sumo;

void IRSensor::SetADChannel(Byte channel)
{
		ADChannel = channel;
}

Word IRSensor::ReadSensorAD()
{
	SWord ADReading = A2DConverter::getSample(ADChannel);
	if (ADValue == 0)
	{
		ADValue = ADReading<<4;
	} else
	{
		ADValue += ADReading-(ADValue>>4);			//smooth reading
	}
	return ADValue>>4;
}

Word IRSensor::ReadSensorDist()  //return the distance in millimeters
{
	bool search =true;
	Word ret =0;
	float gradient =0;
	float calcDist=0;

	//find position in array that matches read in AD value.
	Byte count=0;
	do{
		if(Word(ADValue>>4) < ircalib_1[count][1])
		{
			count++;

			if(count>=19)
				search = false;
		}
		else
		{
			search=false;
		}		

	}while(search);

	if(count==0)//special cases
	{
		ret = ircalib_1[count][0];
	}
	else if (count == 19)
	{
		ret = ircalib_1[count-1][0]; //return the previous value.
	}
	else  //interpolate the values.
	{
		gradient = (static_cast<float>(ircalib_1[count][1])-
					static_cast<float>(ircalib_1[count-1][1]))/
					(static_cast<float>(ircalib_1[count][0])-
					static_cast<float>(ircalib_1[count-1][0]));

		calcDist = ((static_cast<float>(ADValue>>4)-static_cast<float>(ircalib_1[count-1][1]))/
		gradient) + static_cast<float>(ircalib_1[count-1][0]);
		ret = static_cast<Word>(calcDist); //return the previous value. TODO: interpolate value
	}

	return ret;
}
