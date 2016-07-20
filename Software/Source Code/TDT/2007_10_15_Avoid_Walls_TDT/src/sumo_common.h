#ifndef __SUMOBOT_SUMO_COMMON_H__
#define __SUMOBOT_SUMO_COMMON_H__

// define NULL definition
#ifndef NULL
#define NULL				0
#endif

// type definitions
typedef unsigned char		UCHAR;
typedef unsigned short		USHORT;
typedef unsigned long		UINT;
typedef unsigned long long	UINT64;

typedef signed char			CHAR;
typedef signed short		SHORT;
typedef signed long			INT;
typedef signed long long	INT64;

typedef float				FLOAT;
typedef double				DOUBLE;

typedef UCHAR*				PUCHAR;
typedef USHORT*				PUSHORT;
typedef UINT*				PUINT;
typedef UINT64*				PUINT64;

typedef CHAR*				PCHAR;
typedef SHORT*				PSHORT;
typedef INT*				PINT;
typedef INT64*				PINT64;

typedef FLOAT*				PFLOAT;
typedef DOUBLE*				PDOUBLE;

typedef UCHAR				UBYTE;
typedef PUCHAR				PUBYTE;
typedef CHAR				BYTE;
typedef PCHAR				PBYTE;

#define BOOL				bool
#define PBOOL				bool*

typedef volatile UBYTE* RegPointer;
// type definition for a void function pointer
typedef void (FunctionPtr)(void);

#define BIT_SET(port, bit)	port |= (1 << bit)
#define BIT_CLR(port, bit)	port &= ~(1 << bit)

#define max(a, b)			(a < b ? b : a)
#define min(a, b)			(a < b ? a : b)

#define max3(a, b, c)		max(max(a, b), c)
#define min3(a, b, c)		min(min(a, b), c)

#define FREQ_OSC			16000000UL

#include <string.h>
extern "C" void *memset(void *, int, size_t);

#define _DISABLE_INTERRUPT_START \
	{ \
		UBYTE __SIE_flsg = (SREG & 0x80); \
		cli();

#define _DISABLE_INTERRUPT_END \
		SREG |= __SIE_flsg; \
	}

#endif // __SUMOBOT_SUMO_COMMON_H__
