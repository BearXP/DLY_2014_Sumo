#define 	cbi(sfr, bit)   (_SFR_BYTE(sfr) &= ~_BV(bit))
#define 	sbi(sfr, bit)   (_SFR_BYTE(sfr) |= _BV(bit))



uint8_t Get_Switch(uint8_t sw); //returns >0 if the switch "sw" is high, 0 otherwise.
								//sw = 1 is PH6, sw = 2 is PH7