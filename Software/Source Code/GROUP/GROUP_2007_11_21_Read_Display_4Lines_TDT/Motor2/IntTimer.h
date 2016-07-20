#ifndef INTTIMER_H
#define INTTIMER_H
#include "standard.h"
#include "template.h"		//for bit manipulation
#include <avr/io.h>			//for SFRs
#include <avr/interrupt.h>	//for interrupt stuff

enum //clock constants
{
	num_timers = 6,
	clockFreq = 16000000l
};


class IntTimer
{
public:
	IntTimer():
	timerNumber(0)
	{
	}
	IntTimer(int timer)
	{
		timerNumber = timer;
		functionCall = NULL;
		clearMilliseconds();
		Objects[timer] = this;
	}
	~IntTimer()
	{
	}
	bool setFrequency(long freq)
	{
		long cmp;
		long divFreq;
		//assume 16000000 Hz clock for now
		switch (timerNumber)
		{
			case 0:
			{
				TCCR0B |= 0x03;		// divide by 64
				divFreq = 64;
				break;
			}
			default:
			{
				return false;
			}
		}
		cmp = clockFreq/divFreq/freq;
		if ((cmp==0) || (cmp > BYTE_MAX))
		{
			return false;  //can't generate this frequency with this divider
		}
		switch (timerNumber)
		{
			case 0:
			{
				OCR0A = (Byte)cmp;		// set output compare register A
				break;
			}
			default:
			{
			}
		}
		timerIncrement = (1000l<<8)/freq;
		return true;
	}
	void start()
	{
		switch (timerNumber)
		{
			case 0:
			{
				//set WGM0 2:0 = 0x02; //clear on output compare A
				TCCR0B &= ~(1<<WGM02);
				TCCR0A |= 1<<WGM01;
				TCCR0A &= ~(1<<WGM00);
				//enable interrupt on output compare A
				TIMSK0 |= 1<<OCIE0A;
				break;
			}
			default:
			{
			}
		}
	}
	void stop()
	{
		switch (timerNumber)
		{
			case 0:
			{
				//disable interrupt on output compare A
				TIMSK0 &= ~(1<<OCIE0A);
				break;
			}
			default:
			{
			}
		}
	}
	DWord getMilliseconds()
	{
		DWord returnValue;
		//can count up to 2^24 milliseconds, about 4 hours 40 minutes
		Byte sreg = SREG&0x80;
		cli();
		returnValue = milliseconds>>8;
		SREG |= sreg;
		return returnValue;
	}
	void clearMilliseconds()
	{
		Byte sreg = SREG&0x80;
		cli();
		milliseconds = 0;
		SREG |= sreg;
	}
	void setCountDownTime(DWord countTime)
	{
		Byte sreg = SREG&0x80;
		cli();
		countDownTime = countTime<<8;
		SREG |= sreg;
	}
	DWord getCountDownTime()
	{
		DWord returnValue;
		//can count up to 2^24 milliseconds, about 4 hours 40 minutes
		Byte sreg = SREG&0x80;
		cli();
		returnValue = countDownTime>>8;
		SREG |= sreg;
		return returnValue;
	}


	void waitMilliseconds(DWord waitTime)
	{
		DWord initialTime = getMilliseconds();
		do {}
		while ((getMilliseconds() - initialTime) < waitTime);
	}
	void setFunctionCall(void (*f)())
	{
		functionCall = f;
	}
	static void intRoutine(int timer)
	{
		IntTimer *me;
		me = Objects[timer];
		me->milliseconds += me->timerIncrement;
		if (me->countDownTime > me->timerIncrement)
		{
			me->countDownTime -= me->timerIncrement;
		} else
		{
			me->countDownTime = 0;
		}	
		if (me->functionCall != NULL)
		{
			me->functionCall();
		}
	}
private:
	int timerNumber;  //store timer number
	static IntTimer *Objects[num_timers];	//timer objects
	DWord timerIncrement;	//used to increment millisecond counter
	volatile DWord milliseconds;  //millisecond count
	volatile DWord countDownTime;  //count down timer milliseconds
	void (*functionCall)();  //interrupt function
};


#endif //INTTIMER_H
