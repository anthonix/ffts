
	.globl	_neon_x4
	.align	4
_neon_x4:

	.globl _neon_x8
	.align 4
_neon_x8:

	.globl _neon_x8_t
	.align 4
_neon_x8_t:


	.globl _leaf_ee_init
	.align	4, 0x90
_leaf_ee_init:
 		lea L_sse_constants(%rip), %r9 
		xorl	%eax, %eax

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
		movaps    32(%r9), %xmm0            #83.5
 		movaps    0x0(%r9), %xmm8            #83.5
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
        movslq    (%r8, %rax, 4), %r11                                   #83.44
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
        movslq    8(%r8, %rax, 4), %r12                                  #83.59
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

	.globl _leaf_oo
	.align 4
_leaf_oo:
        movaps    (%r9), %xmm5            #92.7
	.align	4, 0x90
LEAF_OO_1:
LEAF_OO_const_0:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm4                           #93.5
        movaps    %xmm4, %xmm6                                  #93.5
LEAF_OO_const_1:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm7                          #93.5
LEAF_OO_const_2:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm10                         #93.5
        addps     %xmm7, %xmm6                                  #93.5
        subps     %xmm7, %xmm4                                  #93.5
LEAF_OO_const_3:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm8                          #93.5
        movaps    %xmm10, %xmm9                                 #93.5
LEAF_OO_const_4:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm1                          #93.5
        movaps    %xmm6, %xmm3                                  #93.5
LEAF_OO_const_5:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm11                         #93.5
        movaps    %xmm1, %xmm2                                  #93.5
LEAF_OO_const_6:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm14                         #93.5
        movaps    %xmm4, %xmm15                                 #93.5
LEAF_OO_const_7:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm12                          #93.5
        movaps    %xmm14, %xmm13                                #93.5
        movslq    (%r8, %rax, 4), %r11                                   #83.44
        subps     %xmm8, %xmm10                                 #93.5
        addps     %xmm8, %xmm9                                  #93.5
        addps     %xmm11, %xmm2                                 #93.5
        subps     %xmm12, %xmm14                                #93.5
        subps     %xmm11, %xmm1                                 #93.5
        addps     %xmm12, %xmm13                                #93.5
        addps     %xmm9, %xmm3                                  #93.5
        subps     %xmm9, %xmm6                                  #93.5
        xorps     %xmm5, %xmm10                                 #93.5
        xorps     %xmm5, %xmm14                                 #93.5
        shufps    $177, %xmm10, %xmm10                          #93.5
        movaps    %xmm2, %xmm9                                  #93.5
        shufps    $177, %xmm14, %xmm14                          #93.5
        movaps    %xmm6, %xmm7                                  #93.5
        movslq    8(%r8, %rax, 4), %r12                                  #83.59
        addq      $4, %rax                                          #92.18
        addps     %xmm10, %xmm4                                 #93.5
        addps     %xmm13, %xmm9                                 #93.5
        subps     %xmm13, %xmm2                                 #93.5
        subps     %xmm10, %xmm15                                #93.5
        movaps    %xmm1, %xmm13                                 #93.5
        movaps    %xmm2, %xmm8                                  #93.5
        movlhps   %xmm4, %xmm7                                  #93.5
        subps     %xmm14, %xmm13                                #93.5
        addps     %xmm14, %xmm1                                 #93.5
        shufps    $238, %xmm4, %xmm6                            #93.5
        movaps    %xmm3, %xmm14                                 #93.5
        movaps    %xmm9, %xmm4                                  #93.5
        movlhps   %xmm15, %xmm14                                #93.5
        movlhps   %xmm13, %xmm4                                 #93.5
        movlhps   %xmm1, %xmm8                                  #93.5
        shufps    $238, %xmm15, %xmm3                           #93.5
        shufps    $238, %xmm13, %xmm9                           #93.5
        shufps    $238, %xmm1, %xmm2                            #93.5
        movaps    %xmm14, (%rdx,%r11,4)                         #93.5
        movaps    %xmm7, 16(%rdx,%r11,4)                        #93.5
        movaps    %xmm4, 32(%rdx,%r11,4)                        #93.5
        movaps    %xmm8, 48(%rdx,%r11,4)                        #93.5
        movaps    %xmm3, (%rdx,%r12,4)                          #93.5
        movaps    %xmm6, 16(%rdx,%r12,4)                        #93.5
        movaps    %xmm9, 32(%rdx,%r12,4)                        #93.5
        movaps    %xmm2, 48(%rdx,%r12,4)                        #93.5
				cmpq	%rcx, %rax
        jne       LEAF_OO_1       # Prob 95%                      #92.14


	.globl _leaf_eo
	.align 4
