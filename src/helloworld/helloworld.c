#include "soc.h"
//! UART registers
static volatile int unsigned * uart_rx  = (volatile int unsigned*)(UART_BASE+UART_RX);
static volatile int unsigned * uart_tx  = (volatile int unsigned*)(UART_BASE+UART_TX);
static volatile int unsigned * uart_stat= (volatile int unsigned*)(UART_BASE+UART_STAT);
static volatile int unsigned * uart_ctrl= (volatile int unsigned*)(UART_BASE+UART_CTRL);

void uart_init() {
    
    // Enable the UART module. Disable interrupts and reset both
    // FIFOs.
    *uart_ctrl = 0x3;
}

//! Function used to recieve bytes from the host.
unsigned char recv_byte_from_host() {
    
    // Wait until the RX FIFO has something in it.
    int unsigned valid = *uart_stat & (0x1);
    while(valid == 0) {
        valid = *uart_stat & (0x1);
    }

    unsigned char tr = *uart_rx & 0xFF;
    
    return tr;
}
 
//! Function used to send bytes to the host.
void send_byte_to_host(unsigned char to_send) {
    // Wait until the TX FIFO is not full
    int unsigned full = *uart_stat & (0x1<<3);
    while(full) {
        full = *uart_stat & (0x1<<3);
    }

    // Write the byte to the TX FIFO of the UART.
    int unsigned to_write = 0 | to_send;
    *uart_tx = to_write;
}

void txstr(char *st, unsigned int len){
	unsigned int j;
	for(j=0;j<len;j++) send_byte_to_host(st[j]);
}

void riscv_main()
{
	int i,j;
    
    uart_init();

	//send Hello world!
	txstr("\n\n\nSCARV Project\n",17);
	txstr("Hello World!\n\n\n\n\n",17);

}

void riscv_irq_delegated_handler() {

}
