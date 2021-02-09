#include "common.h"
#include "speck.h"

#define SPECK_ENC_RD(x,y,k) \
		x = U32_RTR(x,8); x += y; x ^= k; \
		y = U32_RTL(y,3); y ^= x;

#define SPECK_DEC_RD(x,y,k) \
		y ^= x;         y = U32_RTR(y,3); \
		x ^= k; x -= y; x = U32_RTL(x,8); 

#define XD_SPECK_ENC_RD(x, y, k) \
	asm volatile (					\
	"srli	   t0, %0,  8 \n\t" \
	"slli      %0, %0, 24 \n\t" \
	"or		   %0, %0, t0 \n\t" \
    addins "   %0, %0, %1 \n\t"	\
	"xor	   %0, %0, %4 \n\t" \
\
	"slli	   t0, %1,  3 \n\t" \
	"srli      %1, %1, 29 \n\t" \
	"or		   %1, %1, t0 \n\t" \
	"xor	   %1, %1, %0 \n\t" \
    : "=r" (x), "=r" (y) \
    :  "0" (x),  "1" (y), "r" (k) \
	: "t0");

#define XD_SPECK_DEC_RD(x, y, k) \
	asm volatile (					\
	xorins "   %1, %1, %0 \n\t" \
	"srli	   t0, %1,  3 \n\t" \
	"slli      %1, %1, 29 \n\t" \
	"or		   %1, %1, t0 \n\t" \
\
	xorins "   %0, %0, %4 \n\t" \
    "sub   	   %0, %0, %1 \n\t" \
	"slli	   t0, %0,  8 \n\t" \
	"srli      %0, %0, 24 \n\t" \
	"or		   %0, %0, t0 \n\t" \
    : "=r" (x), "=r" (y) \
    :  "0" (x),  "1" (y), "r" (k) \
	: "t0");





/*!
@brief Implements the encryption of the speck64/128 cipher
@details 
*/
void xdiv_speck_encrypt(uint32_t *ekey, uint32_t *px, uint32_t *py){
    uint32_t x = *px;
    uint32_t y = *py;
	SET_TRIGGER
    for (int i = 0; i < T; i++) {
		XD_SPECK_ENC_RD(x,y,ekey[i]);
    }
	CLR_TRIGGER
    *px = x;
    *py = y;
}

void xdiv_speck_decrypt(uint32_t *ekey, uint32_t *px, uint32_t *py){
    uint32_t x = *px;
    uint32_t y = *py;
	SET_TRIGGER
    for (int i = T-1; i >=0; i--) {
		XD_SPECK_DEC_RD(x,y,ekey[i]);
    }
	CLR_TRIGGER
    *px = x;
    *py = y;
}


/*!
@brief Implements the normal encryption of the speck64/128 cipher
@details codesize = 72 bytes (18 inst.); runtimes= 31.4 us (785 cyc)
*/
void conv_speck_encrypt(uint32_t *ekey, uint32_t *px, uint32_t *py){
    uint32_t x = *px;
    uint32_t y = *py;
	SET_TRIGGER
    for (int i = 0; i < T; i++) {
		SPECK_ENC_RD(x,y,ekey[i]);
    }
	CLR_TRIGGER
    *px = x;
    *py = y;
}

void conv_speck_decrypt(uint32_t *ekey, uint32_t *px, uint32_t *py){
    uint32_t x = *px;
    uint32_t y = *py;
	SET_TRIGGER
    for (int i = T-1; i >= 0; i--) {
		SPECK_DEC_RD(x,y,ekey[i]);
    }
	CLR_TRIGGER
    *px = x;
    *py = y;
}

void conv_speck_schedule(uint32_t *ekey, uint32_t *key)
{
	uint32_t i;
	uint32_t l[29];
	for ( i = 0; i < 3; i++) { l[i] = key[i+1];}

    ekey[0] = key[0];
    for ( i = 0; i < T-1; i++) {
        l[i+3]    = U32_RTR(l[i],8);
        l[i+3]   += ekey[i];
        l[i+3]   ^= i;
        ekey[i+1] = U32_RTL(l[i],3);
        ekey[i+1]^= l[i+3];
    }
}
