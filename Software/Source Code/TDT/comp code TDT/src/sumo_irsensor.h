#include "sumo_common.h"

#define IR_AD_MAX 14  //max IR AD Channel


namespace Sumo
{
	class IRSensor
	{
		private:
			UBYTE ADChannel;
			
		public:
			IRSensor();

			IRSensor(UBYTE channel)
			{ 
				SetADChannel(channel);				
			}

			void SetADChannel(UBYTE channel);

			USHORT ReadSensorAD();
			USHORT ReadSensorDist();  //return the distance in millimeters

	};
}
