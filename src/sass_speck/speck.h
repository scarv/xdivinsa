#ifndef SPECK_H
#define SPECK_H
#include <stdint.h>

#define U32_RTL(x,y) ( ( (x) << (y) ) | ( (x) >> ( 32 - (y) ) ) )
#define U32_RTR(x,y) ( ( (x) >> (y) ) | ( (x) << ( 32 - (y) ) ) )

#define T 27
#define m 4
#define n 32

#ifdef XDIVINSA
	#define XSTR(x) #x
	#define opt(x) XSTR(x)
   	#define addins "xdi" opt(XDIVINSA) "add"
	#define xorins "xdi" opt(XDIVINSA) "xor"
#else
		#define addins "add"
		#define xorins "xor"
#endif

#if defined XDIVINSA
	#define speck_encrypt(ekey,px,py) xdiv_speck_encrypt(ekey,px,py)
	#define speck_decrypt(ekey,px,py) xdiv_speck_decrypt(ekey,px,py)
#else 
	#define speck_encrypt(ekey,px,py) conv_speck_encrypt(ekey,px,py)
	#define speck_decrypt(ekey,px,py) conv_speck_decrypt(ekey,px,py)
#endif

void xdiv_speck_encrypt(uint32_t *ekey, uint32_t *px, uint32_t *py);
void xdiv_speck_decrypt(uint32_t *ekey, uint32_t *px, uint32_t *py);

void conv_speck_init(uint32_t *ekey, uint32_t *key);
void conv_speck_encrypt(uint32_t *ekey, uint32_t *px, uint32_t *py);
void conv_speck_decrypt(uint32_t *ekey, uint32_t *px, uint32_t *py);


#endif
