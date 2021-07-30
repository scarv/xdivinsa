
/*!
@file boot.S
@brief The basic boot script used to initialise the RISC-V core post reset.
*/

.section .text
.section .text.riscv_reset
riscv_reset_vector_landing:
    j riscv_reset_handler

.section .text.riscv_irq_handler
riscv_irq_vector_landing:
    j riscv_irq_handler

/* ------------------------------------------------------------*/

.section .text

//
// Turn off the register file permutation
//
//.global prf_disable
//prf_disable:
//    li t0, 0xE0000002 
//    mv x0, t0
//    ret

//
// Turn on the register file permutation
//
//.global prf_enable
//prf_enable:
//    li t0, 0xE0000001 
//    mv x0, t0
//    ret

//
// Whenever the core is reset, we end up here from riscy_reset_vector_landing
//
.global riscv_reset_handler
riscv_reset_handler:

    // Zero all of the registers
    mv  x1 , x0
    mv  x2 , x0
    mv  x3 , x0
    mv  x4 , x0
    mv  x5 , x0
    mv  x6 , x0
    mv  x7 , x0
    mv  x8 , x0
    mv  x9 , x0
    mv  x10, x0
    mv  x11, x0
    mv  x12, x0
    mv  x13, x0
    mv  x14, x0
    mv  x15, x0
    mv  x16, x0
    mv  x17, x0
    mv  x18, x0
    mv  x19, x0
    mv  x20, x0
    mv  x21, x0
    mv  x22, x0
    mv  x23, x0
    mv  x24, x0
    mv  x25, x0
    mv  x26, x0
    mv  x27, x0
    mv  x28, x0
    mv  x29, x0
    mv  x30, x0
    mv  x31, x0
    
    // Setup the stack pointer to the top of the shared memory.
    li  sp, 0xC0007FFC
    
    // Jump to main.
    j   riscv_main


//
// We land here after any environment call
//
.global riscv_irq_handler
riscv_irq_handler:
    sw    ra,4(sp)                      // Push current return address
    addi  sp, sp, 0x4                   // to the stack before call.

    call  riscv_irq_delegated_handler   // Jump to the delegated handler.

    mret

