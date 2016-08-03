#ifndef DOTMATRIX_H
#define DOTMATRIX_H
//includes
#include "standard.h"		//for standard types
#include "sprites.h"		//for sc_hexDigits
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
//B120 B104 B088 B072 B056 B040 B024 B008 : B112 B096 B080 B064 B048 B032 B016 B000
//B121 B105 B089 B073 B057 B041 B025 B009 : B113 B097 B081 B065 B049 B033 B017 B001
// ... ...  ...  ...  ...  ...  ...  ...    ...  ...  ...  ...  ...  ...  ...  ...
//B127 B111 B095 B079 B063 B047 B031 B015 : B119 B103 B087 B071 B055 B039 B023 B007

//Mapped to:

//B000 B001 B002 B003 B004 B005 B006 B007 : B008 B009 B010 B011 B012 B013 B014 B015
//B016 B017 B018 B019 B020 B021 B022 B023 : B024 B025 B026 B027 B028 B029 B030 B031
// ... ...  ...  ...  ...  ...  ...  ...    ...  ...  ...  ...  ...  ...  ...  ...
//B112 B113 B114 B115 B116 B117 B118 B119 : B120 B121 B122 B123 B124 B125 B126 B127


class DotMatrix
{
public:
	DotMatrix()
	{
		LED_PORT2 |= _BV(LED_BLANK);  				//Blank LEDs
		clear();
	}
	~DotMatrix()
	{
		LED_PORT2 |= _BV(LED_BLANK);  //Blank LEDs
	}
	void clear()
	{
		memset(&LEDArray[0], 0, sizeof(LEDArray));	//Clear LED Array
	}
	void update()
	{
		static Byte comcount = 0;
		//set line
		LED_Set_Common(comcount);
		//send line
		LED_Send_Line(LEDArray[comcount&0x07]);
		//increment common driver
		comcount++;
	}
	void showHex(Byte num)
	{
		writeSprite(
			sc_hexDigits[(num >> 4) & 0x0F],
			0,
			0,
			8,
			SPRITE_ALPHANUMERIC_HEIGHT
		);
		writeSprite(
			sc_hexDigits[num & 0x0F],
			8,
			0,
			8,
			SPRITE_ALPHANUMERIC_HEIGHT
		);
	}
	void showInt(int number)  //display an int with up to 4 decimal places
	{
		Byte index;;
		Byte offset = 12;
		for (Byte i = 0; i<4; i++)
		{
			index = number % 10;
			writeSprite(		
				sc_hexDigits_mini[index],
				offset,
				1,
				SPRITE_ALPHANUMERIC_MINI_WIDTH,
				SPRITE_ALPHANUMERIC_MINI_HEIGHT
				);
			number /= 10;
			offset -= 4;
		}
	}

