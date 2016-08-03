/*****************************************************************************
*
* Atmel Corporation
*
* File              : adpcm.h
* Compiler          : IAR EWAAVR 3.10C
* Revision          : $Revision: 1.4 $
* Date              : $Date: Monday, November 15, 2004 12:53:58 UTC $
* Updated by        : $Author: KRB $
*
* Support mail      : avr@atmel.com
*
* Supported devices : This example is written for the ATmega128. The firmware
*                     should compile on any AVR with at least 4kB of Flash
*                     memory (plus enough room for sound data).
*
* AppNote           : AVR336: ADPCM Decoder.
*
* Description       : ADPCM Decoder header file, state structure definition 
*                     and function prototypes, bit level definitions etc. 
*
****************************************************************************/

#ifndef ADPCM_H
#define ADPCM_H

#include "standard.h"	//for RegPointer
#include <avr/pgmspace.h>	//for PGM_P
#include <avr/io.h>			//for SREG
#include <avr/interrupt.h>	//for cli()
#include <stdlib.h>			//for abs()

/*
 * Definitions (bit level, PWM mode etc.) 
 */


#define END_OF_DATA 100     // returned by get_code() when no data left
#define BITS 5              // bit level (2, 3, 4 or 5)
#define BUFFER_SIZE 128     // output buffer size (max. 255 due to index
                            // variables being of type 'char')

/*
 * ADPCM state structure
 */
 
typedef struct adpcm_state_ {
	long yl;	// Locked (slow) state scale factor step size mult.
	short yu;	// Unlocked (fast) state scale factor step size mult.
	short dms;	// Short term energy estimate.
	short dml;	// Long term energy estimate.
	short ap;	// Linear weighting coefficient of 'yl' and 'yu'.

	short a[2];	// Coefficients of pole portion of prediction filter.
	short b[6];	// Coefficients of zero portion of prediction filter.
	short pk[2];	// Signs of previous partially reconstructed signals.

	short dq[6];	// Previous samples of the quantized difference signal.
	short sr[2];	// Previous samples of the reconstructed signal .
	char td;	// Tone detection indicator.
} adpcm_state;

/*
 * Function prototypes.
 */
class ADPCM
{
public:
	ADPCM():
	isRunning(false),
	outOfData(true),
	regPointerSet(false)
	{
		initialize_adpcm();
	}
	~ADPCM()
	{
	}
	void setPWMRegister(RegPointer pwm)
	{
		PWMReg = pwm;
		regPointerSet = true;
	} 
	RegPointer getPWMRegister()
	{
		return PWMReg;
	} 
	void setDataPointer(PGM_P p, Word size)
	{
		packed = p;
		dataSize = size;
	} 
	PGM_P getDataPointer()
	{
		return packed;
	}
	void pause()
	{
		isRunning = false;
	}
	void go()
	{
		isRunning = true;
	}
	void reset()
	{
		initialize_adpcm();
	}
	void update()
	{
  		if((da_ind != decoder_ind) && isRunning)
  		{
  			*PWMReg = buffer[da_ind++] + 256;
  			if(da_ind >= BUFFER_SIZE)
    		{
				da_ind = 0;
			}
  		}	
	}
	bool pumpBuffer()
	{
	  Byte sreg = SREG&0x80;
	  unsigned char code;
	  if (decoder_ind != da_ind)		//room for data
	  {
	  	code = get_code();       
      	if (code != END_OF_DATA)
	  	{         // end of sound
        	sample = decode(code);   	// decode one sample
        	if(abs(sample >> 5) > 255)      // make sure that
			{
          	sample = 0;
			}
        	// 9-bit pwm, decode returns 14-bit value -> shift by 5
        	buffer[decoder_ind] = sample >> 5;
			cli();
        	decoder_ind++;
        	SREG |= sreg;
        	if(decoder_ind>=BUFFER_SIZE)
			{
          		decoder_ind = 0;
			}
	  	} else
		{
			outOfData = true;
		}
		return false;		//may not be full          
	  } else
	  {
		return true;		//full
	  }
	}
	void fillBuffer()
	{
		while (!pumpBuffer());
	}
private:
	bool isRunning;		//used to pause playback
	bool outOfData;		//indicates we have reached the end of data
	adpcm_state state;
	RegPointer PWMReg;
	bool regPointerSet;
	PGM_P packed;  		// pointer to ADPCM-packed sound record
	Word dataSize;		//size of sound record
	signed int     sample;
	int            buffer[];
	unsigned char   da_ind;
	unsigned char   decoder_ind;

// **************************************************************************
// Q7 notation

