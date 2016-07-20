#ifndef SUMO_IRSENSOR_H
#define SUMO_IRSENSOR_H
#include "standard.h"

namespace Sumo
{
	class IRSensor
	{
		private:
			Byte ADChannel;
			SDWord ADValue;
			
		public:
			IRSensor();

			IRSensor(Byte channel) :
			ADValue(0)
			{ 
				SetADChannel(channel);				
			}

			void SetADChannel(Byte channel);

			Word ReadSensorAD();
			Word getADValue()
			{
				return (Word)ADValue;
			}
			Word ReadSensorDist();  //return the distance in millimeters

	};
}
#endif	//SUMO_IRSENSOR_H
