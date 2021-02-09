	.file	"speck.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_a2p0_c2p0_b2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.data
	.align	2
	.type	hexchar, @object
	.size	hexchar, 16
hexchar:
	.ascii	"0123456789ABCDEF"
	.section	.sdata,"aw"
	.align	2
	.type	uart_rx, @object
	.size	uart_rx, 4
uart_rx:
	.word	1080033280
	.align	2
	.type	uart_tx, @object
	.size	uart_tx, 4
uart_tx:
	.word	1080033284
	.align	2
	.type	uart_stat, @object
	.size	uart_stat, 4
uart_stat:
	.word	1080033288
	.align	2
	.type	uart_ctrl, @object
	.size	uart_ctrl, 4
uart_ctrl:
	.word	1080033292
	.align	2
	.type	gpio, @object
	.size	gpio, 4
gpio:
	.word	1073741824
	.text
	.align	1
	.globl	xdiv_speck_encrypt
	.type	xdiv_speck_encrypt, @function
xdiv_speck_encrypt:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	lw	a5,-40(s0)
	lw	a5,0(a5)
	sw	a5,-20(s0)
	lw	a5,-44(s0)
	lw	a5,0(a5)
	sw	a5,-24(s0)
 #APP
# 55 "./speck.c" 1
	la	t5, 1073741824  
	li	    t6,  1       
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	sw     t6,0(t5)    
	nop
	 
# 0 "" 2
 #NO_APP
	sw	zero,-28(s0)
	j	.L2
.L3:
	lw	a5,-28(s0)
	slli	a5,a5,2
	lw	a4,-36(s0)
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
 #APP
# 57 "./speck.c" 1
	srli	   t0, a4,  8 
	slli      a4, a4, 24 
	or		   a4, a4, t0 
	add   a4, a4, a5 
	xor	   a4, a4, a3 
	slli	   t0, a5,  3 
	srli      a5, a5, 29 
	or		   a5, a5, t0 
	xor	   a5, a5, a4 
	
# 0 "" 2
 #NO_APP
	sw	a4,-20(s0)
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L2:
	lw	a4,-28(s0)
	li	a5,26
	ble	a4,a5,.L3
 #APP
# 59 "./speck.c" 1
	nop
	   nop
	   nop
	   nop
	   nop
	la	t5, 1073741824  
	sw	    zero, 0(t5)  
	nop
	   nop
	   nop
	   nop
	   nop
	
# 0 "" 2
 #NO_APP
	lw	a5,-40(s0)
	lw	a4,-20(s0)
	sw	a4,0(a5)
	lw	a5,-44(s0)
	lw	a4,-24(s0)
	sw	a4,0(a5)
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	xdiv_speck_encrypt, .-xdiv_speck_encrypt
	.align	1
	.globl	xdiv_speck_decrypt
	.type	xdiv_speck_decrypt, @function
xdiv_speck_decrypt:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	lw	a5,-40(s0)
	lw	a5,0(a5)
	sw	a5,-20(s0)
	lw	a5,-44(s0)
	lw	a5,0(a5)
	sw	a5,-24(s0)
 #APP
# 67 "./speck.c" 1
	la	t5, 1073741824  
	li	    t6,  1       
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	sw     t6,0(t5)    
	nop
	 
# 0 "" 2
 #NO_APP
	li	a5,26
	sw	a5,-28(s0)
	j	.L5
.L6:
	lw	a5,-28(s0)
	slli	a5,a5,2
	lw	a4,-36(s0)
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
 #APP
# 69 "./speck.c" 1
	xor   a5, a5, a4 
	srli	   t0, a5,  3 
	slli      a5, a5, 29 
	or		   a5, a5, t0 
	xor   a4, a4, a3 
	sub   	   a4, a4, a5 
	slli	   t0, a4,  8 
	srli      a4, a4, 24 
	or		   a4, a4, t0 
	
# 0 "" 2
 #NO_APP
	sw	a4,-20(s0)
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	addi	a5,a5,-1
	sw	a5,-28(s0)
.L5:
	lw	a5,-28(s0)
	bge	a5,zero,.L6
 #APP
# 71 "./speck.c" 1
	nop
	   nop
	   nop
	   nop
	   nop
	la	t5, 1073741824  
	sw	    zero, 0(t5)  
	nop
	   nop
	   nop
	   nop
	   nop
	
# 0 "" 2
 #NO_APP
	lw	a5,-40(s0)
	lw	a4,-20(s0)
	sw	a4,0(a5)
	lw	a5,-44(s0)
	lw	a4,-24(s0)
	sw	a4,0(a5)
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	xdiv_speck_decrypt, .-xdiv_speck_decrypt
	.align	1
	.globl	conv_speck_encrypt
	.type	conv_speck_encrypt, @function
conv_speck_encrypt:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	lw	a5,-40(s0)
	lw	a5,0(a5)
	sw	a5,-20(s0)
	lw	a5,-44(s0)
	lw	a5,0(a5)
	sw	a5,-24(s0)
 #APP
# 84 "./speck.c" 1
	la	t5, 1073741824  
	li	    t6,  1       
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	sw     t6,0(t5)    
	nop
	 
# 0 "" 2
 #NO_APP
	sw	zero,-28(s0)
	j	.L8
