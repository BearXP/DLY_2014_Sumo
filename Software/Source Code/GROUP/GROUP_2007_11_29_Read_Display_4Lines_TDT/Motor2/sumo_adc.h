#ifndef __SUMOBOT_SUMO_ADC_H__
#define __SUMOBOT_SUMO_ADC_H__
#include "standard.h"

namespace Sumo
{
	class A2DConverter
	{
	// enable/disable adc
	public:
		static void enable();
		static void disable();

	// get sample
	public:
		static Word getSample(Byte input);
	};
};

#endif // __SUMOBOT_SUMO_ADC_H__
