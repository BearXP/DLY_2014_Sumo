#include "sumo_common.h"
#include "sumo_adc.h"

const UBYTE AD_batt = 8;
const UBYTE AD_12 = 9;
const UBYTE AD_5 = 10;

const USHORT AD_MAX = 1024;

const FLOAT vref = 5.0;  //5V AVCC

const FLOAT divider_batt = 122.0/22.0; //put in your resistor values here  R_total/R_lower
const FLOAT divider_12 = 69.0/22.0;
const FLOAT divider_5 = 27.6/22.0;

namespace Sumo
{
	class ReadVoltages
	{
		private:			
			USHORT adin;
			
		public:
			ReadVoltages()
			{}
	
			FLOAT GetVoltageBatt()
			{
				adin = A2DConverter::getSample(AD_batt);				
				return (adin*divider_batt*vref/1024);
			}

			FLOAT GetVoltage12()
			{
				adin = A2DConverter::getSample(AD_12);				
				return (adin*divider_12*vref/1024);
			}

			FLOAT GetVoltage5()
			{
				adin = A2DConverter::getSample(AD_5);				
				return (adin*divider_5*vref/1024);
			}
	};
}
