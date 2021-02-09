#ifndef AES_ENC_H
#define AES_ENC_H
#include <stdint.h>

#define AES_128_NB (  4 )
#define AES_128_NK (  4 )
#define AES_128_NR ( 10 )

#define Nb AES_128_NB
#define Nk AES_128_NK
#define Nr AES_128_NR

#define U8_TO_U8_T(r,x) {                                                                     \
  (r)[  0 ] = (x)[  0 ]; (r)[  1 ] = (x)[  4 ]; (r)[  2 ] = (x)[  8 ]; (r)[  3 ] = (x)[ 12 ]; \
  (r)[  4 ] = (x)[  1 ]; (r)[  5 ] = (x)[  5 ]; (r)[  6 ] = (x)[  9 ]; (r)[  7 ] = (x)[ 13 ]; \
  (r)[  8 ] = (x)[  2 ]; (r)[  9 ] = (x)[  6 ]; (r)[ 10 ] = (x)[ 10 ]; (r)[ 11 ] = (x)[ 14 ]; \
  (r)[ 12 ] = (x)[  3 ]; (r)[ 13 ] = (x)[  7 ]; (r)[ 14 ] = (x)[ 11 ]; (r)[ 15 ] = (x)[ 15 ]; \
}

void aes_enc_exp_step( uint8_t* r, const uint8_t* rk, uint8_t rc );
void aes_enc_exp     ( uint8_t* r, const uint8_t*  k             );
extern void aes_enc  ( uint8_t* r, uint8_t* m, uint8_t* k);

extern uint8_t AES_ENC_SBOX[];
extern uint8_t AES_RC[];
extern uint8_t AES_MULX[];
#endif
