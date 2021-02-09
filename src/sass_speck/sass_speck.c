#include "common.h"
#include "sasstarget.h"
#include "speck.h"

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


unsigned char sass_custom(){ set_trigger();     clear_trigger();    return SASS_STATUS_OK;}

//! Pointer to function used for encryption
void sass_encrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen)
{    set_trigger();	clear_trigger();}

//! Pointer to function used for decryption
void sass_decrypt(char * message, char * key, char * cipher, int unsigned keylen, int unsigned mlen) 
{    set_trigger(); clear_trigger();}


unsigned char sass_t_func(char * datout, char * datin ){

	uint32_t ekey[27];
	uint32_t x;
	uint32_t y;


	int    i, j, k, cnt;	
	char * b;
	cnt = 0;
	for (j = 0; j < T; j++){
       	for (i=0; i<4; i++) { b= (char*) (&ekey[j]); b[i]=datin[i+4*cnt];}
		cnt +=1;
	}
	for (i=0; i<4; i++) { b= (char*) (&y); b[i]=datin[i+4*cnt];}
	cnt +=1;
	for (i=0; i<4; i++) { b= (char*) (&x); b[i]=datin[i+4*cnt];}

	//set_trigger();
	speck_decrypt(ekey, &x, &y);
	//clear_trigger();

	cnt = 0;
    for (i=0; i<4; i++) { b= (char*) (&y); datout[i+4*cnt] = b[i];}
	cnt +=1;
	for (i=0; i<4; i++) { b= (char*) (&x); datout[i+4*cnt] = b[i];}
 
    return SASS_STATUS_OK;
}

void riscv_main()
{
    gpio_init();
    uart_init();

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

