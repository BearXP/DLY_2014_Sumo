
#include <avr/io.h>
#include "sumo_util.h"
#include "sumo_ports.h"

uint8_t Get_Switch(uint8_t sw)
{
	uint8_t swin = SW_PORT;
	return (swin & (sw<<6));
}
