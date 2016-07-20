#include <avr/io.h>
#include <avr/interrupt.h>
#include "sumo_timer.h"

using namespace Sumo;

Timer* Timer::sm_timers[TIMER_INSTANCES] = {NULL};

Timer::Timer(UBYTE timer)
{
	m_timer = timer;
	m_funcptr = NULL;
	setFrequency(1);
	stop();
}

Timer* Timer::instance(UBYTE timer)
{
	// check if instance has been created. if
	// not, create the instance
	if (sm_timers[timer] == NULL)
	{
		sm_timers[timer] = new Timer(timer);
	}
	
	// return appropriate instance
	return sm_timers[timer];
}

BOOL Timer::internalSetFrequency(UBYTE timer, UINT frequency)
{
	// use a 1/64 clock divider
	UBYTE clockDivider = 64;

	// determine compare value with this frequency
	UINT compareValue = (FREQ_OSC/clockDivider)/frequency;

	// is the compare value valid?
	// note: the oddity in this situation is that timers 0 and 2 only
	// accepts 8-bit compare values while other timers can happily accept
	// 16-bit values
	if (compareValue == 0)
	{
		return false;
	}
	if (timer == 0 || timer == 2)
	{
		if (compareValue > 0xFF)
		{
			return false;
		}
	}
	if (compareValue > 0xFFFF)
	{
		return false;
	}

	// set the compare value in the register
	switch (timer)
	{
		case 0:
		{
			// clear and set the clock select options to 1/64
			// by setting CSn2:0 to 0x03
			TCCR0B &= ~(_BV(CS02) | _BV(CS01) | _BV(CS00));
			TCCR0B |= (_BV(CS01) | _BV(CS00));
			// then set the output compare register A
			OCR0A = compareValue & 0xFF;
			break;
		}
		case 1:
		{
			// do the same as above, except split compare value
			// into two chunks of 8-bits
			TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
			TCCR1B |= (_BV(CS11) | _BV(CS10));
			OCR1AL = compareValue & 0xFF;
			OCR1AH = (compareValue >> 8) & 0xFF;
			break;
		}
		case 2:
		{
			// do the same as case for timer == 0
			TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));
			TCCR2B |= (_BV(CS21) | _BV(CS20));
			OCR2A = compareValue & 0xFF;
			break;
		}
		case 3:
		{
			// do the same as case for timer == 2
			TCCR3B &= ~(_BV(CS32) | _BV(CS31) | _BV(CS30));
			TCCR3B |= (_BV(CS31) | _BV(CS30));
			OCR3AL = compareValue & 0xFF;
			OCR3AH = (compareValue >> 8) & 0xFF;
			break;
		}
		case 4:
		{
			TCCR4B &= ~(_BV(CS42) | _BV(CS41) | _BV(CS40));
			TCCR4B |= (_BV(CS41) | _BV(CS40));
			OCR4AL = compareValue & 0xFF;
			OCR4AH = (compareValue >> 8) & 0xFF;
			break;
		}
		case 5:
		{
			TCCR5B &= ~(_BV(CS52) | _BV(CS51) | _BV(CS50));
			TCCR5B |= (_BV(CS51) | _BV(CS50));
			OCR5AL = compareValue & 0xFF;
			OCR5AH = (compareValue >> 8) & 0xFF;
			break;
		}
	}

	return true;
}

void Timer::internalStart(UBYTE timer)
{
	switch (timer)
	{
		case 0:
		{
			// clear the output compare register A on a successful
			// compare by setting COMnA1:0 to 0x02
			TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));
			TCCR0A |= _BV(COM0A1);
			// enable interrupt for output compare A
			TIMSK0 |= _BV(OCIE0A);
			break;
		}
		case 1:
		{
			// same as above
			TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
			TCCR1A |= _BV(COM1A1);
			TIMSK1 |= _BV(OCIE1A);
			break;
		}
		case 2:
		{
			TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
			TCCR2A |= _BV(COM2A1);
			TIMSK2 |= _BV(OCIE2A);
			break;
		}
		case 3:
		{
			TCCR3A &= ~(_BV(COM3A1) | _BV(COM3A0));
			TCCR3A |= _BV(COM3A1);
			TIMSK3 |= _BV(OCIE3A);
			break;
		}
		case 4:
		{
			TCCR4A &= ~(_BV(COM4A1) | _BV(COM4A0));
			TCCR4A |= _BV(COM4A1);
			TIMSK4 |= _BV(OCIE4A);
			break;
		}
		case 5:
		{
			TCCR5A &= ~(_BV(COM5A1) | _BV(COM5A0));
			TCCR5A |= _BV(COM5A1);
			TIMSK5 |= _BV(OCIE5A);
			break;
		}
	}
}

void Timer::internalStop(UBYTE timer)
{
	switch (timer)
	{
		case 0:
		{
			// disable interrupt for output compare A
			TIMSK0 &= ~(_BV(OCIE0A));
			break;
		}
		case 1:
		{
			// same as above
			TIMSK1 &= ~(_BV(OCIE1A));
			break;
		}
		case 2:
		{
			TIMSK2 &= ~(_BV(OCIE2A));
			break;
		}
		case 3:
		{
			TIMSK3 &= ~(_BV(OCIE3A));
			break;
		}
		case 4:
		{
			TIMSK4 &= ~(_BV(OCIE4A));
			break;
		}
		case 5:
		{
			TIMSK5 &= ~(_BV(OCIE5A));
			break;
		}
	}
}

BOOL Timer::setFrequency(UINT frequency)
{
	BOOL success = internalSetFrequency(m_timer, frequency);
	if (success)
	{
		m_frequency = frequency;
	}

	return success;
}

UINT Timer::getFrequency()
{
	return m_frequency;
}

void Timer::start()
{
	return internalStart(m_timer);
}

void Timer::stop()
{
	return internalStop(m_timer);
}

void Timer::setFunctionPtr(FunctionPtr funcptr)
{
	m_funcptr = funcptr;
}

void Timer::clearFunctionPtr()
{
	m_funcptr = NULL;
}

void Timer::execFunctionPtr()
{
	if (m_funcptr != NULL)
	{
		m_funcptr();
	}
}

// vectors!!!

ISR(TIMER0_COMPA_vect)
{
	Timer::instance(0)->execFunctionPtr();
}

/*ISR(TIMER1_COMPA_vect)
{
	Timer::instance(1)->execFunctionPtr();
}*/

ISR(TIMER2_COMPA_vect)
{
	Timer::instance(2)->execFunctionPtr();
}

ISR(TIMER3_COMPA_vect)
{
	Timer::instance(3)->execFunctionPtr();
}

ISR(TIMER4_COMPA_vect)
{
	Timer::instance(4)->execFunctionPtr();
}

ISR(TIMER5_COMPA_vect)
{
	Timer::instance(5)->execFunctionPtr();
}
