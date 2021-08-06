#include "common.h"
#include "aes_enc.h"
#include "xdivinsa.h"

void dump_state( uint8_t * x ) {
 for( int i = 0; i < 16; i++ ) {
    puthex8(x[ i ]);
  }
}

void riscv_main()
{
    uint8_t k[ 16 ]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0};
    uint8_t rk[ ( Nr + 1 ) * ( 4 * Nb ) ];
    uint8_t m[ 16 ]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    uint8_t c[ 16 ];


	int r;
    char st[10]={0};
    gpio_init();
    uart_init();
	XD_RDN(r);

    aes_enc_exp( rk, k );
    
    unsigned int cyccnt1, cyccnt2;
    putstr("\n\n\nTest the AES!\n\n");   

#ifndef XDIVINSA
    putstr("Running the normal AES\n");
#else
    putstr("Running the protected AES\n");
#endif

    for (int i=0;i<7;i++){
	MEASURE(cyccnt1, aes_enc( c, m, rk);)

	uint2str(i, st); putstr(st); putstr("  : plaintext = "); dump_state(m); putstr(";   ciphertext = "); dump_state(c); 
                                 putstr(" process time = "); uint2str(cyccnt1, st); putstr(st); putstr(";\n"); 
    m[0] +=1;
    }   
}

void riscv_irq_delegated_handler() {
}

