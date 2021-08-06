.include "aes_macro.S"

.macro  XDIOR RD RS1, RS2
#ifdef XDIVINSA
	#if   (XDIVINSA==1)
       xdi1xor \RD, \RS1, \RS2
	#elif (XDIVINSA==2)
        xdi2xor \RD, \RS1, \RS2
	#elif (XDIVINSA==3)
        xdi3xor \RD, \RS1, \RS2
	#elif (XDIVINSA==4)
        xdi4xor \RD, \RS1, \RS2
	#elif (XDIVINSA==5)
        xdi5xor \RD, \RS1, \RS2
	#elif (XDIVINSA==6)		
        xdi6xor \RD, \RS1, \RS2
	#elif (XDIVINSA==7)
        xdi7xor \RD, \RS1, \RS2
	#else
		xdixor \RD, \RS1, \RS2
	#endif
#else
	    xor \RD, \RS1, \RS2
#endif
	    xor \RD, \RD, x0
.endm

.macro AES_KEY_XD RS0 RS1 RS2 RS3 RT RK
	lw 	    \RT,  0(\RK)
	XDIOR	\RS0, 	\RS0, \RT
	lw 	    \RT,  4(\RK)
	XDIOR   \RS1,	\RS1, \RT
	lw 	    \RT,  8(\RK)
	XDIOR   \RS2, 	\RS2, \RT
	lw 	    \RT, 12(\RK)
	XDIOR   \RS3, 	\RS3, \RT
.endm

.extern AES_ENC_SBOX
.extern AES_MULX

.section .text
.global aes_enc
aes_enc:
// a0 =  uint32_t * r
// a1 =  uint32_t * m 
// a2 =  uint32_t * k

addi    sp,sp,-20
sw      s1,4(sp)
sw      s2,8(sp)
sw      s3,12(sp)
sw      s4,16(sp)

la a3, AES_ENC_SBOX
la a4, AES_MULX

//load
AES_LDW_T_RV	t0,t1,t2,t3, a1, a0

//init round	
//key
AES_KEY_XD 	t0,t1,t2,t3,t5,a2
//store
AES_STW_RV	t0,t1,t2,t3,a0

//iterative rounds
li s1, 9
rnd_loop:
  addi a2, a2, 0x10
  //load & sub
  AES_ENC_SUB_RV  0,  1,   2,  3, t0, t5, a0, a3
  AES_ENC_SUB_RV  4,  5,   6,  7, t1, t5, a0, a3
  AES_ENC_SUB_RV  8,  9,  10, 11, t2, t5, a0, a3
  AES_ENC_SUB_RV  12, 13, 14, 15, t3, t5, a0, a3
  //row
  AES_ENC_ROW_RV  t0,t1,t2,t3,t5
  //mix
  AES_ENC_MIX_RV  t4,t5,t6,a7, t0,t1,t2,t3, s2,s3,s4
  //key
  AES_KEY_XD      t4,t5,t6,a7,t0,a2
  //store
  AES_STW_RV      t4,t5,t6,a7,a0

  addi s1, s1, -1
  bgtz s1, rnd_loop

//final round
addi a2, a2, 0x10
//load & sub
AES_ENC_SUB_RV  0,  1,   2,  3, t0, t5, a0, a3
AES_ENC_SUB_RV  4,  5,   6,  7, t1, t5, a0, a3
AES_ENC_SUB_RV  8,  9,  10, 11, t2, t5, a0, a3
AES_ENC_SUB_RV  12, 13, 14, 15, t3, t5, a0, a3
//row
AES_ENC_ROW_RV  t0,t1,t2,t3,t5
//key
AES_KEY_XD      t0,t1,t2,t3,t5, a2
//store
AES_STW_T_RV    t0,t1,t2,t3, a0

lw      s4,16(sp)
lw      s3,12(sp)
lw      s2,8(sp)
lw      s1,4(sp)
addi    sp,sp,20
ret


