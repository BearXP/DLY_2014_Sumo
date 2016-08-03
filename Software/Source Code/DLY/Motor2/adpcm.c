/*****************************************************************************
*
* Atmel Corporation
*
* File              : adpcm.c
* Compiler          : AVR GCC
* Revision          : $Revision: 1.5 $
* Date              : $Date: Thursday, September 19, 2007 12:53:58 UTC $
* Updated by        : $Author: KRB $
*
* Support mail      : avr@atmel.com
*
* Supported devices : This example is written for the ATmega2560. The firmware
*                     should compile on any AVR with at least 4kB of Flash
*                     memory (plus enough room for sound data).
*
* AppNote           : AVR336: ADPCM Decoder.
*
* Description       : ADPCM Decoder routines.
*
****************************************************************************/


#include "adpcm.h"
#include <stdlib.h>			//for abs()
#include <avr/pgmspace.h>	//for pgm_read_byte()



// **************************************************************************
// Q7 notation

short   *ADPCM::_dqlntab;

short   ADPCM::_dqlntab2[2] = {117, 364};
short	ADPCM::_dqlntab3[4] = {-2048, 135, 273, 373};
short	ADPCM::_dqlntab4[8] = {-2048, 4, 135, 213, 273, 323, 373, 425};
short	ADPCM::_dqlntab5[16] = {-2048, -66, 28, 104, 169, 224, 274, 318,
				358, 395, 429, 459, 488, 514, 539, 566};


// **************************************************************************
// Q4 notation

short   *ADPCM::_witab;
short   ADPCM::_witab2[2] = {-22, 439};
short	ADPCM::_witab3[4] = {-4, 30, 137, 582};
short	ADPCM::_witab4[8] = {-12, 18, 41, 64, 112, 198, 355, 1122};
short	ADPCM::_witab5[16] = {14, 14, 24, 39, 40, 41, 58, 100,
			141, 179, 219, 280, 358, 440, 529, 696};


				
// **************************************************************************
// Q9 notation

short   *ADPCM::_fitab;

short   ADPCM::_fitab2[2] = {0, 0xE00};
short	ADPCM::_fitab3[4] = {0, 0x200, 0x400, 0xE00};
short	ADPCM::_fitab4[8] = {0, 0, 0, 0x200, 0x200, 0x200, 0x600, 0xE00};
short	ADPCM::_fitab5[16] = {0, 0, 0, 0, 0, 0x200, 0x200, 0x200,
			0x200, 0x200, 0x400, 0x600, 0x800, 0xA00, 0xC00, 0xC00};

/*
 * Initializes adpcm state (initial values defined in ITU-T G.726
 * recommendation).
 */

void ADPCM::initialize_adpcm()
{
  
  int		n;
  
  state.yl = 34816;
  state.yu = 544;
  state.dms = 0;
  state.dml = 0;
  state.ap = 0;
  
  for (n = 0; n < 2; n++) {
    state.a[n] = 0;
    state.pk[n] = 0;
    //state.sr[n] = 32;  //fp with 4 bit exp, 6 bit mant -> 32 = 1,5 * 2^0
    state.sr[n] = 2;
  }
  
  for (n = 0; n < 6; n++) {
    state.b[n] = 0;
    state.dq[n] = 32;
    state.dq[n] = 2;
  }
  
  state.td = 0;
  
  switch(BITS){
    case 2:
      _dqlntab = _dqlntab2;
      _witab = _witab2;
      _fitab = _fitab2;
      break;
    case 3:
      _dqlntab = _dqlntab3;
      _witab = _witab3;
      _fitab = _fitab3;
      break;
    case 4:
      _dqlntab = _dqlntab4;
      _witab = _witab4;
      _fitab = _fitab4;
      break;
    case 5:
      _dqlntab = _dqlntab5;
      _witab = _witab5;
      _fitab = _fitab5;
      break;
    default:
      _dqlntab = _dqlntab4;
      _witab = _witab4;
      _fitab = _fitab4;
      break;
  }

}


/*
 * Updates adaptive predictor coefficients a and b
 */

