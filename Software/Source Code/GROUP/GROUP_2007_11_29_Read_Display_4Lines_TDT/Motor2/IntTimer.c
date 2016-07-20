#include "inttimer.h"
IntTimer *IntTimer::Objects[num_timers];

// vectors should only be set up if timer is created
// ...........this should be set up inside the class........
ISR(TIMER0_COMPA_vect)
{
	IntTimer::intRoutine(0);		//timer0
}
