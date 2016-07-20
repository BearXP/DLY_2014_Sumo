#if !defined _STANDARD_H
#define _STANDARD_H

#define __STDC_LIMIT_MACROS
	#include <stdint.h>	
	#include <assert.h>
	#include <string.h>

// machine dependent defines
	// abbreviated types
		typedef unsigned char uchar;
		typedef unsigned int uint;
		typedef unsigned long ulong;
		typedef double Real;
	// fixed size types
		typedef unsigned char Byte;
		typedef signed char SByte;
		typedef unsigned short Word;
		typedef unsigned short USHORT;
		typedef signed short SWord;
		typedef unsigned long int DWord;
		typedef signed long SDWord;
	// sizes
		const Byte BYTE_MAX = UINT8_MAX;
		const SByte SBYTE_MIN = INT8_MIN;
		const SByte SBYTE_MAX = INT8_MAX;
		const Word WORD_MAX = UINT16_MAX;
		const SWord SWORD_MIN = INT16_MIN;
		const SWord SWORD_MAX = INT16_MAX;
		const DWord DWORD_MAX = UINT32_MAX;
		const SDWord SDWORD_MIN = INT32_MIN;
		const SDWord SDWORD_MAX = INT32_MAX;

// types
		typedef volatile Byte* RegPointer;
// structures
	struct pos
	{
		SWord xpos;
		SWord ypos;
	};
	struct vel
	{
		SWord xvel;
		SWord yvel;
	};

// standard defines
	// macros
		#define CONSTANT(name, value) enum {name = value}
		#define ARRAYSIZE(name) (sizeof(name)/sizeof((name)[0]))
		#define ASSERT(condition) AssertEx(condition, "ASSERT")
		#define __stdcall
	// inlines
		// rounding
			template<class T> inline const T round(float x)
				{static_cast<T>(x + (x > 0 ? 0.5f : -0.5f));}
			template<class T> inline const T round(double x)
				{static_cast<T>(x + (x > 0 ? 0.5 : -0.5));}
		// bit and byte manipulation
			template<class T, class T1> inline const bool bitTest(T x, T1 pos)
				{return (x & 1<<pos) != 0;}
		// math
			template<class T> inline const T min(T x, T y)
				{return x<=y?x:y;}
			template<class T> inline const T max(T x, T y)
				{return x>=y?x:y;}
			inline Byte loByte(Word x) {return static_cast<Byte>(x);}
			inline Byte hiByte(Word x) {return static_cast<Byte>(x>>8);}
			inline Word loWord(DWord x) {return static_cast<Word>(x);}
			inline Word hiWord(DWord x) {return static_cast<Word>(x>>16);}

#endif // _STANDARD_H