	static short    *_dqlntab;

	static short    _dqlntab2[2];
	static short	_dqlntab3[4];
	static short	_dqlntab4[8];
	static short	_dqlntab5[16];

// **************************************************************************
// Q4 notation

	static short    *_witab;

	static short    _witab2[2];
	static short	_witab3[4];
	static short	_witab4[8];
	static short	_witab5[16];
				
// **************************************************************************
// Q9 notation

	static short    *_fitab;

	static short    _fitab2[2];
	static short	_fitab3[4];
	static short	_fitab4[8];
	static short	_fitab5[16];

// **************************************************************************

	int scale_factor_adapt();

	inline long update_yl(long yl, int yu)
	{
		long __ret;
		__asm__ __volatile__
		(
			"com %D0" "\n\t"		/*negate yl*/
			"com %C0" "\n\t"
			"com %B0" "\n\t"
			"neg %A0" "\n\t"
			"sbci %B0, 0xFF" "\n\t"
			"sbci %C0, 0xFF" "\n\t"
			"sbci %D0, 0xFF" "\n\t"
			"asr %D0" "\n\t"		/*shift right by 6*/
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"asr %D0" "\n\t"
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"asr %D0" "\n\t"
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"asr %D0" "\n\t"
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"asr %D0" "\n\t"
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"asr %D0" "\n\t"
			"ror %C0" "\n\t"
			"ror %B0" "\n\t"
			"ror %A0" "\n\t"
			"add %A0, %A1" "\n\t"	/*add yu*/
			"adc %B0, %B1" "\n\t"
			"adc %C0, __zero_reg__" "\n\t"
			"adc %D0, __zero_reg__" "\n\t"
			: "=r" (__ret), "=d" (yl)
			: "r" (yl), "0" (yu)
			:
		);
	return __ret;
	}

	int decode(char i);
	/*
	 * Adaptive predictor, calculates signal estimates se & sez
	 *
	 */

	short predictor(short *sez){

  	long sezi;
  	long tmp;
  
  	// Q16 * Q0 -> 32-bit Q16
  
     
  	tmp = (long)state.b[0] * (long)state.dq[0];
  	sezi = tmp;

  	tmp = (long)state.b[1] * (long)state.dq[1];
  	sezi += tmp;

  	tmp = (long)state.b[2]  * (long)state.dq[2];
  	sezi += tmp;

  	tmp = (long)state.b[3]  * (long)state.dq[3];  
  	sezi += tmp;

  	tmp = (long)state.b[4]  * (long)state.dq[4];  
  	sezi += tmp;

  	tmp = (long)state.b[5]  * (long)state.dq[5];
  	sezi += tmp;  
  
  	*sez = sezi >> 18;

  
  	tmp = (long)state.a[0] * (long)state.sr[0];
  	sezi += tmp;
  
  	tmp = (long)state.a[1] * (long)state.sr[1];
  	sezi += tmp;

  	sezi = sezi >> 18;
  	return (sezi);
	}
	void adaptation_speed_control(int fi, int tr, int y);
	char transition_detection(short dql);

	/*
	 * Converts logaritmic dql to linear domain,
	 * returns dq (15-bit SM).
	 */
	int antilog(int sign, int dql)
	{

	  short dex;	// exponent 
	  short dmn;    // mantissa 
	  short dqmag;	// magnitude

	  if (dql < 0)
	  {
	    return ((sign) ? -1 : 0);
	  } else
	  {		
	    dex = (dql >> 7) & 15;    // exponent (4 bits)
	    dmn = dql & 127;          // mantissa (7 + 1 bits)
	    dmn = 128 + dmn;  
	    dqmag = ((long)dmn << 7) >> (14 - dex);
 	    return (sign ? -dqmag : dqmag);
 	  }
	}

	short update_pred_coeff(int	dq,
	                int	sr,
	                int	dqsez,
                        char    tr,
                        char    pk0,
                        adpcm_state state);

	void initialize_adpcm();
	unsigned char get_code(void);

};


#endif //ADPCM_H
