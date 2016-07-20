#ifndef __SUMOBOT_SUMO_TIMER_H__
#define __SUMOBOT_SUMO_TIMER_H__

#include "sumo_common.h"

// define number of timers
#define TIMER_INSTANCES			6

namespace Sumo
{
	class Timer
	{
	// type definition for a void function pointer
	typedef void (FunctionPtr)(void);

	// instance management and private constructors
	private:
		Timer(UBYTE timer);
		UBYTE m_timer;
	private:
		static Timer* sm_timers[TIMER_INSTANCES];
	public:
		static Timer* instance(UBYTE timer);

	// global timer set frequency
	private:
		static BOOL internalSetFrequency(UBYTE timer, UINT frequency);

	// global timer start/stop
	private:
		static void internalStart(UBYTE timer);
		static void internalStop(UBYTE timer);

	// timer timer frequency management
	public:
		BOOL setFrequency(UINT frequency);
		UINT getFrequency();
	private:
		UINT m_frequency;

	// timer start/stop
	public:
		void start();
		void stop();

	// timer trigger management
	public:
		void setFunctionPtr(FunctionPtr funcptr);
		void clearFunctionPtr();
		void execFunctionPtr();
	private:
		FunctionPtr* m_funcptr;
	};
};

#endif // __SUMOBOT_SUMO_TIMER_H__
