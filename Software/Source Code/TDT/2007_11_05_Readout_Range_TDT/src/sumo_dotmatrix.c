#include <avr/io.h>
#include <avr/interrupt.h>
#include "sumo_dotmatrix.h"

using namespace Sumo;

DotMatrix* DotMatrix::sm_dotmatrix = NULL;

const UBYTE DotMatrix::CommonDriverMap[DOTMATRIX_COMMON_DRIVERS][2] = {
	{15, 7}, {14, 6}, {13, 5}, {12, 4},
	{11, 3}, {10, 2}, {9, 1}, {8, 0}
};

DotMatrix::DotMatrix()
{
	this->init();

	m_currentDriver = 0;
}

DotMatrix* DotMatrix::instance()
{
	if (sm_dotmatrix == NULL)
	{
		sm_dotmatrix = new DotMatrix();
	}

	return sm_dotmatrix;
}

void DotMatrix::init()
{
	// set all signal lines from micro to led driver as outputs
	// initially set these outputs to 0
	LED_DDR1 |= (_BV(LED_SIN) | _BV(LED_LATCH) | _BV(LED_SCLK) | _BV(LED_BLANK));
	LED_PORT1 &= ~(_BV(LED_SIN) | _BV(LED_LATCH) | _BV(LED_SCLK) | _BV(LED_BLANK));
	LED_DDR2 |= (_BV(LED_CSEL0) | _BV(LED_CSEL1) | _BV(LED_CSEL2) | _BV(LED_SOUT));
	LED_PORT2 &= ~(_BV(LED_CSEL0) | _BV(LED_CSEL1) | _BV(LED_CSEL2) | _BV(LED_SOUT));

	// reset the screen
	reset();
}

void DotMatrix::reset()
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// reset led matrix buffers
	memset(&m_image[0], 0x00, sizeof(m_image));

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::refresh()
{
	// reset current driver
	m_currentDriver = 0;

	// perform incremental refresh for each driver. in the end of
	// it all, m_currentDriver will be set to 0
	for (UINT i = 0; i < DOTMATRIX_COMMON_DRIVERS; i++)
	{
		incrementalRefresh();
	}
}

void DotMatrix::incrementalRefresh()
{
	// blank led until common driver value is set
	BIT_SET(LED_PORT1, LED_BLANK);

	// load common driver address to driver
	LED_PORT2 &= ~(_BV(LED_CSEL0) | _BV(LED_CSEL1) | _BV(LED_CSEL2));
	LED_PORT2 |= ((m_currentDriver & 0x07) << LED_CSEL0);

	// define an array to load into the led driver
	BOOL bitsToSend[DOTMATRIX_ROWS * 2];

	// for each row...
	for (UINT j = 0; j < DOTMATRIX_ROWS; j++)
	{
		// pick the bit value for the first column definition
		// note that column 0 is the MSB of the row
		bitsToSend[j] = m_image[j] &
			(1 << (DOTMATRIX_COLUMNS - 1) - CommonDriverMap[m_currentDriver][0]);
	}

	// for each row again...
	for (UINT j = 0; j < DOTMATRIX_ROWS; j++)
	{
		// pick the bit value for the second column definition
		// note that column 0 is the MSB of the row
		bitsToSend[j + DOTMATRIX_ROWS] = m_image[j] &
			(1 << (DOTMATRIX_COLUMNS - 1) - CommonDriverMap[m_currentDriver][1]);
	}

	// for each bit to send...
	for (UINT j = 0; j < DOTMATRIX_ROWS * 2; j++)
	{
		// set or clear the LED_SIN signal
		if (bitsToSend[j])
		{
			BIT_SET(LED_PORT1, LED_SIN);
		}
		else
		{
			BIT_CLR(LED_PORT1, LED_SIN);
		}

		// clock the value in
		BIT_SET(LED_PORT1, LED_SCLK);
		BIT_CLR(LED_PORT1, LED_SCLK);
	}

	// latch the common driver
	BIT_SET(LED_PORT1, LED_LATCH);
	BIT_CLR(LED_PORT1, LED_LATCH);
	
	// un-blank leds
	BIT_CLR(LED_PORT1, LED_BLANK);

	// increment common driver address, reset if pass maximum
	m_currentDriver++;
	if (m_currentDriver == DOTMATRIX_COMMON_DRIVERS)
	{
		m_currentDriver = 0;
	}
}

