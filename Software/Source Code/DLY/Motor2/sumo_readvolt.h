#include "standard.h"
#include "sumo_adc.h"

const Byte AD_batt = 8;
const Byte AD_12 = 9;
const Byte AD_5 = 10;

const Word AD_MAX = 1024;

const float vref = 5.0;  //5V AVCC

const float divider_batt = 122.0/22.0; //put in your resistor values here  R_total/R_lower
const float divider_12 = 69/22.0;
const float divider_5 = 27.6/22.0;

namespace Sumo
{
	class ReadVoltages
	{
		private:			
			SWord adin;
			SDWord VBat;
			SDWord V12;
			SDWord V5;
			
		public:
			ReadVoltages():
			VBat(0),
			V12(0),
			V5(0)
			{}
			void readVolts()
			{
				readVoltageBatt();
				readVoltage12();
				readVoltage5();
			}
			void readVoltageBatt()
			{
				static bool first = true;
				if (first)
				{
					adin = A2DConverter::getSample(AD_batt);	//prime VBatt
					VBat = adin<<8;
					first = false;
				}
				adin = A2DConverter::getSample(AD_batt);
				VBat += adin-(VBat>>8);
			}
			float GetVoltageBatt()
			{
				readVoltageBatt();
				return (VBat*divider_batt*vref/1024/256);
			}

			void readVoltage12()
			{
				static bool first = true;
				if (first)
				{
					adin = A2DConverter::getSample(AD_12);
					V12 = adin<<8;
					first = false;
				}
				adin = A2DConverter::getSample(AD_12);
				V12 += adin-(V12>>8);			
			}

			float GetVoltage12()
			{
				readVoltage12();				
				return (V12*divider_12*vref/1024/256);
			}

			void readVoltage5()
			{
				static bool first = true;
				if (first)
				{
					adin = A2DConverter::getSample(AD_5);
					V5 = adin<<8;
					first = false;
				}
				adin = A2DConverter::getSample(AD_5);
				V5 += adin-(V5>>8);			
			}

			float GetVoltage5()
			{
				readVoltage5();				
				return (V5*divider_5*vref/1024/256);
			}
	};
}
