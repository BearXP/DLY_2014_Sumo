#include "sumo_common.h"

#define IR_AD_MAX 14  //max IR AD Channel
#define IR_CALIB_MAX 1 //max IR Calib


namespace Sumo
{
	class IRSensor
	{
		private:
			UBYTE ADChannel;
			UBYTE IRCalibChan;  //0 or 1
			
		public:
			IRSensor();

			IRSensor(UBYTE channel)
			{ 
				SetADChannel(channel);
				SetIRCalib(0);				
			}

			void SetADChannel(UBYTE channel);
			void SetIRCalib(UBYTE arraynum);

			USHORT ReadSensorAD();
			USHORT ReadSensorDist();  //return the distance in millimeters

	};
}