_leaf_eo:
LEAF_EO_const_0:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm9                          #88.5
LEAF_EO_const_2:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm7                          #88.5
        movaps    %xmm9, %xmm11                                 #88.5
LEAF_EO_const_3:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm5                           #88.5
        movaps    %xmm7, %xmm6                                  #88.5
LEAF_EO_const_1:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm4                          #88.5
        subps     %xmm5, %xmm7                                  #88.5
        addps     %xmm4, %xmm11                                 #88.5
        subps     %xmm4, %xmm9                                  #88.5
        addps     %xmm5, %xmm6                                  #88.5
        movaps    (%r9), %xmm3            #88.5
        movaps    %xmm11, %xmm10                                #88.5
        xorps     %xmm3, %xmm7                                  #88.5
        movaps    %xmm9, %xmm8                                  #88.5
        shufps    $177, %xmm7, %xmm7                            #88.5
        addps     %xmm6, %xmm10                                 #88.5
        subps     %xmm6, %xmm11                                 #88.5
        subps     %xmm7, %xmm8                                  #88.5
        addps     %xmm7, %xmm9                                  #88.5
        movslq    8(%r8, %rax, 4), %r12                                  #83.59
        movaps    %xmm10, %xmm2                                 #88.5
        movslq    (%r8, %rax, 4), %r11                                   #83.44
        movaps    %xmm11, %xmm1                                 #88.5
        shufps    $238, %xmm8, %xmm10                           #88.5
        addq      $4, %rax                                       #90.5
        shufps    $238, %xmm9, %xmm11                           #88.5
        movaps    %xmm10, (%rdx,%r12,4)                         #88.5
        movaps    %xmm11, 16(%rdx,%r12,4)                       #88.5
LEAF_EO_const_4:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm15                         #88.5
LEAF_EO_const_5:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm12                         #88.5
        movaps    %xmm15, %xmm14                                #88.5
LEAF_EO_const_6:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm4                          #88.5
        addps     %xmm12, %xmm14                                #88.5
        subps     %xmm12, %xmm15                                #88.5
LEAF_EO_const_7:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm13                         #88.5
        movaps    %xmm4, %xmm5                                  #88.5
        movaps    %xmm14, %xmm7                                 #88.5
        addps     %xmm13, %xmm5                                 #88.5
        subps     %xmm13, %xmm4                                 #88.5
        movlhps   %xmm8, %xmm2                                  #88.5
        movaps    %xmm5, %xmm8                                  #88.5
        movlhps   %xmm15, %xmm7                                 #88.5
        xorps     %xmm3, %xmm15                                 #88.5
        movaps    %xmm5, %xmm6                                  #88.5
        subps     %xmm14, %xmm5                                 #88.5
        addps     %xmm14, %xmm6                                 #88.5
        movlhps   %xmm9, %xmm1                                  #88.5
        movaps    %xmm4, %xmm14                                 #88.5
        movlhps   %xmm4, %xmm8                                  #88.5
        movaps    %xmm1, %xmm12                                 #88.5
        shufps    $177, %xmm15, %xmm15                          #88.5
        movaps    0x30(%r9), %xmm11           #88.5
        subps     %xmm15, %xmm14                                #88.5
        mulps     %xmm7, %xmm11                                 #88.5
        addps     %xmm15, %xmm4                                 #88.5
        movaps    0x30(%r9), %xmm9            #88.5
        movaps    0x40(%r9), %xmm15           #88.5
        shufps    $177, %xmm7, %xmm7                            #88.5
        mulps     %xmm8, %xmm9                                  #88.5
        mulps     %xmm15, %xmm7                                 #88.5
        shufps    $177, %xmm8, %xmm8                            #88.5
        subps     %xmm7, %xmm11                                 #88.5
        mulps     %xmm15, %xmm8                                 #88.5
        movaps    %xmm11, %xmm10                                #88.5
        addps     %xmm8, %xmm9                                  #88.5
        shufps    $238, %xmm14, %xmm6                           #88.5
        subps     %xmm9, %xmm11                                 #88.5
        addps     %xmm9, %xmm10                                 #88.5
        xorps     %xmm3, %xmm11                                 #88.5
        movaps    %xmm2, %xmm3                                  #88.5
        shufps    $177, %xmm11, %xmm11                          #88.5
        subps     %xmm10, %xmm3                                 #88.5
        addps     %xmm10, %xmm2                                 #88.5
        addps     %xmm11, %xmm12                                #88.5
        subps     %xmm11, %xmm1                                 #88.5
        shufps    $238, %xmm4, %xmm5                            #88.5
        movaps    %xmm5, 48(%rdx,%r12,4)                        #88.5
        movaps    %xmm6, 32(%rdx,%r12,4)                        #88.5
        movaps    %xmm2, (%rdx,%r11,4)                          #88.5
        movaps    %xmm1, 16(%rdx,%r11,4)                        #88.5
        movaps    %xmm3, 32(%rdx,%r11,4)                        #88.5
        movaps    %xmm12, 48(%rdx,%r11,4)                       #88.5
	

	.globl _leaf_oe
	.align 4
