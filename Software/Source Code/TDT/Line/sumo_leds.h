
#include <inttypes.h>

void LED_Send_Line(uint16_t line);  //clock in 16 bits into segment driver and latch
void LED_Set_Common(uint8_t com);	//set the common driver
