#include "common.h"
#include "sasstarget.h"

// For evaluating Ttest on Multiplication instruction  ===========
typedef uint32_t  limb_t;			
typedef uint64_t dlimb_t;

//  r_1*2^w + r_0  = e*f
#define LIMB_MUL0(r_1,r_0,e,f)   {                            \
  dlimb_t __t  = ( dlimb_t )( e ) *                           \
                 ( dlimb_t )( f ) ;                           \
                                                              \
  r_0 =       ( limb_t )( __t >> 0                    );      \
  r_1 =       ( limb_t )( __t >> (8*sizeof( limb_t )) );      \
}

#define RPI(d) {                                     \
        asm volatile (                               \
            "li t1," #d              "\n\t"         \
            "RPIloop: addi t1, t1, -1  \n\t"         \
            "bnez t1, RPIloop          \n\t"         \
            : : );                                   \
}


#ifdef XDIVINSA_T0
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1(*r_1, *r_0, e, f);
//  dlimb_t   t;
//	CiMult_f2(t, e, f);
//  *r_0 =   ( limb_t )( t >> 0                    );      
//  *r_1 =   ( limb_t )( t >> (8*sizeof( limb_t )) );      
    CLR_TRIG
}
#elif XDIVINSA_T1
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1t1(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_T2
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1t2(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_T3
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1t3(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_T4
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1t4(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_DT1
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1dt1(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_DT2
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1dt2(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_DT3
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1dt3(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif XDIVINSA_DT4
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	CiMult_f1dt4(*r_1, *r_0, e, f);   
    CLR_TRIG
}
#elif RPI_DT0
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
	LIMB_MUL0(*r_1, *r_0, e, f);
    CLR_TRIG
} 
#elif RPI_DT1
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
    RPI(1)
	LIMB_MUL0(*r_1, *r_0, e, f);
    CLR_TRIG
} 
#elif RPI_DT2
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
    RPI(3)
	LIMB_MUL0(*r_1, *r_0, e, f);
    CLR_TRIG
} 
#elif RPI_DT3
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
    RPI(7)
	LIMB_MUL0(*r_1, *r_0, e, f);
    CLR_TRIG
} 
#elif RPI_DT4
void mul0(limb_t * r_1, limb_t* r_0, limb_t e, limb_t f){
    SET_TRIG
    RPI(15)
	LIMB_MUL0(*r_1, *r_0, e, f);
    CLR_TRIG
} 
#endif
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

	limb_t e, f;
	limb_t r_1, r_0;

	int    i;	
	char * b;

    for (i=0; i<4; i++) { b= (char*) (&e); b[3-i]=datin[i];}
    for (i=0; i<4; i++) { b= (char*) (&f); b[3-i]=datin[i+4];}

	//set_trigger();  
	clear_trigger();
	mul0(&r_1, &r_0, e, f);

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