_leaf_oe:
        movaps    (%r9), %xmm0           #59.5
        #movaps    0x20(%r9), %xmm1           #59.5
LEAF_OE_const_2:
				movaps    0xBEBAFECA(%rsi,%rax,4), %xmm6                          #70.5
LEAF_OE_const_3:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm8                           #70.5
        movaps    %xmm6, %xmm10                                 #70.5
        shufps    $228, %xmm8, %xmm10                           #70.5
        movaps    %xmm10, %xmm9                                 #70.5
        shufps    $228, %xmm6, %xmm8                            #70.5
LEAF_OE_const_0:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm12                         #70.5
LEAF_OE_const_1:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm7                          #70.5
        movaps    %xmm12, %xmm14                                #70.5
        movslq    (%r8, %rax, 4), %r11                                   #83.44
        addps     %xmm8, %xmm9                                  #70.5
        subps     %xmm8, %xmm10                                 #70.5
        addps     %xmm7, %xmm14                                 #70.5
        subps     %xmm7, %xmm12                                 #70.5
        movaps    %xmm9, %xmm4                                  #70.5
        movaps    %xmm14, %xmm13                                #70.5
        shufps    $238, %xmm10, %xmm4                           #70.5
        xorps     %xmm0, %xmm10                                 #70.5
        shufps    $177, %xmm10, %xmm10                          #70.5
        movaps    %xmm12, %xmm11                                #70.5
        movaps    %xmm14, %xmm5                                 #70.5
        addps     %xmm9, %xmm13                                 #70.5
        subps     %xmm10, %xmm11                                #70.5
        subps     %xmm9, %xmm14                                 #70.5
        shufps    $238, %xmm12, %xmm5                           #70.5
        addps     %xmm10, %xmm12                                #70.5
        movslq    8(%r8, %rax, 4), %r12                                  #83.59
        movlhps   %xmm11, %xmm13                                #70.5
        movaps    %xmm13, (%rdx,%r11,4)                         #70.5
        movaps    0x30(%r9), %xmm13          #70.5
        movlhps   %xmm12, %xmm14                                #70.5
        movaps    0x40(%r9), %xmm12          #70.5
        mulps     %xmm5, %xmm13                                 #70.5
        shufps    $177, %xmm5, %xmm5                            #70.5
        mulps     %xmm12, %xmm5                                 #70.5
        movaps    %xmm14, 16(%rdx,%r11,4)                       #70.5
        subps     %xmm5, %xmm13                                 #70.5
        movaps    0x30(%r9), %xmm5           #70.5
        mulps     %xmm4, %xmm5                                  #70.5
        shufps    $177, %xmm4, %xmm4                            #70.5
        mulps     %xmm12, %xmm4                                 #70.5
LEAF_OE_const_4:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm9                          #70.5
        addps     %xmm4, %xmm5                                  #70.5
LEAF_OE_const_6:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm7                          #70.5
        movaps    %xmm9, %xmm3                                  #70.5
LEAF_OE_const_7:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm2                          #70.5
        movaps    %xmm7, %xmm6                                  #70.5
