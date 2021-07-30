#include <stdint.h>
#include <stddef.h>
#include "soc.h"
#include "xdivinsa.h"

#ifndef COMMON_H
#define COMMON_H
static uint8_t hexchar[16]= "0123456789ABCDEF";

//! UART registers
static volatile int unsigned * uart_rx  = (volatile int unsigned*)(UART_BASE+UART_RX);
static volatile int unsigned * uart_tx  = (volatile int unsigned*)(UART_BASE+UART_TX);
static volatile int unsigned * uart_stat= (volatile int unsigned*)(UART_BASE+UART_STAT);
static volatile int unsigned * uart_ctrl= (volatile int unsigned*)(UART_BASE+UART_CTRL);

//! GPIO registers
static volatile int unsigned * gpio = (volatile int unsigned*)(GPIO_BASE);

//! Write a character to stdout.
extern int  putchar(uint32_t c);

//! Write a string to a stream with no trailing '\n' character.
extern void putstr(uint8_t * str);

//! Write a hexadecimal representation of a 32-bit number to stdout.
extern void puthex(uint32_t tp);

//! Write a hexadecimal representation of a 8-bit number to stdout.
extern void puthex8(uint8_t tp);

//! Write a hexadecimal representation of a 64-bit number to stdout.
extern void puthex64(uint64_t tp); 

//
extern void test_dump( char* id, uint32_t * x, int l_x );

unsigned int uint2str(unsigned long long int n, char *st);

//! Sample the clock cycle counter (used for timing checks)
uint32_t rdcycle(); 

//! Sample the clock cycle counter (used for timing checks)
uint32_t rdinstret(); 

//! naieve memset implementation
void *memset(void *s, int c, size_t n);

//! naieve memcpy implementation
void *memcpy(void *str1, const void *str2, size_t n); 

//! Setup the GPIO
void gpio_init();

 //! Used to set the external trigger to start capturing a trace.
void set_trigger();

//! Used to clear the external trigger.
void clear_trigger(); 

void uart_init(); 

#ifdef rocket
#define MEASURE(id, stmt) {                                      \
    uint32_t  id ## _cycle_pre,  id ## _cycle_post;              \
                                                                 \
    asm volatile("csrr %0, mcycle \n":"=r" (id ## _cycle_pre));  \
    stmt;                                                        \
    asm volatile("csrr %0, mcycle \n":"=r" (id ## _cycle_post)); \
                                                                 \
    id = (id ## _cycle_post - id ## _cycle_pre);                 \
  }
#else
#define MEASURE(id, stmt) {                                    \
    uint32_t  id ## _cycle_pre,  id ## _cycle_post;            \
                                                               \
    asm volatile("rdcycle   %0" : "=r" (id ## _cycle_pre));    \
    stmt;                                                      \
    asm volatile("rdcycle   %0" : "=r" (id ## _cycle_post));   \
                                                               \
    id = (id ## _cycle_post - id ## _cycle_pre);               \
  }
#endif
#define SET_TRIGGER asm volatile (         \
        "la	t5, %0  \n\t"                                      \
        "li	    t6,  1       \n\t"                                      \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "sw     t6,0(t5)    \n\t"                                       \
        "nop\n\t "               \
        : : "i" (GPIO_BASE): "t5", "t6" );

#define CLR_TRIGGER asm volatile (         \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        "la	t5, %0  \n\t"                                \
        "sw	    zero, 0(t5)  \n\t"                                      \
        "nop\n\t   nop\n\t   nop\n\t   nop\n\t   nop\n\t"               \
        : : "i" (GPIO_BASE) : "t5", "t6" );

#endif
