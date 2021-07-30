#ifndef SOC_H
#define SOC_H

/* UART */
#define UART_BASE	  0x40600000
#define UART_RX         	0x00
#define UART_TX         	0x04
#define UART_STAT       	0x08
#define UART_CTRL       	0x0C

/* GPIO */
#define GPIO_BASE  	  0x40000000
#define GPIO_DATA           0x00
#define GPIO_CTRL           0x01

/* BRAM */
#define BRAM_BASE  	  0xC0000000

#endif /* SOC_H */
