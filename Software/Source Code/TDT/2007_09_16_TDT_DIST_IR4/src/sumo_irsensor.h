#include "sumo_common.h"

#define IR_AD_MAX 14  //max and min IR AD Channel
#define IR_AD_MIN 11

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

		public:
			typedef USHORT IRSensorCalibrationData[19][2];
			void loadCalibrationData(IRSensorCalibrationData& data)
			{
				for (int i = 0; i < 19; i++)
				{
					m_calib_data[i][0] = data[i][0];
					m_calib_data[i][1] = data[i][1];
				}
			}
		private:
			IRSensorCalibrationData m_calib_data;
	};
}
