
	.globl	_neon_x4
	.align	4
_neon_x4:

	.globl _neon_x8
	.align 4
_neon_x8:

	.globl _neon_x8_t
	.align 4
_neon_x8_t:



# eax is loop counter (init to 0)
# rcx is loop max count
# rsi is 'in' base pointer
# rdx is 'out' base pointer
# r8 is offsets pointer
# r9 is constants pointer
# scratch: rax r11 r12
	.globl _leaf_ee
	.align	4, 0x90
_leaf_ee:
 		lea L_sse_constants(%rip), %r9 
		movaps    32(%r9), %xmm0            #83.5
 		movaps    0x0(%r9), %xmm8            #83.5
		xorl	%eax, %eax
	.align	4, 0x90
LEAF_EE_1:
LEAF_EE_const_0:
				movaps    0xBEBAFECA(%rsi,%rax,4), %xmm7                           #83.5
LEAF_EE_const_2:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm12                         #83.5
        movaps    %xmm7, %xmm6                                  #83.5
LEAF_EE_const_3:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm10                         #83.5
        movaps    %xmm12, %xmm11                                #83.5
        subps     %xmm10, %xmm12                                #83.5
        addps     %xmm10, %xmm11                                #83.5
        xorps     %xmm8, %xmm12                                 #83.5
LEAF_EE_const_1:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm9                          #83.5
LEAF_EE_const_4:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm10                         #83.5
        addps     %xmm9, %xmm6                                  #83.5
        subps     %xmm9, %xmm7                                  #83.5
LEAF_EE_const_5:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm13                         #83.5
        movaps    %xmm10, %xmm9                                 #83.5
LEAF_EE_const_6:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm3                          #83.5
        movaps    %xmm6, %xmm5                                  #83.5
LEAF_EE_const_7:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm14                          #83.5
        movaps    %xmm3, %xmm15                                 #83.5
        shufps    $177, %xmm12, %xmm12                          #83.5
        movaps    %xmm7, %xmm4                                  #83.5
        movslq    (%r8, %rax, 8), %r11                                   #83.44
        subps     %xmm13, %xmm10                                #83.5
        subps     %xmm14, %xmm3                                 #83.5
        addps     %xmm11, %xmm5                                 #83.5
        subps     %xmm11, %xmm6                                 #83.5
        subps     %xmm12, %xmm4                                 #83.5
        addps     %xmm12, %xmm7                                 #83.5
        addps     %xmm13, %xmm9                                 #83.5
        addps     %xmm14, %xmm15                                #83.5
        movaps    16(%r9), %xmm12           #83.5
        movaps    %xmm9, %xmm1                                  #83.5
        movaps    16(%r9), %xmm11           #83.5
        movaps    %xmm5, %xmm2                                  #83.5
        mulps     %xmm10, %xmm12                                #83.5
        subps     %xmm15, %xmm9                                 #83.5
        addps     %xmm15, %xmm1                                 #83.5
        mulps     %xmm3, %xmm11                                 #83.5
        addps     %xmm1, %xmm2                                  #83.5
        subps     %xmm1, %xmm5                                  #83.5
        shufps    $177, %xmm10, %xmm10                          #83.5
        xorps     %xmm8, %xmm9                                  #83.5
        shufps    $177, %xmm3, %xmm3                            #83.5
        movaps    %xmm6, %xmm1                                  #83.5
        mulps     %xmm0, %xmm10                                 #83.5
        movaps    %xmm4, %xmm13                                 #83.5
        mulps     %xmm0, %xmm3                                  #83.5
        subps     %xmm10, %xmm12                                #83.5
        addps     %xmm3, %xmm11                                 #83.5
        movaps    %xmm12, %xmm3                                 #83.5
        movaps    %xmm7, %xmm14                                 #83.5
        shufps    $177, %xmm9, %xmm9                            #83.5
        subps     %xmm11, %xmm12                                #83.5
        addps     %xmm11, %xmm3                                 #83.5
        subps     %xmm9, %xmm1                                  #83.5
        addps     %xmm9, %xmm6                                  #83.5
        addps     %xmm3, %xmm4                                  #83.5
        subps     %xmm3, %xmm13                                 #83.5
        xorps     %xmm8, %xmm12                                 #83.5
        movaps    %xmm2, %xmm3                                  #83.5
        shufps    $177, %xmm12, %xmm12                          #83.5
        movaps    %xmm6, %xmm9                                  #83.5
        movslq    4(%r8, %rax, 8), %r12                                  #83.59
        movlhps   %xmm4, %xmm3                                  #83.5
				addq	    $4, %rax
        shufps    $238, %xmm4, %xmm2                            #83.5
        movaps    %xmm1, %xmm4                                  #83.5
        movaps    %xmm3, (%rdx,%r11,4)                          #83.5
        subps     %xmm12, %xmm7                                 #83.5
        addps     %xmm12, %xmm14                                #83.5
        movlhps   %xmm7, %xmm4                                  #83.5
        shufps    $238, %xmm7, %xmm1                            #83.5
        movaps    %xmm5, %xmm7                                  #83.5
        movlhps   %xmm13, %xmm7                                 #83.5
        movlhps   %xmm14, %xmm9                                 #83.5
        shufps    $238, %xmm13, %xmm5                           #83.5
        shufps    $238, %xmm14, %xmm6                           #83.5
        movaps    %xmm4, 16(%rdx,%r11,4)                        #83.5
        movaps    %xmm7, 32(%rdx,%r11,4)                        #83.5
        movaps    %xmm9, 48(%rdx,%r11,4)                        #83.5
        movaps    %xmm2, (%rdx,%r12,4)                          #83.5
        movaps    %xmm1, 16(%rdx,%r12,4)                        #83.5
        movaps    %xmm5, 32(%rdx,%r12,4)                        #83.5
        movaps    %xmm6, 48(%rdx,%r12,4)                        #83.5
				cmpq	%rcx, %rax
        jne      LEAF_EE_1 

	.globl _neon_oo
	.align 4
_neon_oo:


	.globl _neon_eo
	.align 4
_neon_eo:
	

	.globl _neon_oe
	.align 4
_neon_oe:
	
	
	.globl	_neon_end
	.align	4
_neon_end:
	
	
	.globl _sse_leaf_ee_offsets
	.align 4
_sse_leaf_ee_offsets:
	.long LEAF_EE_const_0-_leaf_ee+0x4
	.long LEAF_EE_const_1-_leaf_ee+0x5
	.long LEAF_EE_const_2-_leaf_ee+0x5
	.long LEAF_EE_const_3-_leaf_ee+0x5
	.long LEAF_EE_const_4-_leaf_ee+0x5
	.long LEAF_EE_const_5-_leaf_ee+0x5
	.long LEAF_EE_const_6-_leaf_ee+0x4
	.long LEAF_EE_const_7-_leaf_ee+0x5
	
	.section	__TEXT, __const
	.align 4
L_sse_constants:
L_2il0floatpacket.719:
	.long	0x00000000,0x80000000,0x00000000,0x80000000
	.align 4
L_2il0floatpacket.720:
	.long	0x3f3504f3,0x3f3504f3,0x3f3504f3,0x3f3504f3
	.align 4
L_2il0floatpacket.721:
	.long	0xbf3504f3,0x3f3504f3,0xbf3504f3,0x3f3504f3
	.align 4
L_2il0floatpacket.722:
	.long	0x3f800000,0x3f800000,0x3f3504f3,0x3f3504f3
	.align 4
L_2il0floatpacket.723:
	.long	0x00000000,0x00000000,0xbf3504f3,0x3f3504f3