short ADPCM::update_pred_coeff(int	dq,     // quantized prediction difference 
	                int	sr,     // reconstructed signal 
	                int	dqsez,  // difference from 2-pole predictor 
                        char    tr,     // transition signal?
                        char    pk0,
                        adpcm_state state){


  short	  a2p;		
  short	  a1ul;		
  char	  pks1;	        
  char    pks2;
  short	  fa1;

	
  if (tr == 1) {	  // transition signal -> zero coeffs
    state.a[0] = 0;
    state.a[1] = 0;
    state.b[0] = 0;
    state.b[1] = 0;
    state.b[2] = 0;
    state.b[3] = 0;
    state.b[4] = 0;
    state.b[5] = 0;
    a2p = 0;
    
  } else {		
    pks1 = pk0 ^ state.pk[0];
    pks2 = pk0 ^ state.pk[1];
    
    a2p = state.a[1] - (state.a[1] >> 7);   	// update a2
    if (dqsez != 0) {
      fa1 = (pks1) ? state.a[0] : -state.a[0];
      if (fa1 < -8191)	      // a[0] < -0.5 ?
	a2p -= 0x100;   
      else if (fa1 > 8191)    // a[0] > 0.5 ?
        a2p += 0xFF;
      else
	a2p += fa1 >> 5;

      if (pks2) 
	if (a2p <= -12160)
          a2p = -12288;   
	else if (a2p >= 12416)
	  a2p = 12288;
	else
	  a2p -= 0x80;
      else if (a2p <= -12416)
	a2p = -12288;
      else if (a2p >= 12160)
	a2p = 12288;
      else
	a2p += 0x80;
    }
    state.a[1] = a2p;
    
    state.a[0] -= state.a[0] >> 8;      // update a1
    if (dqsez != 0)
      if (pks1 == 0)
        state.a[0] += 192;
      else
        state.a[0] -= 192;

    // limit a1: 
    a1ul = 15360 - a2p;
    if (state.a[0] < -a1ul)
      state.a[0] = -a1ul;
    else if (state.a[0] > a1ul)
      state.a[0] = a1ul;

    // update adaptive predictor b coeffs

    if (BITS == 5){  // 5-bit adpcm code
      state.b[0] -= state.b[0] >> 9;
      state.b[1] -= state.b[1] >> 9;
      state.b[2] -= state.b[2] >> 9;
      state.b[3] -= state.b[3] >> 9;
      state.b[4] -= state.b[4] >> 9;
      state.b[5] -= state.b[5] >> 9;
    }else{          // 2-, 3-, and 4-bit
      state.b[0] -= state.b[0] >> 8;
      state.b[1] -= state.b[1] >> 8;
      state.b[2] -= state.b[2] >> 8;
      state.b[3] -= state.b[3] >> 8;
      state.b[4] -= state.b[4] >> 8;
      state.b[5] -= state.b[5] >> 8;
    }  
    
    if ((dq ^ state.dq[0]) >= 0)
      state.b[0] += 128;
    else
      state.b[0] -= 128;

    if ((dq ^ state.dq[1]) >= 0)
      state.b[1] += 128;
    else
      state.b[1] -= 128;
        
    if ((dq ^ state.dq[2]) >= 0)
      state.b[2] += 128;
    else
      state.b[2] -= 128;
       
    if ((dq ^ state.dq[3]) >= 0)
      state.b[3] += 128;
    else
      state.b[3] -= 128;
        
    if ((dq ^ state.dq[4]) >= 0)
      state.b[4] += 128;
    else
      state.b[4] -= 128;
      
    if ((dq ^ state.dq[5]) >= 0)
      state.b[5] += 128;
    else
      state.b[5] -= 128;
  }

  state.dq[5] = state.dq[4];
  state.dq[4] = state.dq[3];
  state.dq[3] = state.dq[2];
  state.dq[2] = state.dq[1];
  state.dq[1] = state.dq[0];
  state.dq[0] = dq;

  state.sr[1] = state.sr[0];
  state.sr[0] = sr;

  return a2p;	 
}




  
/*
 * Decodes ADPCM signal i. Returns 14-bit linear PCM signal.
 */
 
