#ifndef CHACHA20_H
#define CHACHA20_H
#include <stdint.h>

#define U32_RTL(x,y) ( ( (x) << (y) ) | ( (x) >> ( 32 - (y) ) ) )

#ifdef XDIVINSA
	#define XSTR(x) #x
	#define opt(x) XSTR(x)
   	#define addins "xdi" opt(XDIVINSA) "add"
#else
		#define addins "add"
#endif


#ifdef XDIVINSA
#define chacha20_block(output,input) xdiv_chacha20_block(output,input)
#else 
#define chacha20_block(output,input) conv_chacha20_block(output,input)
#endif
/*!
@brief Implements the ChaCha20 block transformation
*/
void xdiv_chacha20_block(
    uint32_t output[16], //!< Output array
    uint32_t input [16]  //!< Input array
);

void conv_chacha20_block(
    uint32_t output[16], //!< Output array
    uint32_t input [16]  //!< Input array
);

#endif
