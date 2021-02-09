#include "common.h"
#include "sasstarget.h"
#include "aes_enc.h"

// For evaluating Ttest on AES  ===========
//              LSB
//uint8_t  k[16]={0xf0,0xde,0xbc,0x9a,0x78,0x56,0x34,0x12,0xef,0xcd,0xab,0x89,0x67,0x45,0x23,0x01};
uint8_t  k[16]={0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0};
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

    uint8_t c[ 16 ], m[ 16 ]; 

    uint8_t rk[ ( Nr + 1 ) * ( 4 * Nb ) ];
    aes_enc_exp( rk, k );
        
    memcpy(m, datin, 16);

	SET_TRIGGER
    aes_enc( c, m, rk);
	CLR_TRIGGER

    memcpy(datout, c, 16);

    return SASS_STATUS_OK;
}

void riscv_main()
{
	int r;
    gpio_init();
    uart_init();
	XD_RDN(r);
    
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