void DotMatrix::loadImage(const Image imageData)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// for each row...
	for (UINT i = 0; i < DOTMATRIX_ROWS; i++)
	{
		// save image data
		m_image[i] = imageData[i];
	}

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::loadSprite(const USHORT* spriteData, UBYTE width,
	UBYTE height, UBYTE row, UBYTE column)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// for each row in the sprite...
	UINT sr = 0;
	for (UINT r = row; r < min(row + height, DOTMATRIX_ROWS); r++)
	{
		// for each column in the sprite...
		UINT sc = 0;
		for (UINT c = column; c < min(column + width, DOTMATRIX_COLUMNS); c++)
		{
			// if sprite data is set, then set it on the image
			// or else clear it
			if (spriteData[sr] & (1 << ((width - 1) - sc)))
			{
				m_image[r] |= (1 << ((DOTMATRIX_COLUMNS - 1) - c));
			}
			else
			{
				m_image[r] &= ~(1 << ((DOTMATRIX_COLUMNS - 1) - c));
			}

			sc++;
		}

		sr++;
	}

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::internalShiftLeft(BOOL withWrap)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// for each row
	for (UINT i = 0; i < DOTMATRIX_ROWS; i++)
	{
		// save the most significant bit
		BOOL msb = m_image[i] & (1 << (DOTMATRIX_COLUMNS - 1));

		// shift data to the left
		m_image[i] <<= 1;

		// if wrapping is set, set the lsb to whatever
		// the previous msb was
		m_image[i] |= (withWrap ? (msb ? 0x01 : 0x00) : 0x00);
	}

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::internalShiftRight(BOOL withWrap)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// for each row
	for (UINT i = 0; i < DOTMATRIX_ROWS; i++)
	{
		// save the least significant bit
		BOOL lsb = m_image[i] & 0x01;

		// shift data to the right
		m_image[i] >>= 1;

		// if wrapping is set, set the msb to whatever
		// the previous lsb was
		m_image[i] |= (withWrap ?
			(lsb ? 0x01 : 0x00) << (DOTMATRIX_COLUMNS - 1) : 0x00
		);
	}

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::internalShiftUp(BOOL withWrap)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// save data in first row
	USHORT firstRow = m_image[0];

	// for each row from second row onward
	for (UINT i = 1; i < DOTMATRIX_ROWS; i++)
	{
		// copy data to previous row
		m_image[i - 1] = m_image[i];
	}

	// if wrapping is set, save previous first row data to the
	// last row
	m_image[DOTMATRIX_ROWS - 1] = (withWrap ? firstRow : 0x0000);

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::internalShiftDown(BOOL withWrap)
{
	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// save data in last row
	USHORT lastRow = m_image[DOTMATRIX_ROWS - 1];

	// for each row from last to second...
	for (UINT i = (DOTMATRIX_ROWS - 1); i > 0; i--)
	{
		// copy data from previous row
		m_image[i] = m_image[i - 1];
	}

	// if wrapping is set, save previous last row data to the
	// first row
	m_image[0] = (withWrap ? lastRow : 0x0000);

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END
}

void DotMatrix::shiftLeft()
{
	internalShiftLeft(false);
}

void DotMatrix::shiftRight()
{
	internalShiftRight(false);
}

void DotMatrix::shiftUp()
{
	internalShiftUp(false);
}

void DotMatrix::shiftDown()
{
	internalShiftDown(false);
}

void DotMatrix::wrapLeft()
{
	internalShiftLeft(true);
}

void DotMatrix::wrapRight()
{
	internalShiftRight(true);
}

void DotMatrix::wrapUp()
{
	internalShiftUp(true);
}

void DotMatrix::wrapDown()
{
	internalShiftDown(true);
}

