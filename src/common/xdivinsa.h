#ifndef XDiVINSA_H
#define XDiVINSA_H
#include <stdint.h>

#define XD_RDN(r) asm volatile     (" xdirdn   %0, %1    \n\t" :"=r" (r) : "r" (r)); 


#define STR1(x) #x
#define STR(x) STR1(x)
#define EXTRACT(a, size, offset) (((~(~0 << size) << offset) & a) >> offset)

#define CUSTOMX_OPCODE(x) CUSTOM_ ## x
#define CUSTOM_0 0b0001011
#define CUSTOM_1 0b0101011
#define CUSTOM_2 0b1011011
#define CUSTOM_3 0b1111011

#define CUSTOMX(X, rd, rs1, rs2, funct)         \
  CUSTOMX_OPCODE(X)                   |         \
  (rd                   << (7))       |         \
  (0x7                  << (7+5))     |         \
  (rs1                  << (7+5+3))   |         \
  (rs2                  << (7+5+3+5)) |         \
  (EXTRACT(funct, 7, 0) << (7+5+3+5+5))

// rd, rs1, and rs2 are data
// rd_n, rs_1, and rs2_n are the register numbers to use
#define ROCC_INST_R_R_R(X, rd, rs1, rs2, funct, rd_n, rs1_n, rs2_n) { \
    register uint32_t rd_  asm ("x" # rd_n);                            \
    register uint32_t rs1_ asm ("x" # rs1_n) = (uint32_t) rs1;          \
    register uint32_t rs2_ asm ("x" # rs2_n) = (uint32_t) rs2;          \
    asm volatile (                                                      \
        ".word " STR(CUSTOMX(X, rd_n, rs1_n, rs2_n, funct)) "\n\t"      \
        : "=r" (rd_)                                                    \
        : [_rs1] "r" (rs1_), [_rs2] "r" (rs2_));                        \
    rd = rd_;                                                           \
  }

#define ROCC_INST_DM_R_R(X, dm, rs1, rs2, funct, rd_n, rs1_n, rs2_n) { \
    register uint32_t rd_  asm ("x" # rd_n);                            \
    register uint32_t rs1_ asm ("x" # rs1_n) = (uint32_t) rs1;          \
    register uint32_t rs2_ asm ("x" # rs2_n) = (uint32_t) rs2;          \
    asm volatile (                                                      \
        ".word " STR(CUSTOMX(X, rd_n,  rs1_n, rs2_n, funct)) "\n\t"     \
        ".word " STR(CUSTOMX(X, rs1_n, rs1_n, rs2_n, 7)) "\n\t"	        \
        "sw %1, %0 \n\t"                                                \
        "sw %2, 4+%0 \n\t"                                              \
        : "=o" (dm)                                                     \
        : "r" (rd_), [_rs1] "r" (rs1_), [_rs2] "r" (rs2_));                        \
  }

//source operands rs1(a2), rs2(a3) via registers, destination register t0
#define ROCC_INST_M1_M0_R_R(X, rd1, rd0, rs1, rs2, funct, rd_n, rs1_n, rs2_n) { \
    register uint32_t rd_  asm ("x" # rd_n);                           \
    register uint32_t rs1_ asm ( "x" # rs1_n) = (uint32_t) rs1;         \
    register uint32_t rs2_ asm ("x" # rs2_n) = (uint32_t) rs2;          \
    asm volatile (                                                      \
        ".word " STR(CUSTOMX(X, rd_n,  rs1_n, rs2_n, funct)) "\n\t"     \
		".word " STR(CUSTOMX(X, rs1_n, rs1_n, rs2_n, 7    )) "\n\t"	    \
        "sw %2, %0 \n\t"                                                \
        "sw %3, %1 \n\t"                                                \
        : "=m" (rd0),"=m" (rd1)                                         \
        : "r" (rd_), [_rs1] "r" (rs1_), [_rs2] "r" (rs2_));             \
  }

// Standard macro that passes rd, rs1, and rs2 via registers
#define ROCC_INST( X, rd,     rs1, rs2, funct)   ROCC_INST_R_R_R(    X, rd,     rs1, rs2, funct, 5, 6, 7)

// Standard macro: destination operand rd(t0), source operands rs1(a2), rs2(a3) via registers, result values @a1, @a0
#define ROCC_INST1(X, m1, m0, rs1, rs2, funct)   ROCC_INST_M1_M0_R_R(X, m1, m0, rs1, rs2, funct, 5, 6, 7)

// Standard macro: destination operand rd(t0), source operands rs1(a1), rs2(a2) via registers, result values @dm
#define ROCC_INST2(X, dm,     rs1, rs2, funct)   ROCC_INST_DM_R_R(   X, dm,     rs1, rs2, funct, 5, 6, 7)


#define XCi 0	// custom instruction set index from [0..3]

#define CiRand(    y              )   ROCC_INST( XCi,  y,       0,   0, 2)

#define CiAddi(    y,     op1, op2)     ROCC_INST( XCi,  y,     op1, op2, 0)
#define CiMult(    y,     op1, op2)     ROCC_INST( XCi,  y,     op1, op2, 1)

#define CiAddi_f1(  m1, m0, op1, op2)   ROCC_INST1(XCi, m1, m0, op1, op2, 0)
#define CiMult_f1(  m1, m0, op1, op2)   ROCC_INST1(XCi, m1, m0, op1, op2, 1)

#define CiAddi_f2(  dm,     op1, op2)   ROCC_INST2(XCi, dm,     op1, op2, 0)
#define CiMult_f2(  dm,     op1, op2)   ROCC_INST2(XCi, dm,     op1, op2, 1)

//fixed delay time instructions
#define CiAddi_dt1( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+1)<<3 + 0)
#define CiAddi_dt2( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+2)<<3 + 0)
#define CiAddi_dt3( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+3)<<3 + 0)
#define CiAddi_dt4( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+4)<<3 + 0) 

#define CiMult_dt1( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+1)<<3 + 1)
#define CiMult_dt2( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+2)<<3 + 1)
#define CiMult_dt3( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+3)<<3 + 1)
#define CiMult_dt4( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+4)<<3 + 1)

#define CiMult_f1dt1(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+1)<<3 + 1)
#define CiMult_f1dt2(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+2)<<3 + 1)
#define CiMult_f1dt3(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+3)<<3 + 1)
#define CiMult_f1dt4(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+4)<<3 + 1)

//random execution time instructions
#define CiAddi_t1( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+1)<<3 + 0 + 4)
#define CiAddi_t2( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+2)<<3 + 0 + 4)
#define CiAddi_t3( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+3)<<3 + 0 + 4)
#define CiAddi_t4( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+4)<<3 + 0 + 4) 

#define CiMult_t1( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+1)<<3 + 1 + 4)
#define CiMult_t2( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+2)<<3 + 1 + 4)
#define CiMult_t3( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+3)<<3 + 1 + 4)
#define CiMult_t4( y,     op1, op2)    ROCC_INST( XCi,  y,     op1, op2, (8+4)<<3 + 1 + 4)

#define CiMult_f1t1(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+1)<<3 + 1 + 4)
#define CiMult_f1t2(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+2)<<3 + 1 + 4)
#define CiMult_f1t3(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+3)<<3 + 1 + 4)
#define CiMult_f1t4(m1, m0, op1, op2)  ROCC_INST1(XCi, m1, m0, op1, op2, (8+4)<<3 + 1 + 4)

#endif  
