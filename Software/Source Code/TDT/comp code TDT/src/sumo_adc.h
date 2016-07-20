#ifndef __SUMOBOT_SUMO_ADC_H__
#define __SUMOBOT_SUMO_ADC_H__

#include "sumo_common.h"

// define number of timers
#define ADC_INPUTS			4

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
		static USHORT getSample(UBYTE input);
	};
};

#endif // __SUMOBOT_SUMO_ADC_H__
