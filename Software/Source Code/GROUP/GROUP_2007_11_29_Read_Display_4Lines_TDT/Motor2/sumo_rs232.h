#ifndef __SUMOBOT_SUMO_RS232_H__
#define __SUMOBOT_SUMO_RS232_H__

#include "sumo_common.h"

// define number of rs-232 interfaces
#define RS232_INSTANCES			2

// define default buffer size for receive/transmit data
#define RS232_BUFFER_SIZE		256

namespace Sumo
{
	class RS232
	{
	// enumerations for configuring serial communications
	public:
		enum BaudRate
		{
			BAUD_300 = 300,
			BAUD_600 = 600,
			BAUD_1200 = 1200,
			BAUD_1800 = 1800,
			BAUD_2400 = 2400,
			BAUD_3600 = 3600,
			BAUD_4800 = 4800,
			BAUD_7200 = 7200,
			BAUD_9600 = 9600,
			BAUD_14400 = 14400,
			BAUD_28800 = 28800,
			BAUD_38400 = 38400,
			BAUD_57600 = 57600
		};
		enum DataBits
		{
			DATA_5_BITS = 0,
			DATA_6_BITS = 1,
			DATA_7_BITS = 2,
			DATA_8_BITS = 3,
			DATA_9_BITS = 7
		};
		enum Parity
		{
			PARITY_NONE = 0,
			PARITY_EVEN = 2,
			PARITY_ODD = 3
		};
		enum StopBits
		{
			STOP_1_BIT = 0,
			STOP_2_BIT = 1
		};

	// instance management and private constructors
	private:
		RS232(UBYTE port);
		UBYTE m_port;
		BaudRate m_baud;
		DataBits m_databits;
		Parity m_parity;
		StopBits m_stopbits;
	private:
		static RS232* sm_rs232ports[RS232_INSTANCES];
	public:
		static RS232* instance(UBYTE port);

	// internal configuration functions
	private:
		static void internalConfigure(UBYTE port, BaudRate baud, DataBits databits,
			Parity parity, StopBits stopbits);

	// internal port enable/disable
	private:
		static void internalEnable(UBYTE port, BOOL transmit, BOOL receive);

	// internal transmission functions
	private:
		static void internalTransmit(UBYTE port, UBYTE ch);

	// initialisation/configuration functions
	public:
		void init();
		void configurePort(BaudRate baud, DataBits databits, Parity parity, StopBits stopbits);

	// port enable/disable
	public:
		void enable(BOOL transmit, BOOL receive);
	private:
		BOOL m_txEnabled;
		BOOL m_rxEnabled;

	// buffering
	private:
		UBYTE m_txBuffer[RS232_BUFFER_SIZE];
		UBYTE m_rxBuffer[RS232_BUFFER_SIZE];
		UINT m_txBufferSize;
		UINT m_rxBufferSize;

	// receive, reading, writing and flushing
	public:
		void receive(UBYTE ch);
		UINT read(UBYTE* buf, UINT len = 0);
		UINT write(UBYTE* buf, UINT len);
		void flush();
	};
};

#endif // __SUMOBOT_SUMO_RS232_H__