int ADPCM::decode(char i)
{
  short         se, sez;
  short         y;
  short         sr;
  short         dq;
  short         dql;
  short         dqsez;
  char          sign;
  char          tr;
  char          pk0;
  int           dif;
  int           al;
  long          tmp;
  int           ylfrac;
  long          ylint;  
  int           mag;
  int           dqthr;
  int           thr1, thr2;  
  static short  a2p = 0;
  
  
  /********************
   * Adaptive Predictor
   ********************/
  se = predictor(&sez);      
  
  

  /*************************
   * Scale factor adaptation
   *************************/
  if (state.ap >= 256)
    y = state.yu;
  else {
    y = (state.yl >> 6);
    dif = state.yu - y;
    al = state.ap >> 2;

    if (dif > 0)
      y += ((long)((long)dif * (long)al)) >> 6;
    else if (dif < 0)
      y += ((long)((long)dif * (long)al + 0x3f)) >> 6;
  }
   


  /**********************************
   * Difference signal dq calculation
   **********************************/
  sign = i & (1<<(BITS-1));         // split ADPCM code to sign 
  if(i>=(1<<(BITS-1)))              // and magnitude
    i = ((1<<BITS)-1)-i;

  dql = _dqlntab[i] + (y >> 2);
  dq = antilog(sign, dql);          // dq: 15-bit tc

  sr = se + dq;	                    // reconstructed signal calc.
  dqsez = dq + sez;	            // pole prediction diff. 
  pk0 = (dqsez < 0) ? 1 : 0;
       	


  /**********************
   * Transition detection
   **********************/
  if (state.td == 0){  // td(k-1) is 0 if a2(k-1) was >= -0.71875
    tr = 0;
  }else{
    mag = abs(dq);    
    ylint = state.yl >> 15;	
    ylfrac = (state.yl >> 10) & 0x1F;	
    thr1 = (32 + ylfrac) << ylint;	
    thr2 = (ylint > 9) ? 31 << 10 : thr1;
    dqthr = (thr2 + (thr2 >> 1)) >> 1;
    if (mag <= dqthr)
      tr = 0;
    else
      tr = 1;
  }



  /*******************************
   * Scale factor step size update
   *******************************/
  tmp = ((long)_witab[i]) << 5;
  state.yu = y + ((tmp - y) >> 5);  
  
  if (state.yu < 544)	            // limit yu:
    state.yu = 544;                // 1.06 (544 Q9) < yu < 10.00 (5120 Q9)
  else if (state.yu > 5120)
    state.yu = 5120;

  state.yl = update_yl(state.yl, state.yu);
  
  
  
  /***************************************
   * Adaptive predictor coefficient update
   ***************************************/       
  a2p = update_pred_coeff(dq, sr, dqsez, tr, pk0, state);

  state.pk[1] = state.pk[0];
  state.pk[0] = pk0;
        
        
  
  /*****************
   * Tone Detection
   *****************/
  if (tr == 1)
    state.td = 0;	
  else if (a2p < -11776)	
    state.td = 1;	
  else		
    state.td = 0;

  
  
  /*******************************************
   * Adaptation Speed Control parameter update
   *******************************************/
  state.dms += (_fitab[i] - state.dms) >> 5;  
  // fi to Q11, subtraction, mult by 2^-7
  state.dml += (((_fitab[i] << 2) - state.dml) >> 7);

  // transition?
  if (tr == 1)          
    state.ap = 256;    // ap = 1;
  // y < 3 (idle channel)?  
  else if (y < 1536)    // ap = (1-2^-4)ap + 2^-3
    state.ap += (0x200 - state.ap) >> 4;  
  // tone?
  else if (state.td == 1) 
    state.ap += (0x200 - state.ap) >> 4;
  // average magnitude of I changing rapidly?  
  // dms q12, dml q14
  else if (abs((state.dms << 2) - state.dml) >= (state.dml >> 3))
  //else if (abs(state.dms - (state.dml >> 2)) >= (state.dml >> 1))
    state.ap += (0x200 - state.ap) >> 4;
  else
    state.ap += (-state.ap) >> 4;
  
  if(abs(sr) > 8192)
    sr = dq + se - 1;

  return sr;
  
}

/* 
 * Gets one ADPCM code (2, 3, 4 or 5 bits) from flash memory.
 * Returns END_OF_DATA when at the end of recording. Length
 * of recording is INPUTSIZE.
 */
 
unsigned char ADPCM::get_code(void){

  static unsigned int	in_buffer = 0;
  static char			in_bits = 0;
  static long           buf_offset = 0;
  unsigned char			in_byte;
  unsigned char         code;

  if (in_bits < BITS) {
    in_byte = pgm_read_byte(packed + buf_offset++);
    if (buf_offset >= dataSize)
	{
      buf_offset = 0;
      in_buffer = 0;
      in_bits = 0;
      return END_OF_DATA;
    }
    in_buffer |= (in_byte << in_bits);
    in_bits += 8;
  }
  code = in_buffer & ((1 << BITS) - 1);
  in_buffer >>= BITS;
  in_bits -= BITS;
  return code;
}




