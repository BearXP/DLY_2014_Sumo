#ifndef __SUMOBOT_SUMO_DOTMATRIX_H__
#define __SUMOBOT_SUMO_DOTMATRIX_H__

#include <avr/io.h>
#include "sumo_common.h"

/*
	// our-coordinate system...

	       +-- origin (0,0)
	       V
	---+  +--------+ +--------+
	   |  |oooooooo| |oooooooo| <-- (15,0)
	   |  |oooooooo| |oooooooo|
	U3 |  |oooooooo| |oooooooo|
	   |  |oooooooo| |oooooooo|
	LED|  |oooooooo| |oooooooo|
	DRV|  |oooooooo| |oooooooo|
	   |  |oooooooo| |oooooooo|
	   |  |oooooooo| |oooooooo| <-- (15,7)
	---+  +--------+ +--------+
	       ^
	       +-- (0,7)
*/

// define dot matrix dimensions
#define DOTMATRIX_COLUMNS			16
#define DOTMATRIX_ROWS				8
#define DOTMATRIX_COMMON_DRIVERS	8

// define signals to led driver
#define LED_SCLK					PL4
#define LED_SIN						PL5
#define LED_LATCH					PL6
#define LED_BLANK					PL7
#define LED_CSEL0					PD4
#define LED_CSEL1					PD5
#define LED_CSEL2					PD6
#define LED_SOUT					PD7

// define ports to be used by led driver
#define LED_PORT1					PORTL
#define LED_PORT2					PORTD

// define DDR's to be used by led driver
#define LED_DDR1					DDRL
#define LED_DDR2					DDRD

namespace Sumo
{
	class DotMatrix
	{
	// define a mapping between common drivers and
	// columns
	private:
		static const UBYTE CommonDriverMap[DOTMATRIX_COMMON_DRIVERS][2];

	// type definition for an image
	public:
		typedef USHORT Image[DOTMATRIX_ROWS];

	// singleton management and private constructors
	private:
		DotMatrix();
	private:
		static DotMatrix* sm_dotmatrix;
	public:
		static DotMatrix* instance();

	// led matrix buffers
	private:
		Image m_image;

	// device initialisation
	public:
		void init();

	// screen reset
	public:
		void reset();

	// screen refresh
	public:
		void refresh();
		void incrementalRefresh();

	// driver refresh
	private:
		void refreshCurrentDriver();
		UBYTE m_currentDriver;

	// image loading
	public:
		void loadImage(const Image imageData);

	// sprite loading
	public:
		void loadSprite(const USHORT* spriteData, UBYTE width, UBYTE height,
			UBYTE row, UBYTE column);

	// internal shift/wrap operations
	public:
		void internalShiftLeft(BOOL withWrap);
		void internalShiftRight(BOOL withWrap);
		void internalShiftUp(BOOL withWrap);
		void internalShiftDown(BOOL withWrap);
	
	// shifting operations
	public:
		void shiftLeft();
		void shiftRight();
		void shiftUp();
		void shiftDown();

	// wrapping operations
	public:
		void wrapLeft();
		void wrapRight();
		void wrapUp();
		void wrapDown();
	};
};

#endif // __SUMOBOT_SUMO_DOTMATRIX_H__
