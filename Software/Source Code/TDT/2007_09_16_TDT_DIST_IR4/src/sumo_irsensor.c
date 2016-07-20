
#include "sumo_irsensor.h"
#include "sumo_adc.h"
#include "sumo_ircalib.h"

using namespace Sumo;

void IRSensor::SetADChannel(UBYTE channel)
{
	if(channel>=IR_AD_MIN && channel<=IR_AD_MAX)
	{
		ADChannel = channel;
	}
}

USHORT IRSensor::ReadSensorAD()
{
	return A2DConverter::getSample(ADChannel);
}

USHORT IRSensor::ReadSensorDist()  //return the distance in millimeters
{
	bool search =true;	
	USHORT ret =0;
	USHORT adsample = ReadSensorAD();
	FLOAT gradient =0;
	FLOAT calcDist=0;

	//find position in array that matches read in AD value.
	UBYTE count=0;
	do{
		if(adsample < ircalib_1[count][1])
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
		gradient = (static_cast<FLOAT>(ircalib_1[count][1])-
					static_cast<FLOAT>(ircalib_1[count-1][1]))/
					(static_cast<FLOAT>(ircalib_1[count][0])-
					static_cast<FLOAT>(ircalib_1[count-1][0]));

		calcDist = ((static_cast<FLOAT>(adsample)-static_cast<FLOAT>(ircalib_1[count-1][1]))/
		gradient) + static_cast<FLOAT>(ircalib_1[count-1][0]);
		ret = static_cast<USHORT>(calcDist); //return the previous value. TODO: interpolate value
	}

	return ret;
}
