//-----------------------------------------------------------------------------
//	General templated functions
//-----------------------------------------------------------------------------
#ifndef TEMPLATE_H
#define TEMPLATE_H
template<class T0, class T1, class T2> inline const bool InRange(T0 x, T1 min, T2 max)
	{	return static_cast<bool>
             (((static_cast<T1> (x)) >= min) &&
               ((static_cast<T2> (x)) <= max));}
template<class T0, class T1> inline T0 BitClear(const T0 x, const T1 mask)
	{
		return	x&~(1<<mask);
	}
template<class T0, class T1> inline T0 BitSet(const T0 x, const T1 mask)
	{
		return	x|(1<<mask);
	}
template<class T0> inline const bool Odd(const T0 x)
	{
		return	(x & 1)==1;
	}
template<class T0> inline const bool Even(const T0 x)
	{
		return	(x & 1)!=1;
	}
inline char EightToSevenP(char ch)
	{
		const char CHAR_BITS=8;
		int parity = Odd(ch);
		char tempChar = ch;
		for (int i=1;i<CHAR_BITS;i++)
		{
			tempChar=tempChar>>1;
			parity+=Odd(tempChar)?1:0;
		}
		return ch+(Odd(parity)?0x80:0x00);
	}

#endif //TEMPLATE_H