.L9:
	lw	a5,-20(s0)
	rori	a5,a5,8
	sw	a5,-20(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-28(s0)
	slli	a5,a5,2
	lw	a4,-36(s0)
	add	a5,a4,a5
	lw	a5,0(a5)
	lw	a4,-20(s0)
	xor	a5,a4,a5
	sw	a5,-20(s0)
	lw	a4,-24(s0)
	li	a5,3
	rol	a5,a4,a5
	sw	a5,-24(s0)
	lw	a4,-24(s0)
	lw	a5,-20(s0)
	xor	a5,a4,a5
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L8:
	lw	a4,-28(s0)
	li	a5,26
	ble	a4,a5,.L9
 #APP
# 88 "./speck.c" 1
	nop
	   nop
	   nop
	   nop
	   nop
	la	t5, 1073741824  
	sw	    zero, 0(t5)  
	nop
	   nop
	   nop
	   nop
	   nop
	
# 0 "" 2
 #NO_APP
	lw	a5,-40(s0)
	lw	a4,-20(s0)
	sw	a4,0(a5)
	lw	a5,-44(s0)
	lw	a4,-24(s0)
	sw	a4,0(a5)
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	conv_speck_encrypt, .-conv_speck_encrypt
	.align	1
	.globl	conv_speck_decrypt
	.type	conv_speck_decrypt, @function
conv_speck_decrypt:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	lw	a5,-40(s0)
	lw	a5,0(a5)
	sw	a5,-20(s0)
	lw	a5,-44(s0)
	lw	a5,0(a5)
	sw	a5,-24(s0)
 #APP
# 96 "./speck.c" 1
	la	t5, 1073741824  
	li	    t6,  1       
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	nop
	   nop
	   nop
	   nop
	   nop
	sw     t6,0(t5)    
	nop
	 
# 0 "" 2
 #NO_APP
	li	a5,26
	sw	a5,-28(s0)
	j	.L11
.L12:
	lw	a4,-24(s0)
	lw	a5,-20(s0)
	xor	a5,a4,a5
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	rori	a5,a5,3
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	slli	a5,a5,2
	lw	a4,-36(s0)
	add	a5,a4,a5
	lw	a5,0(a5)
	lw	a4,-20(s0)
	xor	a5,a4,a5
	sw	a5,-20(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	sub	a5,a4,a5
	sw	a5,-20(s0)
	lw	a4,-20(s0)
	li	a5,8
	rol	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-28(s0)
	addi	a5,a5,-1
	sw	a5,-28(s0)
.L11:
	lw	a5,-28(s0)
	bge	a5,zero,.L12
 #APP
# 100 "./speck.c" 1
	nop
	   nop
	   nop
	   nop
	   nop
	la	t5, 1073741824  
	sw	    zero, 0(t5)  
	nop
	   nop
	   nop
	   nop
	   nop
	
# 0 "" 2
 #NO_APP
	lw	a5,-40(s0)
	lw	a4,-20(s0)
	sw	a4,0(a5)
	lw	a5,-44(s0)
	lw	a4,-24(s0)
	sw	a4,0(a5)
	nop
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	conv_speck_decrypt, .-conv_speck_decrypt
	.align	1
	.globl	conv_speck_schedule
	.type	conv_speck_schedule, @function
conv_speck_schedule:
	addi	sp,sp,-160
	sw	s0,156(sp)
	addi	s0,sp,160
	sw	a0,-148(s0)
	sw	a1,-152(s0)
	sw	zero,-20(s0)
	j	.L14
.L15:
	lw	a5,-20(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	lw	a4,-152(s0)
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-20(s0)
	slli	a5,a5,2
	addi	a3,s0,-16
	add	a5,a3,a5
	sw	a4,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L14:
	lw	a4,-20(s0)
	li	a5,2
	bleu	a4,a5,.L15
	lw	a5,-152(s0)
	lw	a4,0(a5)
	lw	a5,-148(s0)
	sw	a4,0(a5)
	sw	zero,-20(s0)
	j	.L16
.L17:
	lw	a5,-20(s0)
	slli	a5,a5,2
	addi	a4,s0,-16
	add	a5,a4,a5
	lw	a4,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	rori	a4,a4,8
	slli	a5,a5,2
	addi	a3,s0,-16
	add	a5,a3,a5
	sw	a4,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	slli	a5,a5,2
	addi	a4,s0,-16
	add	a5,a4,a5
	lw	a3,-120(a5)
	lw	a5,-20(s0)
	slli	a5,a5,2
	lw	a4,-148(s0)
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	add	a4,a3,a4
	slli	a5,a5,2
	addi	a3,s0,-16
	add	a5,a3,a5
	sw	a4,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	slli	a5,a5,2
	addi	a4,s0,-16
	add	a5,a4,a5
	lw	a3,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	lw	a4,-20(s0)
	xor	a4,a3,a4
	slli	a5,a5,2
	addi	a3,s0,-16
	add	a5,a3,a5
	sw	a4,-120(a5)
	lw	a5,-20(s0)
	slli	a5,a5,2
	addi	a4,s0,-16
	add	a5,a4,a5
	lw	a3,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	lw	a4,-148(s0)
	add	a5,a4,a5
	li	a4,3
	rol	a4,a3,a4
	sw	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	lw	a4,-148(s0)
	add	a5,a4,a5
	lw	a3,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,3
	slli	a5,a5,2
	addi	a4,s0,-16
	add	a5,a4,a5
	lw	a4,-120(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	slli	a5,a5,2
	lw	a2,-148(s0)
	add	a5,a2,a5
	xor	a4,a3,a4
	sw	a4,0(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L16:
	lw	a4,-20(s0)
	li	a5,25
	bleu	a4,a5,.L17
	nop
	nop
	lw	s0,156(sp)
	addi	sp,sp,160
	jr	ra
	.size	conv_speck_schedule, .-conv_speck_schedule
	.ident	"GCC: (GNU) 10.0.0 20190929 (experimental)"
