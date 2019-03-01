#include "soc.h"
#include "xdivinsa.h"

//! UART registers
static volatile int unsigned * uart_rx  = (volatile int unsigned*)(UART_BASE+UART_RX);
static volatile int unsigned * uart_tx  = (volatile int unsigned*)(UART_BASE+UART_TX);
static volatile int unsigned * uart_stat= (volatile int unsigned*)(UART_BASE+UART_STAT);
static volatile int unsigned * uart_ctrl= (volatile int unsigned*)(UART_BASE+UART_CTRL);

//! GPIO registers
static volatile int unsigned * gpio = (volatile int unsigned*)(GPIO_BASE);

//! Setup the GPIO
void gpio_init(){
    gpio[GPIO_CTRL] = 0x0;		// Configure all pins as outputs.
}

 //! Used to set the external trigger to start capturing a trace.
void set_trigger() {
    gpio[GPIO_DATA] |= 0x1;		// Set zeroth gpio bit
}

//! Used to clear the external trigger.
void clear_trigger() {    
    gpio[GPIO_DATA] &= 0x0;		// Clear zeroth gpio bit
}

void uart_init() {        
    *uart_ctrl = 0x3;		// Enable the UART module. Disable interrupts and reset both FIFOs.
}
 

void send_byte_to_host(unsigned char to_send) {
    // Wait until the TX FIFO is not full
    int unsigned  full = *uart_stat & (0x1<<3);
    while(full) { full = *uart_stat & (0x1<<3);  }
    // Write the byte to the TX FIFO of the UART.
    int unsigned to_write = 0 | to_send;
    *uart_tx = to_write;
}

void txstr(char *st){
	unsigned int j=0;
	while(st[j] != 0){ send_byte_to_host(st[j]);  j++; }
}

unsigned char recv_byte_from_host() {
    // Wait until the RX FIFO has something in it.
    unsigned int        valid = *uart_stat & (0x1);
    while(valid == 0) { valid = *uart_stat & (0x1); }

    unsigned char tr = *uart_rx & 0xFF;    
    return tr;
}

static inline unsigned int rdcycle() {
  unsigned int val;
#ifdef rocket
  asm volatile ("csrr %0, mcycle \n":"=r" (val) ::);
#else
#ifdef picorv
  asm volatile ("rdcycle %0 ;\n":"=r" (val) ::);
#endif
#endif
  return val;
}

unsigned int uint2str(unsigned long long int n, char *st){
	unsigned int len;
	unsigned int temp;
	short i;
	len = 0;
	
	for(temp=n;temp!=0;temp/=10,len++);    
    
	for(i=len-1,temp=n;i>=0;i--) {
		st[i]=(temp%10)+0x30;
      	temp/=10;
    }

    n=n>>32;
	if (n > 0){
    	st[len]='+'; len++;
		int j=len;
		for(temp=n;temp!=0;temp/=10,len++);    
    
		for(i=len-1,temp=n;i>=j;i--) {
			st[i]=(temp%10)+0x30;
      		temp/=10;
    	}
    
		st[len]='*'; len++;
		st[len]='2'; len++; st[len]='^'; len++; st[len]='3'; len++;st[len]='2'; len++;
	}
	st[len]=0;
	return len;
}


int riscv_main() {

char st[10]={0};
unsigned int len;

gpio[GPIO_CTRL] = 0x0;
gpio[GPIO_DATA] |= 0x1;
uart_init();
gpio[GPIO_DATA] |= 0x0;


int i;
volatile unsigned int a, b, c0, c1;
volatile unsigned long long int c;
unsigned int t_beg,t_end;

CiRand(a); //init

txstr("\n\n\nHello from RISC-V CPU to test the xdivinsa instructions!\n\n");

txstr("\nTest reading TRNG using custom instruction\n");
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiRand(c0);
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr(":cirand = "); uint2str(c0, st); txstr(st); txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	}

txstr("\nTest XDivinsa instructions\n");
CiRand(a); 
//a &= 0xFFFF;
txstr("first  number a="); uint2str(a, st); txstr(st); txstr(";\n");

CiRand(b); 
//b &= 0xFFFF;
txstr("second number b="); uint2str(b, st); txstr(st); txstr(";\n");

txstr("sizeof(c) ="); uint2str(sizeof(c), st); txstr(st); txstr(";\n");

txstr("Running normal instruction\n");
for (i=1;i<7;i++){
	
	t_beg=rdcycle();
	c = (long long int) a+b;
	t_end=rdcycle();
	//printf("%d: add = %d process time= %d\n", i, c, t_end-t_beg);
	uint2str(i, st); txstr(st); txstr("  :addi = "); uint2str(c, st); txstr(st); txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
}
  
for (i=1;i<7;i++){
	t_beg=rdcycle(); 
	c =(long long int) a*b;
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr("  :mult = "); uint2str(c, st); txstr(st); txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	}

txstr("\nRunning custom instruction f1\n"); 
for (i=1;i<7;i++){ 
	t_beg=rdcycle(); 
	CiAddi_f1(c1, c0, a, b);   
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr(":ciaddi = "); uint2str(c0, st); txstr(st); txstr(", "); uint2str(c1, st); txstr(st); 
	txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	} 
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiMult_f1(c1, c0, a, b);
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr(":cimult = "); uint2str(c0, st); txstr(st); txstr(", "); uint2str(c1, st); txstr(st);
	txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	}

txstr("\nRunning custom instruction f2\n"); 
for (i=1;i<7;i++){ 
	t_beg=rdcycle();
	CiAddi_f2(c, a, b);   
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr(":ciaddi = "); uint2str(c, st); txstr(st); txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	} 
for (i=1;i<7;i++){
	t_beg=rdcycle();
	CiMult_f2(c, a, b);
	t_end=rdcycle();
	uint2str(i, st); txstr(st); txstr(":cimult = "); uint2str(c, st); txstr(st); txstr(" process time= "); uint2str(t_end-t_beg, st); txstr(st); txstr(";\n"); 
	}
}
void riscv_irq_delegated_handler() {

}