LEAF_OE_const_5:
        movaps    0xBEBAFECA(%rsi,%rax,4), %xmm15                         #70.5
        movaps    %xmm13, %xmm4                                 #70.5
        subps     %xmm2, %xmm7                                  #70.5
        addps     %xmm15, %xmm3                                 #70.5
        subps     %xmm15, %xmm9                                 #70.5
        addps     %xmm2, %xmm6                                  #70.5
        subps     %xmm5, %xmm13                                 #70.5
        addps     %xmm5, %xmm4                                  #70.5
        xorps     %xmm0, %xmm7                                  #70.5
        addq      $4, %rax                                       #72.5
        movaps    %xmm3, %xmm2                                  #70.5
        shufps    $177, %xmm7, %xmm7                            #70.5
        movaps    %xmm9, %xmm8                                  #70.5
        xorps     %xmm0, %xmm13                                 #70.5
        addps     %xmm6, %xmm2                                  #70.5
        subps     %xmm7, %xmm8                                  #70.5
        subps     %xmm6, %xmm3                                  #70.5
        addps     %xmm7, %xmm9                                  #70.5
        movaps    %xmm2, %xmm10                                 #70.5
        movaps    %xmm3, %xmm11                                 #70.5
        shufps    $238, %xmm8, %xmm2                            #70.5
        shufps    $238, %xmm9, %xmm3                            #70.5
        movaps    %xmm2, %xmm14                                 #70.5
        shufps    $177, %xmm13, %xmm13                          #70.5
        subps     %xmm4, %xmm14                                 #70.5
        addps     %xmm4, %xmm2                                  #70.5
        movaps    %xmm3, %xmm4                                  #70.5
        subps     %xmm13, %xmm3                                 #70.5
        addps     %xmm13, %xmm4                                 #70.5
        movlhps   %xmm8, %xmm10                                 #70.5
        movlhps   %xmm9, %xmm11                                 #70.5
        movaps    %xmm10, 32(%rdx,%r11,4)                       #70.5
        movaps    %xmm11, 48(%rdx,%r11,4)                       #70.5
        movaps    %xmm2, (%rdx,%r12,4)                          #70.5
        movaps    %xmm3, 16(%rdx,%r12,4)                        #70.5
        movaps    %xmm14, 32(%rdx,%r12,4)                       #70.5
        movaps    %xmm4, 48(%rdx,%r12,4)                        #70.5
	
	
	.globl	_leaf_end
	.align	4
_leaf_end:
	
	
	.globl _sse_leaf_ee_offsets
	.globl _sse_leaf_oo_offsets
	.globl _sse_leaf_eo_offsets
	.globl _sse_leaf_oe_offsets
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
_sse_leaf_oo_offsets:
	.long LEAF_OO_const_0-_leaf_oo+0x4
	.long LEAF_OO_const_1-_leaf_oo+0x4
	.long LEAF_OO_const_2-_leaf_oo+0x5
	.long LEAF_OO_const_3-_leaf_oo+0x5
	.long LEAF_OO_const_4-_leaf_oo+0x4
	.long LEAF_OO_const_5-_leaf_oo+0x5
	.long LEAF_OO_const_6-_leaf_oo+0x5
	.long LEAF_OO_const_7-_leaf_oo+0x5
_sse_leaf_eo_offsets:
	.long LEAF_EO_const_0-_leaf_eo+0x5
	.long LEAF_EO_const_1-_leaf_eo+0x4
	.long LEAF_EO_const_2-_leaf_eo+0x4
	.long LEAF_EO_const_3-_leaf_eo+0x4
	.long LEAF_EO_const_4-_leaf_eo+0x5
	.long LEAF_EO_const_5-_leaf_eo+0x5
	.long LEAF_EO_const_6-_leaf_eo+0x4
	.long LEAF_EO_const_7-_leaf_eo+0x5
_sse_leaf_oe_offsets:
	.long LEAF_OE_const_0-_leaf_oe+0x5
	.long LEAF_OE_const_1-_leaf_oe+0x4
	.long LEAF_OE_const_2-_leaf_oe+0x4
	.long LEAF_OE_const_3-_leaf_oe+0x5
	.long LEAF_OE_const_4-_leaf_oe+0x5
	.long LEAF_OE_const_5-_leaf_oe+0x5
	.long LEAF_OE_const_6-_leaf_oe+0x4
	.long LEAF_OE_const_7-_leaf_oe+0x4
	
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
