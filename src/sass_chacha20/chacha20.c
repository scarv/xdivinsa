#include "common.h"
#include "chacha20.h"

#define QRF(S,a,b,c,d) \
    S[a] += S[b]; S[d] ^= S[a]; S[d] = U32_RTL(S[d], 16); \
    S[c] += S[d]; S[b] ^= S[c]; S[b] = U32_RTL(S[b], 12); \
    S[a] += S[b]; S[d] ^= S[a]; S[d] = U32_RTL(S[d],  8); \
    S[c] += S[d]; S[b] ^= S[c]; S[b] = U32_RTL(S[b],  7); 

#define XD_QRF(a,b,c,d) \
	asm volatile (					\
	addins "   %0, %0, %1 \n\t" \
	"xor	   %3, %3, %0 \n\t" \
	"slli	   t0, %3, 16 \n\t" \
	"srli      %3, %3, 16 \n\t" \
	"or		   %3, %3, t0 \n\t" \
\
	"add	   %2, %2, %3 \n\t" \
	"xor	   %1, %1, %2 \n\t" \
	"slli	   t0, %1, 12 \n\t" \
	"srli      %1, %1, 20 \n\t" \
	"or		   %1, %1, t0 \n\t" \
\
	"add	   %0, %0, %1 \n\t" \
	"xor	   %3, %3, %0 \n\t" \
	"slli	   t0, %3,  8 \n\t" \
	"srli      %3, %3, 24 \n\t" \
	"or		   %3, %3, t0 \n\t" \
\
	"add       %2, %2, %3 \n\t" \
	"xor	   %1, %1, %2 \n\t" \
	"slli	   t0, %1,  7 \n\t" \
	"srli      %1, %1, 25 \n\t" \
	"or		   %1, %1, t0 \n\t" \
\
    : "=r" (a), "=r" (b), "=r" (c), "=r" (d) \
    :  "0" (a),  "1" (b),  "2" (c),  "3" (d) \
	: "t0");

#define XD_ADD(r,a,b) asm volatile ( addins "  %0, %1, %2\n\t" :"=r" (r) : "r" (a), "r" (b)); 



/*!
@brief Implements ChaCha20 block transformation using xdivisa
@details codesize =  ? bytes ( ? inst.); runtimes= ? us (? cyc)
*/

void xdiv_chacha20_block(
    uint32_t output[16], //!< Output array
    uint32_t input [16]  //!< Input array
){
    uint32_t ws[16];

    for(int i = 0; i < 16; i ++) {
        ws[i] = input[i];
    }
	SET_TRIGGER
    for(int i = 0; i < 10; i ++) {
        XD_QRF(ws[0], ws[4], ws[ 8], ws[12]);
		XD_QRF(ws[1], ws[5], ws[ 9], ws[13]);
		XD_QRF(ws[2], ws[6], ws[10], ws[14]);
		XD_QRF(ws[3], ws[7], ws[11], ws[15]);
        
        XD_QRF(ws[0], ws[5], ws[10], ws[15]);
		XD_QRF(ws[1], ws[6], ws[11], ws[12]);
		XD_QRF(ws[2], ws[7], ws[ 8], ws[13]);
		XD_QRF(ws[3], ws[4], ws[ 9], ws[14]);
    }

    for(int i = 0; i < 16; i ++) {
        output[i] = input[i] + ws[i];
    }
	CLR_TRIGGER
    return;
}

/*!
@brief Implements the unprotected ChaCha20 block transformation
@details codesize = 936 bytes (234 inst.); runtimes= 163.4 us (4085 cyc)
*/
void conv_chacha20_block(
    uint32_t output[16], //!< Output array
    uint32_t input [16]  //!< Input array
){
    uint32_t ws[16];

    for(int i = 0; i < 16; i ++) {
        ws[i] = input[i];
    }
	SET_TRIGGER
    for(int i = 0; i < 10; i ++) {
        QRF(ws, 0, 4,  8, 12);
        QRF(ws, 1, 5,  9, 13);
        QRF(ws, 2, 6, 10, 14);
        QRF(ws, 3, 7, 11, 15);
        
        QRF(ws, 0, 5, 10, 15);
        QRF(ws, 1, 6, 11, 12);
        QRF(ws, 2, 7,  8, 13);
        QRF(ws, 3, 4,  9, 14);
    }

    for(int i = 0; i < 16; i ++) {
        output[i] = input[i] + ws[i];
    }
	CLR_TRIGGER
    return;
}

