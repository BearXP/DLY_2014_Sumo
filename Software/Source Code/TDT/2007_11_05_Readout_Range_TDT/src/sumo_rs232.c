#include <avr/io.h>
#include <avr/interrupt.h>
#include "sumo_rs232.h"

using namespace Sumo;

RS232* RS232::sm_rs232ports[RS232_INSTANCES] = {NULL};

RS232::RS232(UBYTE port)
{
	m_port = port;
	memset(&(m_txBuffer[0]), 0x00, sizeof(m_txBuffer));
	memset(&(m_rxBuffer[0]), 0x00, sizeof(m_rxBuffer));
	m_txBufferSize = 0;
	m_rxBufferSize = 0;
	m_txEnabled = false;
	m_rxEnabled = false;

	// perform device initialisation
	init();
}

RS232* RS232::instance(UBYTE port)
{
	// check if instance has been created. if
	// not, create the instance
	if (sm_rs232ports[port] == NULL)
	{
		sm_rs232ports[port] = new RS232(port);
	}
	
	// return appropriate instance
	return sm_rs232ports[port];
}

void RS232::internalConfigure(UBYTE port, BaudRate baud,
	DataBits databits, Parity parity, StopBits stopbits)
{
	// determine value to feed into baud rate register
	UINT regBaudRate = ((FREQ_OSC / 16) / (UINT)baud) - 1;
	
	switch (port)
	{
		case 0:
		{
			// set up baud rate register
			UBRR0H = (regBaudRate >> 8) & 0xFF;
			UBRR0L = regBaudRate & 0xFF;

			// set up port settings
			UCSR0C = (0<<UMSEL01) | (0<<UMSEL00) | // asynchronous mode
				((UBYTE)parity << UPM00) |         // parity setting
				((UBYTE)stopbits << USBS0) |       // stop bits setting
				((UBYTE)databits << UCSZ00);       // data bits setting
			break;
		}
		case 1:
		{
			// same as above
			UBRR1H = (regBaudRate >> 8) & 0xFF;
			UBRR1L = regBaudRate & 0xFF;
			UCSR1C = (0<<UMSEL11) | (0<<UMSEL10) | // asynchronous mode
				((UBYTE)parity << UPM10) |         // parity setting
				((UBYTE)stopbits << USBS1) |       // stop bits setting
				((UBYTE)databits << UCSZ10);       // data bits setting
			break;
		}
	}
}

void RS232::internalEnable(UBYTE port, BOOL transmit, BOOL receive)
{
	switch (port)
	{
		case 0:
		{
			// enable/disable transmit mode
			if (transmit)
				UCSR0B |= _BV(TXEN0);
			else
				UCSR0B &= ~(_BV(TXEN0));
			
			// enable/disable receive mode
			if (receive)
				UCSR0B |= (_BV(RXEN0) | _BV(RXCIE0));
			else
				UCSR0B &= ~(_BV(RXEN0) | _BV(RXCIE0));
			
			break;	
		}
		case 1:
		{
			// same as above
			if (transmit)
				UCSR1B |= _BV(TXEN1);
			else
				UCSR1B &= ~(_BV(TXEN1));
			
			if (receive)
				UCSR1B |= (_BV(RXEN1) | _BV(RXCIE1));
			else
				UCSR1B &= ~(_BV(RXEN1) | _BV(RXCIE1));
			
			break;
		}
	}
}

void RS232::internalTransmit(UBYTE port, UBYTE ch)
{
	switch (port)
	{
		case 0:
		{
			// wait until transmit buffer is free
			while (UCSR0A & _BV(UDRE0) == 0)
			{
				asm volatile ("nop\n\t" ::);
			}

			// put data into the buffer
			UDR0 = ch;

			break;
		}
		case 1:
		{
			// similar to above
			while (UCSR1A & _BV(UDRE1) == 0)
			{
				asm volatile ("nop\n\t" ::);
			}
			UDR1 = ch;
			break;
		}
	}
}

void RS232::init()
{
	// configure for 9600 baud, 8 bits, no parity, 1 stop bit
	configurePort(BAUD_9600, DATA_8_BITS, PARITY_NONE, STOP_1_BIT);
}

void RS232::configurePort(BaudRate baud, DataBits databits,
	Parity parity, StopBits stopbits)
{
	internalConfigure(m_port, baud, databits, parity, stopbits);

	m_baud = baud;
	m_databits = databits;
	m_parity = parity;
	m_stopbits = stopbits;
}

void RS232::enable(BOOL transmit, BOOL receive)
{
	internalEnable(m_port, transmit, receive);

	m_txEnabled = transmit;
	m_rxEnabled = receive;
}

void RS232::receive(UBYTE ch)
{
	// ignore if the receive buffer is already full
	if (m_rxBufferSize == sizeof(m_rxBuffer))
	{
		return;
	}

	// add to buffer and increment buffer size
	m_rxBuffer[m_rxBufferSize] = ch;
	m_rxBufferSize++;
}

UINT RS232::read(UBYTE* buf, UINT len)
{
	// define number of bytes to read. assume that if the supplied length
	// is zero then the caller requests the entire buffer
	UINT bytesToRead = min((len == 0 ? sizeof(m_rxBuffer) : len), m_rxBufferSize);

	// break out of function if there is no data to read
	if (bytesToRead == 0)
	{
		return 0;
	}

	// start block for disabling an interrupt
	_DISABLE_INTERRUPT_START
	
	// for each byte in the buffer requested...
	for (UINT i = 0; i < bytesToRead; i++)
	{
		// copy to user buffer
		buf[i] = m_rxBuffer[i];
	}

	// if the whole buffer was obtained...
	if (bytesToRead == m_rxBufferSize)
	{
		// reset the buffer
		memset(&(m_rxBuffer[0]), 0x00, sizeof(m_rxBuffer));
		m_rxBufferSize = 0;
	}
	// or else...
	else
	{
		// move the buffer downward
		for (UINT i = 0; i < bytesToRead; i++)
		{
			m_rxBuffer[i] = m_rxBuffer[i + bytesToRead];
		}

		// decrement buffer size
		m_rxBufferSize -= bytesToRead;
	}

	// end block for disabling an interrupt
	_DISABLE_INTERRUPT_END

	// return bytes read
	return bytesToRead;
}

UINT RS232::write(UBYTE* buf, UINT len)
{
	// define number of bytes to write into the buffer. this is to
	// prevent buffer overflows
	UINT bytesToWrite = min(len, sizeof(m_txBuffer) - m_txBufferSize);

	// for each byte to be written to the buffer...
	for (UINT i = 0; i < bytesToWrite; i++)
	{
		// copy from user buffer
		m_txBuffer[m_txBufferSize + i] = buf[i];
	}

	// increment transmit buffer size
	m_txBufferSize += bytesToWrite;

	// return bytes written
	return bytesToWrite;
}

void RS232::flush()
{
	// for each byte in the transmit buffer...
	for (UINT i = 0; i < m_txBufferSize; i++)
	{
		// transmit byte to serial port
		internalTransmit(m_port, m_txBuffer[i]);
	}

	// reset the buffer
	memset(&(m_txBuffer[0]), 0x00, sizeof(m_txBuffer));
	m_txBufferSize = 0;
}

ISR(USART0_RX_vect)
{
	UBYTE byte = UDR0;
	RS232::instance(0)->receive(byte);
}

ISR(USART1_RX_vect)
{
	UBYTE byte = UDR1;
	RS232::instance(1)->receive(byte);
}
