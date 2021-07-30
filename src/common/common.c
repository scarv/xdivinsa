#include "common.h"


//! Write a character to serial port.
int  putchar(uint32_t c){
    // Wait until the TX FIFO is not full
    int unsigned  full = *uart_stat & (0x1<<3);
    while(full) { full = *uart_stat & (0x1<<3);  }

    // Write the byte to the TX FIFO of the UART.
    int unsigned to_write = 0 | c;
    *uart_tx = to_write;
}

//! Write a string to a stream with no trailing '\n' character.
void putstr(uint8_t * str){
	int i=0;
	while (str[i] !=0) {
		putchar(str[i]);
		i++;
	}
}

//! Write a hexadecimal representation of a 32-bit number to serial port.
void puthex(uint32_t tp){
	for(int i=0;i<8;i++){
		uint8_t c = hexchar[((tp & 0xF0000000)>>28)];
		putchar(c);
		tp <<=4;		
	}
}

//! Write a hexadecimal representation of a 8-bit number to serial port.
void puthex8(uint8_t tp){
	for(int i=0;i<2;i++){
		uint8_t c = hexchar[((tp & 0xF0)>>4)];
		putchar(c);
		tp <<=4;		
	}
}

//! Write a hexadecimal representation of a 64-bit number to serial port.
void puthex64(uint64_t tp) {
    uint32_t a = tp & 0xFFFFFFFF;
    uint32_t b = (tp >> 32) & 0xFFFFFFFF;
    puthex(b);
    puthex(a);
}

unsigned int uint2str(unsigned long long int n, char *st){
	unsigned int len;
	unsigned int temp;
	short i;
	len = 0;
	
    if (n == 0) {
	st[len]='0'; len++;
	st[len]=0;
	return len;
    }

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

void test_dump( char* id, uint32_t * x, int l_x ) {
  putstr(id);
  putstr(" = long( '");
	 
 for( int i = l_x - 1; i >= 0; i-- ) {
    puthex(x[ i ]);
  }

  putstr( "', 16 ) \n" );  
}

//! Sample the clock cycle counter (used for timing checks)
inline uint32_t rdcycle() {
    uint32_t val;
#ifdef rocket
   asm volatile ("csrr %0, mcycle \n":"=r" (val) ::);
#else
#ifdef picorv
   asm volatile ("rdcycle %0 ;\n":"=r" (val) ::);
#endif
#endif

    return val;
}

//! Sample the clock cycle counter (used for timing checks)
inline uint32_t rdinstret() {
    uint32_t tr;
    asm volatile ("rdinstret %0":"=r"(tr));
    return tr;
}

//! naieve memset implementation
void *memset(void *s, int c, size_t n){
    unsigned char * k = s;
    for(size_t i = 0; i < n; i ++) {
        k[i] = (unsigned char)c;
    }
    return s;
}

//! naieve memcpy implementation
void *memcpy(void *str1, const void *str2, size_t n) {
    unsigned char       * s1 = str1;
    const unsigned char * s2 = str2;
    for(size_t i = 0; i < n; i ++) {
        s1[i] = s2[i];
    }
    return str1;
}

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
    gpio[GPIO_DATA] = 0x0;		// Clear zeroth gpio bit 
}

void uart_init() {        
    *uart_ctrl = 0x3;		// Enable the UART module. Disable interrupts and reset both FIFOs.
}

