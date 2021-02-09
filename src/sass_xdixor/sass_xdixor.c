#include "common.h"
#include "sasstarget.h"

// For evaluating Ttest on xdivinsa xor instruction  ===========
#ifdef XDIVINSA	
		#define XSTR(x) #x
		#define opt(x) XSTR(x)
    	#define xorins "xdi" opt(XDIVINSA) "xor"
#else
		#define xorins "xor"
#endif
#define XD_XOR(r,a,b) asm volatile ( xorins "   %0, %1, %2\n\t" :"=r" (r) : "r" (a), "r" (b)); 

#define INS_NOP asm volatile (         \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        : : );
//================================================================
 
//! Context object for host/target communication
sass_ctx sass;

 
//! Function used to send bytes to the host.
void sass_send_byte_to_host(unsigned char to_send) { 
    // Wait until the TX FIFO is not full
    int unsigned  full = *uart_stat & (0x1<<3);
    while(full) { full = *uart_stat & (0x1<<3);   }
    // Write the byte to the TX FIFO of the UART.
    int unsigned to_write = 0 | to_send;
    *uart_tx = to_write;
}

//! Function used to recieve bytes from the host.
unsigned char sass_recv_byte_from_host() {
    // Wait until the RX FIFO has something in it.
    int unsigned        valid = *uart_stat & (0x1);
    while(valid == 0) { valid = *uart_stat & (0x1); }

    unsigned char tr = *uart_rx & 0xFF;
    return tr;
}

unsigned char sass_custom(){

	set_trigger();     clear_trigger();
    return SASS_STATUS_OK;
}

//! Pointer to function used for encryption
void sass_encrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen)
{
    set_trigger();	clear_trigger();
}

//! Pointer to function used for decryption
void sass_decrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen) 
{
    set_trigger();  clear_trigger();
}

unsigned char sass_t_func(char * datout, char * datin ){

	uint32_t e, f;
	register uint32_t x  asm ("x5");
	register uint32_t y  asm ("x6");
	register uint32_t z  asm ("x7");
	volatile uint32_t r_1, r_0;
	int i;
    char * b;


    for (i=0; i<4; i++) { b= (char*) (&e); b[3-i]=datin[i];}
    for (i=0; i<4; i++) { b= (char*) (&f); b[3-i]=datin[i+4];}
	x   = e; y=f;
	asm volatile ( "li x7,0x00 \n\t");

	SET_TRIGGER
//	INS_NOP
	XD_XOR(z, x, y);
//	XD_ADD(r_1, e, f);
	INS_NOP
	INS_NOP
	CLR_TRIGGER

	r_0 = z;
    for (i=0; i<4; i++) { b= (char*) (&r_1); datout[i  ] = b[3-i];}
    for (i=0; i<4; i++) { b= (char*) (&r_0); datout[i+4] = b[3-i];}
    return SASS_STATUS_OK;
}

void riscv_main()
{
    gpio_init();
    uart_init();

    int r;
    CiRand(r);

    // Setup the SASS context
 	sass_ctx_init(&sass);
    sass.send_byte_to_host   = sass_send_byte_to_host;
    sass.recv_byte_from_host = sass_recv_byte_from_host;
    sass.encrypt             = sass_encrypt;
    sass.decrypt             = sass_decrypt;
    sass.custom              = sass_custom;
	sass.t_func              = sass_t_func;

    sass_target_run(&sass);

}

void riscv_irq_delegated_handler() {
}