	void showFloat(float number, Byte number_places = 4)  //display a float with up to 4 decimal places
	{
		Byte index;
		if (number_places > 4)
		{
			number_places = 4;
		}
		for (Byte i=0; i<number_places; i++)
		{
			number = number*10;
		}

		Byte offset = 12;
		for (Byte i = 0; i<4; i++)
		{
			index = Word(number)%10;
			writeSprite(		
				sc_hexDigits_mini[index],
				offset,
				1,
				SPRITE_ALPHANUMERIC_MINI_WIDTH,
				SPRITE_ALPHANUMERIC_MINI_HEIGHT
				);
			number /= 10;
			offset -= 4;
		}
		//decimal place
		setBit(7,5);


}
	void writeSprite(const Byte sprite[8], Byte xPos=0, Byte yPos=0, Byte xSize=8, Byte ySize=8)
	{
		for (Byte x=0; x<xSize; x++)
			for (Byte y=0; y<ySize; y++)
			{
				writeBit(x+xPos, y+yPos, sprite[y]&(1<<(7-x)));
			}
	}
	void setSprite(const Byte sprite[8], Byte xPos=0, Byte yPos=0, Byte xSize=8, Byte ySize=8)
	{
		for (Byte x=0; x<xSize; x++)
			for (Byte y=0; y<ySize; y++)
			{
				if (sprite[y]&(1<<(7-x)))
				{
					setBit(x+xPos, y+yPos);
				}
			}
	}
	void xorSprite(Byte sprite[8], Byte xPos=0, Byte yPos=0, Byte xSize=8, Byte ySize=8)
	{
		for (Byte x=0; x<xSize; x++)
			for (Byte y=0; y<ySize; y++)
			{
				xorBit(x+xPos, y+yPos);
			}
	}
	void clearSprite(Byte sprite[8], Byte xPos=0, Byte yPos=0, Byte xSize=8, Byte ySize=8)
	{
		for (Byte x=0; x<xSize; x++)
			for (Byte y=0; y<ySize; y++)
			{
				if (sprite[y]&(1<<(7-x)))
				{
					clearBit(x+xPos, y+yPos);
				}
			}
	}
	void writeBit(Byte x, Byte y, bool bit)
	{
		if (bit)
		{
			setBit(x, y);
		} else
		{
			clearBit(x, y);
		}
	}
	void setBit(Byte x, Byte y)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(x&0x07)] |= 1<<(((y&0x07)+(0x08^(x&0x08)))&0x0F);  //gobbledegook to do above transform
		SREG |= sreg;
	}
	void xorBit(Byte x, Byte y)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(x&0x07)] ^= 1<<(((y&0x07)+(0x08^(x&0x08)))&0x0F);  //gobbledegook to do above transform
		SREG |= sreg;
	}
	void clearBit(Byte x, Byte y)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(x&0x07)] &= ~(1<<(((y&0x07)+(0x08^(x&0x08)))&0x0F));  //gobbledegook to do above transform
		SREG |= sreg;
	}
	void setBit(Byte b)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(b&0x07)] |= 1<<((((b&0x7F)>>4)+(0x08^(b&0x08)))&0x0F);  //gobbledegook to do above transform
		SREG |= sreg;
	}
	void xorBit(Byte b)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(b&0x07)] ^= 1<<((((b&0x7F)>>4)+(0x08^(b&0x08)))&0x0F);  //gobbledegook to do above transform
		SREG |= sreg;
	}
	void clearBit(Byte b)
	{
		Byte sreg = SREG&0x80;
		cli();
		LEDArray[0x07-(b&0x07)] &= ~(1<<((((b&0x7F)>>4)+(0x08^(b&0x08)))&0x0F));
		SREG |= sreg;
	}
private:
Word LEDArray[8];				//storage for LED data (16x8)
void LED_Send_Line(Word line)   //clock in 16 bits into segment driver and latch
{
	
	for(Byte i=0;i<16;i++) //LSB first
	{
		if((line & 0x01) > 0) //check MSB
		{
			LED_PORT2 |= _BV(LED_SIN);			
		}
		else
		{
			LED_PORT2 &= ~(_BV(LED_SIN));
		}
		line >>= 1;  //shift data right one	
		//clock in bit
		LED_PORT2 |= _BV(LED_SCLK);  //set clock high
		LED_PORT2 &= ~(_BV(LED_SCLK)); //set clock low		
		
	}
	
	//latch in line
	LED_PORT2 |= _BV(LED_LATCH);  //set clock high
	LED_PORT2 &= ~(_BV(LED_LATCH)); //set clock low	

	LED_PORT2 &= ~(_BV(LED_BLANK)); //Un-blank LEDS
}


void LED_Set_Common(Byte com)	//set the common driver  Range 0-7
{				
	LED_PORT2 |= _BV(LED_BLANK);  //Blank LEDs until line sent and latched
	com = (com&0x07)<<4;   //shift so it matches bits on port
	LED_PORT1 &= ~(0x07<<4);  //clear CSEL bits
	LED_PORT1 |= com;  //set new CSEL values
}
};
#endif // DOTMATRIX_H
