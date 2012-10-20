/*
 
 This file is part of FFTS -- The Fastest Fourier Transform in the South
  
 Copyright (c) 2012, Anthony M. Blake <amb@anthonix.com>
 Copyright (c) 2012, The University of Waikato 
 
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 	* Redistributions of source code must retain the above copyright
 		notice, this list of conditions and the following disclaimer.
 	* Redistributions in binary form must reproduce the above copyright
 		notice, this list of conditions and the following disclaimer in the
 		documentation and/or other materials provided with the distribution.
 	* Neither the name of the organization nor the
	  names of its contributors may be used to endorse or promote products
 		derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL ANTHONY M. BLAKE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


	.globl	_neon_x4
	.align	4
_neon_x4:

	.globl _neon_x8
	.align 4
_neon_x8:

	.globl _neon_x8_t
	.align 4
_neon_x8_t:


#ifdef __APPLE__
	.globl _leaf_ee_init
_leaf_ee_init:
#else
	.globl leaf_ee_init
leaf_ee_init:
#endif
 		#lea L_sse_constants(%rip), %r9 
		movq 0xe0(%rdi), %r9
		xorl	%eax, %eax
# eax is loop counter (init to 0)
# rcx is loop max count
# rsi is 'in' base pointer
# rdx is 'out' base pointer
# r8 is offsets pointer
# r9 is constants pointer
# scratch: rax r11 r12
#	.align	4, 0x90

# _leaf_ee + 9 needs 16 byte alignment
#ifdef __APPLE__
	.globl _leaf_ee
_leaf_ee:
#else
	.globl leaf_ee
leaf_ee:
#endif
		movaps    32(%r9), %xmm0            #83.5
 		movaps    (%r9), %xmm8            #83.5
LEAF_EE_1:
LEAF_EE_const_0:
				movaps    0xFECA(%rsi,%rax,4), %xmm7                           #83.5
LEAF_EE_const_2:
        movaps    0xFECA(%rsi,%rax,4), %xmm12                         #83.5
        movaps    %xmm7, %xmm6                                  #83.5
LEAF_EE_const_3:
        movaps    0xFECA(%rsi,%rax,4), %xmm10                         #83.5
        movaps    %xmm12, %xmm11                                #83.5
        subps     %xmm10, %xmm12                                #83.5
        addps     %xmm10, %xmm11                                #83.5
        xorps     %xmm8, %xmm12                                 #83.5
LEAF_EE_const_1:
        movaps    0xFECA(%rsi,%rax,4), %xmm9                          #83.5
LEAF_EE_const_4:
        movaps    0xFECA(%rsi,%rax,4), %xmm10                         #83.5
        addps     %xmm9, %xmm6                                  #83.5
        subps     %xmm9, %xmm7                                  #83.5
LEAF_EE_const_5:
        movaps    0xFECA(%rsi,%rax,4), %xmm13                         #83.5
        movaps    %xmm10, %xmm9                                 #83.5
LEAF_EE_const_6:
        movaps    0xFECA(%rsi,%rax,4), %xmm3                          #83.5
        movaps    %xmm6, %xmm5                                  #83.5
LEAF_EE_const_7:
        movaps    0xFECA(%rsi,%rax,4), %xmm14                          #83.5
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
        #movntdq    %xmm3, (%rdx,%r11,4)                          #83.5
        subps     %xmm12, %xmm7                                 #83.5
        addps     %xmm12, %xmm14                                #83.5
        movlhps   %xmm7, %xmm4                                  #83.5
        shufps    $238, %xmm7, %xmm1                            #83.5
        movaps    %xmm5, %xmm7                                  #83.5
        movlhps   %xmm13, %xmm7                                 #83.5
        movlhps   %xmm14, %xmm9                                 #83.5
        shufps    $238, %xmm13, %xmm5                           #83.5
        shufps    $238, %xmm14, %xmm6                           #83.5
        movaps    %xmm3, (%rdx,%r11,4)                          #83.5
        movaps    %xmm4, 16(%rdx,%r11,4)                        #83.5
        movaps    %xmm7, 32(%rdx,%r11,4)                        #83.5
        movaps    %xmm9, 48(%rdx,%r11,4)                        #83.5
        movaps    %xmm2, (%rdx,%r12,4)                          #83.5
        movaps    %xmm1, 16(%rdx,%r12,4)                        #83.5
        movaps    %xmm5, 32(%rdx,%r12,4)                        #83.5
        movaps    %xmm6, 48(%rdx,%r12,4)                        #83.5
				cmpq	%rcx, %rax
        jne      LEAF_EE_1 
        


# _leaf_oo + 4 needs to be 16 byte aligned
#ifdef __APPLE__
	.globl _leaf_oo
_leaf_oo:
#else
	.globl leaf_oo
leaf_oo:
#endif
        movaps    (%r9), %xmm5            #92.7
LEAF_OO_1:
LEAF_OO_const_0:
        movaps    0xFECA(%rsi,%rax,4), %xmm4                           #93.5
        movaps    %xmm4, %xmm6                                  #93.5
LEAF_OO_const_1:
        movaps    0xFECA(%rsi,%rax,4), %xmm7                          #93.5
LEAF_OO_const_2:
        movaps    0xFECA(%rsi,%rax,4), %xmm10                         #93.5
        addps     %xmm7, %xmm6                                  #93.5
        subps     %xmm7, %xmm4                                  #93.5
LEAF_OO_const_3:
        movaps    0xFECA(%rsi,%rax,4), %xmm8                          #93.5
        movaps    %xmm10, %xmm9                                 #93.5
LEAF_OO_const_4:
        movaps    0xFECA(%rsi,%rax,4), %xmm1                          #93.5
        movaps    %xmm6, %xmm3                                  #93.5
LEAF_OO_const_5:
        movaps    0xFECA(%rsi,%rax,4), %xmm11                         #93.5
        movaps    %xmm1, %xmm2                                  #93.5
LEAF_OO_const_6:
        movaps    0xFECA(%rsi,%rax,4), %xmm14                         #93.5
        movaps    %xmm4, %xmm15                                 #93.5
LEAF_OO_const_7:
        movaps    0xFECA(%rsi,%rax,4), %xmm12                          #93.5
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

#ifdef __APPLE__
	.globl _leaf_eo
_leaf_eo:
#else
	.globl leaf_eo
leaf_eo:
#endif
LEAF_EO_const_0:
        movaps    0xFECA(%rsi,%rax,4), %xmm9                          #88.5
LEAF_EO_const_2:
        movaps    0xFECA(%rsi,%rax,4), %xmm7                          #88.5
        movaps    %xmm9, %xmm11                                 #88.5
LEAF_EO_const_3:
        movaps    0xFECA(%rsi,%rax,4), %xmm5                           #88.5
        movaps    %xmm7, %xmm6                                  #88.5
LEAF_EO_const_1:
        movaps    0xFECA(%rsi,%rax,4), %xmm4                          #88.5
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
        shufps    $238, %xmm9, %xmm11                           #88.5
        movaps    %xmm10, (%rdx,%r12,4)                         #88.5
        movaps    %xmm11, 16(%rdx,%r12,4)                       #88.5
LEAF_EO_const_4:
        movaps    0xFECA(%rsi,%rax,4), %xmm15                         #88.5
LEAF_EO_const_5:
        movaps    0xFECA(%rsi,%rax,4), %xmm12                         #88.5
        movaps    %xmm15, %xmm14                                #88.5
LEAF_EO_const_6:
        movaps    0xFECA(%rsi,%rax,4), %xmm4                          #88.5
        addps     %xmm12, %xmm14                                #88.5
        subps     %xmm12, %xmm15                                #88.5
LEAF_EO_const_7:
        movaps    0xFECA(%rsi,%rax,4), %xmm13                         #88.5
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
        addq      $4, %rax                                       #90.5
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
	

#ifdef __APPLE__
	.globl _leaf_oe
_leaf_oe:
#else
	.globl leaf_oe
leaf_oe:
#endif
        movaps    (%r9), %xmm0           #59.5
        #movaps    0x20(%r9), %xmm1           #59.5
LEAF_OE_const_2:
				movaps    0xFECA(%rsi,%rax,4), %xmm6                          #70.5
LEAF_OE_const_3:
        movaps    0xFECA(%rsi,%rax,4), %xmm8                           #70.5
        movaps    %xmm6, %xmm10                                 #70.5
        shufps    $228, %xmm8, %xmm10                           #70.5
        movaps    %xmm10, %xmm9                                 #70.5
        shufps    $228, %xmm6, %xmm8                            #70.5
LEAF_OE_const_0:
        movaps    0xFECA(%rsi,%rax,4), %xmm12                         #70.5
LEAF_OE_const_1:
        movaps    0xFECA(%rsi,%rax,4), %xmm7                          #70.5
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
        movaps    0xFECA(%rsi,%rax,4), %xmm9                          #70.5
        addps     %xmm4, %xmm5                                  #70.5
LEAF_OE_const_6:
        movaps    0xFECA(%rsi,%rax,4), %xmm7                          #70.5
        movaps    %xmm9, %xmm3                                  #70.5
LEAF_OE_const_7:
        movaps    0xFECA(%rsi,%rax,4), %xmm2                          #70.5
        movaps    %xmm7, %xmm6                                  #70.5
LEAF_OE_const_5:
        movaps    0xFECA(%rsi,%rax,4), %xmm15                         #70.5
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
	
	
#ifdef __APPLE__
	.globl	_leaf_end
_leaf_end:
#else
	.globl	leaf_end
leaf_end:
#endif

#ifdef __APPLE__
	.globl	_x_init
_x_init:
#else
	.globl	x_init
x_init:
#endif
        #movaps    L_sse_constants(%rip), %xmm3           #34.3
				movaps   (%r9), %xmm3           #34.3
				movq        0x20(%rdi),%r8
#ifdef __APPLE__
	.globl	_x4
_x4:
#else	
	.globl	x4
x4:
#endif
        movaps    64(%rdx), %xmm0                               #34.3
        movaps    96(%rdx), %xmm1                               #34.3
        movaps    (%rdx), %xmm7                                 #34.3
        movaps    (%r8), %xmm4      #const
        movaps    %xmm7, %xmm9                                  #34.3
        movaps    %xmm4, %xmm6                                  #34.3
        movaps    16(%r8), %xmm2      #const
        mulps     %xmm0, %xmm6                                  #34.3
        mulps     %xmm1, %xmm4                                  #34.3
        shufps    $177, %xmm0, %xmm0                            #34.3
        shufps    $177, %xmm1, %xmm1                            #34.3
        mulps     %xmm2, %xmm0                                  #34.3
        mulps     %xmm1, %xmm2                                  #34.3
        subps     %xmm0, %xmm6                                  #34.3
        addps     %xmm2, %xmm4                                  #34.3
        movaps    %xmm6, %xmm5                                  #34.3
        subps     %xmm4, %xmm6                                  #34.3
        addps     %xmm4, %xmm5                                  #34.3
        movaps    32(%rdx), %xmm8                               #34.3
        xorps     %xmm3, %xmm6                                  #34.3
        shufps    $177, %xmm6, %xmm6                            #34.3
        movaps    %xmm8, %xmm10                                 #34.3
        movaps    112(%rdx), %xmm12                             #34.3
        subps     %xmm5, %xmm9                                  #34.3
        addps     %xmm5, %xmm7                                  #34.3
        addps     %xmm6, %xmm10                                 #34.3
        subps     %xmm6, %xmm8                                  #34.3
        movaps    %xmm7, (%rdx)                                 #34.3
        movaps    %xmm8, 32(%rdx)                               #34.3
        movaps    %xmm9, 64(%rdx)                               #34.3
        movaps    %xmm10, 96(%rdx)                              #34.3
        movaps    32(%r8), %xmm14    #const                          #34.3
        movaps    80(%rdx), %xmm11                              #34.3
        movaps    %xmm14, %xmm0                                 #34.3
        movaps    48(%r8), %xmm13    #const                          #34.3
        mulps     %xmm11, %xmm0                                 #34.3
        mulps     %xmm12, %xmm14                                #34.3
        shufps    $177, %xmm11, %xmm11                          #34.3
        shufps    $177, %xmm12, %xmm12                          #34.3
        mulps     %xmm13, %xmm11                                #34.3
        mulps     %xmm12, %xmm13                                #34.3
        subps     %xmm11, %xmm0                                 #34.3
        addps     %xmm13, %xmm14                                #34.3
        movaps    %xmm0, %xmm15                                 #34.3
        subps     %xmm14, %xmm0                                 #34.3
        addps     %xmm14, %xmm15                                #34.3
        xorps     %xmm3, %xmm0                                  #34.3
        movaps    16(%rdx), %xmm1                               #34.3
        movaps    48(%rdx), %xmm2                               #34.3
        movaps    %xmm1, %xmm4                                  #34.3
        shufps    $177, %xmm0, %xmm0                            #34.3
        movaps    %xmm2, %xmm5                                  #34.3
        addps     %xmm15, %xmm1                                 #34.3
        subps     %xmm0, %xmm2                                  #34.3
        subps     %xmm15, %xmm4                                 #34.3
        addps     %xmm0, %xmm5                                  #34.3
        movaps    %xmm1, 16(%rdx)                               #34.3
        movaps    %xmm2, 48(%rdx)                               #34.3
        movaps    %xmm4, 80(%rdx)                               #34.3
        movaps    %xmm5, 112(%rdx)                              #34.3
				ret	
	
# _x8_soft + 5 needs to be 16 byte aligned
#ifdef __APPLE__
	.globl	_x8_soft
_x8_soft:
#else
	.globl	x8_soft
x8_soft:
#endif
	xorl %eax, %eax
				movq      %rdx, %rbx     
        movq      %r8, %rsi
				leaq       (%rdx,%rcx,4), %r9  
        leaq       (%r9,%rcx,4), %r10 
        leaq       (%r10,%rcx,4), %r11 
        leaq       (%r11,%rcx,4), %r12
        leaq       (%r12,%rcx,4), %r13
        leaq       (%r13,%rcx,4), %r14
        leaq       (%r14,%rcx,4), %r15
X8_soft_loop:   
        movaps    (%rsi), %xmm9       
        movaps    (%r10,%rax,4), %xmm6 
        movaps    %xmm9, %xmm11        
        movaps    (%r11,%rax,4), %xmm7 
        movaps    16(%rsi), %xmm8      
        mulps     %xmm6, %xmm11      
        mulps     %xmm7, %xmm9       
        shufps    $177, %xmm6, %xmm6 
        mulps     %xmm8, %xmm6       
        shufps    $177, %xmm7, %xmm7 
        subps     %xmm6, %xmm11   
        mulps     %xmm7, %xmm8     
        movaps    %xmm11, %xmm10    
        addps     %xmm8, %xmm9       
        movaps    32(%rsi), %xmm15    
        addps     %xmm9, %xmm10        
        subps     %xmm9, %xmm11        
        movaps    (%rbx,%rax,4), %xmm5 
        movaps    %xmm15, %xmm6        
        movaps    (%r12,%rax,4), %xmm12
        movaps    %xmm5, %xmm2         
        movaps    (%r14,%rax,4), %xmm13
        xorps     %xmm3, %xmm11     #const   
        movaps    48(%rsi), %xmm14     
        subps     %xmm10, %xmm2        
        mulps     %xmm12, %xmm6        
        addps     %xmm10, %xmm5        
        mulps     %xmm13, %xmm15       
        movaps    64(%rsi), %xmm10     
        movaps    %xmm5, %xmm0         
        shufps    $177, %xmm12, %xmm12 
        shufps    $177, %xmm13, %xmm13 
        mulps     %xmm14, %xmm12       
        mulps     %xmm13, %xmm14       
        subps     %xmm12, %xmm6        
        addps     %xmm14, %xmm15       
        movaps    (%r13,%rax,4), %xmm7  
        movaps    %xmm10, %xmm13         
        movaps    (%r15,%rax,4), %xmm8    
        movaps    %xmm6, %xmm12      
        movaps    80(%rsi), %xmm9     
        addq      $96, %rsi           
        mulps     %xmm7, %xmm13      
        subps     %xmm15, %xmm6      
        addps     %xmm15, %xmm12     
        mulps     %xmm8, %xmm10      
        subps     %xmm12, %xmm0          
        addps     %xmm12, %xmm5          
        shufps    $177, %xmm7, %xmm7     
        xorps     %xmm3, %xmm6   #const        
        shufps    $177, %xmm8, %xmm8     
        movaps    %xmm2, %xmm12          
        mulps     %xmm9, %xmm7           
        mulps     %xmm8, %xmm9          
        subps     %xmm7, %xmm13        
        addps     %xmm9, %xmm10       
        movaps    (%r9,%rax,4), %xmm4        
        shufps    $177, %xmm11, %xmm11       
        movaps    %xmm4, %xmm1              
        shufps    $177, %xmm6, %xmm6       
        addps     %xmm11, %xmm1           
        subps     %xmm11, %xmm4          
        addps     %xmm6, %xmm12         
        subps     %xmm6, %xmm2         
        movaps    %xmm13, %xmm11      
        movaps    %xmm4, %xmm14      
        movaps    %xmm1, %xmm6      
        subps     %xmm10, %xmm13   
        addps     %xmm10, %xmm11  
        xorps     %xmm3, %xmm13  #const  
        addps     %xmm11, %xmm4                                 
        subps     %xmm11, %xmm14                                
        shufps    $177, %xmm13, %xmm13                          
        movaps    %xmm5, (%rbx,%rax,4)                                 
        movaps    %xmm4, (%r9,%rax,4)                                  
        movaps    %xmm2, (%r10,%rax,4)                                 
        subps     %xmm13, %xmm1                                 
        addps     %xmm13, %xmm6                                 
        movaps    %xmm1, (%r11,%rax,4)                                  
        movaps    %xmm0, (%r12,%rax,4)                                 
        movaps    %xmm14, (%r13,%rax,4)                                
        movaps    %xmm12, (%r14,%rax,4)                                
        movaps    %xmm6, (%r15,%rax,4)                                 
        addq      $4, %rax   
				cmpq	%rcx, %rax
        jne       X8_soft_loop
				ret

#ifdef __APPLE__
	.globl	_x8_hard
_x8_hard:
#else
	.globl	x8_hard
x8_hard:
#endif
        movaps    (%r9), %xmm5           
X8_loop:  
        movaps    (%r8), %xmm9                                 
X8_const_2:
        movaps    0xFECA(%rdx,%rax,4), %xmm6  
        movaps    %xmm9, %xmm11                                 
X8_const_3:
        movaps    0xFECA(%rdx,%rax,4), %xmm7  
        movaps    16(%r8), %xmm8                               
        mulps     %xmm6, %xmm11                                 
        mulps     %xmm7, %xmm9                                  
        shufps    $177, %xmm6, %xmm6                            
        mulps     %xmm8, %xmm6                                  
        shufps    $177, %xmm7, %xmm7                            
        subps     %xmm6, %xmm11                                 
        mulps     %xmm7, %xmm8                                  
        movaps    %xmm11, %xmm10                                
        addps     %xmm8, %xmm9                                  
        movaps    32(%r8), %xmm15                              
        addps     %xmm9, %xmm10                                 
        subps     %xmm9, %xmm11                                 
X8_const_0:
        movaps    0xFECA(%rdx,%rax,4), %xmm3     
        movaps    %xmm15, %xmm6                                 
X8_const_4:
        movaps    0xFECA(%rdx,%rax,4), %xmm12
        movaps    %xmm3, %xmm2                                  
X8_const_6:
        movaps    0xFECA(%rdx,%rax,4), %xmm13
        xorps     %xmm5, %xmm11                                 
        movaps    48(%r8), %xmm14                              
        subps     %xmm10, %xmm2                                 
        mulps     %xmm12, %xmm6                                 
        addps     %xmm10, %xmm3                                 
        mulps     %xmm13, %xmm15                                
        movaps    64(%r8), %xmm10                              
        movaps    %xmm3, %xmm0                                  
        shufps    $177, %xmm12, %xmm12                          
        shufps    $177, %xmm13, %xmm13                          
        mulps     %xmm14, %xmm12                                
        mulps     %xmm13, %xmm14                                
        subps     %xmm12, %xmm6                                 
        addps     %xmm14, %xmm15                                
X8_const_5:
        movaps    0xFECA(%rdx,%rax,4), %xmm7
        movaps    %xmm10, %xmm13                                
X8_const_7:
        movaps    0xFECA(%rdx,%rax,4), %xmm8
        movaps    %xmm6, %xmm12                                 
        movaps    80(%r8), %xmm9                               
        addq      $96, %r8                                     
        mulps     %xmm7, %xmm13                                 
        subps     %xmm15, %xmm6                                 
        addps     %xmm15, %xmm12                                
        mulps     %xmm8, %xmm10                                 
        subps     %xmm12, %xmm0                                 
        addps     %xmm12, %xmm3                                 
        shufps    $177, %xmm7, %xmm7                            
        xorps     %xmm5, %xmm6                                  
        shufps    $177, %xmm8, %xmm8                            
        movaps    %xmm2, %xmm12                                 
        mulps     %xmm9, %xmm7                                  
        mulps     %xmm8, %xmm9                                  
        subps     %xmm7, %xmm13                                 
        addps     %xmm9, %xmm10                                 
X8_const_1:
        movaps    0xFECA(%rdx,%rax,4), %xmm4   
        shufps    $177, %xmm11, %xmm11                          
        movaps    %xmm4, %xmm1                                  
        shufps    $177, %xmm6, %xmm6                            
        addps     %xmm11, %xmm1                                 
        subps     %xmm11, %xmm4                                 
        addps     %xmm6, %xmm12                                 
        subps     %xmm6, %xmm2                                  
        movaps    %xmm13, %xmm11                                
        movaps    %xmm4, %xmm14                                 
        movaps    %xmm1, %xmm6                                  
        subps     %xmm10, %xmm13                                
        addps     %xmm10, %xmm11                                
        xorps     %xmm5, %xmm13                                 
        addps     %xmm11, %xmm4                                 
        subps     %xmm11, %xmm14                                
        shufps    $177, %xmm13, %xmm13                          
X8_const1_0:
        movaps    %xmm3, 0xFECA(%rdx,%rax,4)
X8_const1_1:
        movaps    %xmm4, 0xFECA(%rdx,%rax,4)
X8_const1_2:
        movaps    %xmm2, 0xFECA(%rdx,%rax,4) 
        subps     %xmm13, %xmm1                                 
        addps     %xmm13, %xmm6                                 
X8_const1_3:
        movaps    %xmm1, 0xFECA(%rdx,%rax,4) 
X8_const1_4:
        movaps    %xmm0, 0xFECA(%rdx,%rax,4)
X8_const1_5:
        movaps    %xmm14, 0xFECA(%rdx,%rax,4)
X8_const1_6:
        movaps    %xmm12, 0xFECA(%rdx,%rax,4) 
X8_const1_7:
        movaps    %xmm6, 0xFECA(%rdx,%rax,4)
        addq      $4, %rax   
				cmpq	%rcx, %rax
        jne       X8_loop

#ifdef __APPLE__	
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
#else
	.globl sse_leaf_ee_offsets
	.globl sse_leaf_oo_offsets
	.globl sse_leaf_eo_offsets
	.globl sse_leaf_oe_offsets
	.align 4
sse_leaf_ee_offsets:
	.long LEAF_EE_const_0-leaf_ee+0x4
	.long LEAF_EE_const_1-leaf_ee+0x5
	.long LEAF_EE_const_2-leaf_ee+0x5
	.long LEAF_EE_const_3-leaf_ee+0x5
	.long LEAF_EE_const_4-leaf_ee+0x5
	.long LEAF_EE_const_5-leaf_ee+0x5
	.long LEAF_EE_const_6-leaf_ee+0x4
	.long LEAF_EE_const_7-leaf_ee+0x5
sse_leaf_oo_offsets:
	.long LEAF_OO_const_0-leaf_oo+0x4
	.long LEAF_OO_const_1-leaf_oo+0x4
	.long LEAF_OO_const_2-leaf_oo+0x5
	.long LEAF_OO_const_3-leaf_oo+0x5
	.long LEAF_OO_const_4-leaf_oo+0x4
	.long LEAF_OO_const_5-leaf_oo+0x5
	.long LEAF_OO_const_6-leaf_oo+0x5
	.long LEAF_OO_const_7-leaf_oo+0x5
sse_leaf_eo_offsets:
	.long LEAF_EO_const_0-leaf_eo+0x5
	.long LEAF_EO_const_1-leaf_eo+0x4
	.long LEAF_EO_const_2-leaf_eo+0x4
	.long LEAF_EO_const_3-leaf_eo+0x4
	.long LEAF_EO_const_4-leaf_eo+0x5
	.long LEAF_EO_const_5-leaf_eo+0x5
	.long LEAF_EO_const_6-leaf_eo+0x4
	.long LEAF_EO_const_7-leaf_eo+0x5
sse_leaf_oe_offsets:
	.long LEAF_OE_const_0-leaf_oe+0x5
	.long LEAF_OE_const_1-leaf_oe+0x4
	.long LEAF_OE_const_2-leaf_oe+0x4
	.long LEAF_OE_const_3-leaf_oe+0x5
	.long LEAF_OE_const_4-leaf_oe+0x5
	.long LEAF_OE_const_5-leaf_oe+0x5
	.long LEAF_OE_const_6-leaf_oe+0x4
	.long LEAF_OE_const_7-leaf_oe+0x4
#endif

#ifdef __APPLE__
	.data
#else
	.section .data
#endif
	.p2align 4
#ifdef __APPLE__	
	.globl _sse_constants
_sse_constants:
#else
	.globl sse_constants
sse_constants:
#endif
	.long	0x00000000,0x80000000,0x00000000,0x80000000
	.long	0x3f3504f3,0x3f3504f3,0x3f3504f3,0x3f3504f3
	.long	0xbf3504f3,0x3f3504f3,0xbf3504f3,0x3f3504f3
	.long	0x3f800000,0x3f800000,0x3f3504f3,0x3f3504f3
	.long	0x00000000,0x00000000,0xbf3504f3,0x3f3504f3
#ifdef __APPLE__	
	.globl _sse_constants_inv
_sse_constants_inv:
#else
	.globl sse_constants_inv
sse_constants_inv:
#endif
	.long	0x80000000,0x00000000,0x80000000,0x00000000
	.long	0x3f3504f3,0x3f3504f3,0x3f3504f3,0x3f3504f3
	.long	0x3f3504f3,0xbf3504f3,0x3f3504f3,0xbf3504f3
	.long	0x3f800000,0x3f800000,0x3f3504f3,0x3f3504f3
	.long	0x00000000,0x00000000,0x3f3504f3,0xbf3504f3
