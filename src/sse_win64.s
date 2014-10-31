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

    .code64

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

# rcx is a pointer to the ffts_plan
# eax is loop counter (init to 0)
# rbx is loop max count
# rdx is 'in' base pointer
# r8  is 'out' base pointer
# rdi is offsets pointer
# rsi is constants pointer
# scratch: rax r10 r11

        xorl %eax, %eax
        movq (%rcx), %rdi
        movq 0xe0(%rcx), %rsi

# _leaf_ee + 8 needs 16 byte alignment
#ifdef __APPLE__
    .globl _leaf_ee
_leaf_ee:
#else
    .globl leaf_ee
leaf_ee:
#endif
        movaps    32(%rsi), %xmm0                                #83.5
        movaps    (%rsi), %xmm8                                  #83.5
LEAF_EE_1:
LEAF_EE_const_0:
        movaps    0xFECA(%rdx,%rax,4), %xmm7                    #83.5
LEAF_EE_const_2:
        movaps    0xFECA(%rdx,%rax,4), %xmm12                   #83.5
        movaps    %xmm7, %xmm6                                  #83.5
LEAF_EE_const_3:
        movaps    0xFECA(%rdx,%rax,4), %xmm10                   #83.5
        movaps    %xmm12, %xmm11                                #83.5
        subps     %xmm10, %xmm12                                #83.5
        addps     %xmm10, %xmm11                                #83.5
        xorps     %xmm8, %xmm12                                 #83.5
LEAF_EE_const_1:
        movaps    0xFECA(%rdx,%rax,4), %xmm9                    #83.5
LEAF_EE_const_4:
        movaps    0xFECA(%rdx,%rax,4), %xmm10                   #83.5
        addps     %xmm9, %xmm6                                  #83.5
        subps     %xmm9, %xmm7                                  #83.5
LEAF_EE_const_5:
        movaps    0xFECA(%rdx,%rax,4), %xmm13                   #83.5
        movaps    %xmm10, %xmm9                                 #83.5
LEAF_EE_const_6:
        movaps    0xFECA(%rdx,%rax,4), %xmm3                    #83.5
        movaps    %xmm6, %xmm5                                  #83.5
LEAF_EE_const_7:
        movaps    0xFECA(%rdx,%rax,4), %xmm14                   #83.5
        movaps    %xmm3, %xmm15                                 #83.5
        shufps    $177, %xmm12, %xmm12                          #83.5
        movaps    %xmm7, %xmm4                                  #83.5
        movslq    (%rdi, %rax, 4), %r10                         #83.44
        subps     %xmm13, %xmm10                                #83.5
        subps     %xmm14, %xmm3                                 #83.5
        addps     %xmm11, %xmm5                                 #83.5
        subps     %xmm11, %xmm6                                 #83.5
        subps     %xmm12, %xmm4                                 #83.5
        addps     %xmm12, %xmm7                                 #83.5
        addps     %xmm13, %xmm9                                 #83.5
        addps     %xmm14, %xmm15                                #83.5
        movaps    16(%rsi), %xmm12                               #83.5
        movaps    %xmm9, %xmm1                                  #83.5
        movaps    16(%rsi), %xmm11                               #83.5
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
        movslq    8(%rdi, %rax, 4), %r11                        #83.59
        movlhps   %xmm4, %xmm3                                  #83.5
        addq      $4, %rax
        shufps    $238, %xmm4, %xmm2                           #83.5
        movaps    %xmm1, %xmm4                                 #83.5
        subps     %xmm12, %xmm7                                #83.5
        addps     %xmm12, %xmm14                               #83.5
        movlhps   %xmm7, %xmm4                                 #83.5
        shufps    $238, %xmm7, %xmm1                           #83.5
        movaps    %xmm5, %xmm7                                 #83.5
        movlhps   %xmm13, %xmm7                                #83.5
        movlhps   %xmm14, %xmm9                                #83.5
        shufps    $238, %xmm13, %xmm5                          #83.5
        shufps    $238, %xmm14, %xmm6                          #83.5
        movaps    %xmm3, (%r8,%r10,4)                          #83.5
        movaps    %xmm4, 16(%r8,%r10,4)                        #83.5
        movaps    %xmm7, 32(%r8,%r10,4)                        #83.5
        movaps    %xmm9, 48(%r8,%r10,4)                        #83.5
        movaps    %xmm2, (%r8,%r11,4)                          #83.5
        movaps    %xmm1, 16(%r8,%r11,4)                        #83.5
        movaps    %xmm5, 32(%r8,%r11,4)                        #83.5
        movaps    %xmm6, 48(%r8,%r11,4)                        #83.5
        cmpq      %rbx, %rax
        jne       LEAF_EE_1

# _leaf_oo + 3 needs to be 16 byte aligned
#ifdef __APPLE__
    .globl _leaf_oo
_leaf_oo:
#else
    .globl leaf_oo
leaf_oo:
#endif
        movaps    (%rsi), %xmm5                                 #92.7
LEAF_OO_1:
LEAF_OO_const_0:
        movaps    0xFECA(%rdx,%rax,4), %xmm4                    #93.5
        movaps    %xmm4, %xmm6                                  #93.5
LEAF_OO_const_1:
        movaps    0xFECA(%rdx,%rax,4), %xmm7                    #93.5
LEAF_OO_const_2:
        movaps    0xFECA(%rdx,%rax,4), %xmm10                   #93.5
        addps     %xmm7, %xmm6                                  #93.5
        subps     %xmm7, %xmm4                                  #93.5
LEAF_OO_const_3:
        movaps    0xFECA(%rdx,%rax,4), %xmm8                    #93.5
        movaps    %xmm10, %xmm9                                 #93.5
LEAF_OO_const_4:
        movaps    0xFECA(%rdx,%rax,4), %xmm1                    #93.5
        movaps    %xmm6, %xmm3                                  #93.5
LEAF_OO_const_5:
        movaps    0xFECA(%rdx,%rax,4), %xmm11                   #93.5
        movaps    %xmm1, %xmm2                                  #93.5
LEAF_OO_const_6:
        movaps    0xFECA(%rdx,%rax,4), %xmm14                   #93.5
        movaps    %xmm4, %xmm15                                 #93.5
LEAF_OO_const_7:
        movaps    0xFECA(%rdx,%rax,4), %xmm12                   #93.5
        movaps    %xmm14, %xmm13                                #93.5
        movslq    (%rdi, %rax, 4), %r10                         #83.44
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
        movslq    8(%rdi, %rax, 4), %r11                        #83.59
        addq      $4, %rax                                      #92.18
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
        movaps    %xmm14, (%r8,%r10,4)                          #93.5
        movaps    %xmm7, 16(%r8,%r10,4)                         #93.5
        movaps    %xmm4, 32(%r8,%r10,4)                         #93.5
        movaps    %xmm8, 48(%r8,%r10,4)                         #93.5
        movaps    %xmm3, (%r8,%r11,4)                           #93.5
        movaps    %xmm6, 16(%r8,%r11,4)                         #93.5
        movaps    %xmm9, 32(%r8,%r11,4)                         #93.5
        movaps    %xmm2, 48(%r8,%r11,4)                         #93.5
        cmpq      %rbx, %rax
        jne       LEAF_OO_1       # Prob 95%                    #92.14

#ifdef __APPLE__
    .globl _leaf_eo
_leaf_eo:
#else
    .globl leaf_eo
leaf_eo:
#endif
LEAF_EO_const_0:
        movaps    0xFECA(%rdx,%rax,4), %xmm9                    #88.5
LEAF_EO_const_2:
        movaps    0xFECA(%rdx,%rax,4), %xmm7                    #88.5
        movaps    %xmm9, %xmm11                                 #88.5
LEAF_EO_const_3:
        movaps    0xFECA(%rdx,%rax,4), %xmm5                    #88.5
        movaps    %xmm7, %xmm6                                  #88.5
LEAF_EO_const_1:
        movaps    0xFECA(%rdx,%rax,4), %xmm4                    #88.5
        subps     %xmm5, %xmm7                                  #88.5
        addps     %xmm4, %xmm11                                 #88.5
        subps     %xmm4, %xmm9                                  #88.5
        addps     %xmm5, %xmm6                                  #88.5
        movaps    (%rsi), %xmm3                                 #88.5
        movaps    %xmm11, %xmm10                               #88.5
        xorps     %xmm3, %xmm7                                 #88.5
        movaps    %xmm9, %xmm8                                 #88.5
        shufps    $177, %xmm7, %xmm7                           #88.5
        addps     %xmm6, %xmm10                                #88.5
        subps     %xmm6, %xmm11                                #88.5
        subps     %xmm7, %xmm8                                 #88.5
        addps     %xmm7, %xmm9                                 #88.5
        movslq    8(%rdi, %rax, 4), %r11                       #83.59
        movaps    %xmm10, %xmm2                                #88.5
        movslq    (%rdi, %rax, 4), %r10                        #83.44
        movaps    %xmm11, %xmm1                                #88.5
        shufps    $238, %xmm8, %xmm10                          #88.5
        shufps    $238, %xmm9, %xmm11                          #88.5
        movaps    %xmm10, (%r8,%r11,4)                         #88.5
        movaps    %xmm11, 16(%r8,%r11,4)                       #88.5
LEAF_EO_const_4:
        movaps    0xFECA(%rdx,%rax,4), %xmm15                   #88.5
LEAF_EO_const_5:
        movaps    0xFECA(%rdx,%rax,4), %xmm12                   #88.5
        movaps    %xmm15, %xmm14                                #88.5
LEAF_EO_const_6:
        movaps    0xFECA(%rdx,%rax,4), %xmm4                    #88.5
        addps     %xmm12, %xmm14                                #88.5
        subps     %xmm12, %xmm15                                #88.5
LEAF_EO_const_7:
        movaps    0xFECA(%rdx,%rax,4), %xmm13                   #88.5
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
        movaps    0x30(%rsi), %xmm11                             #88.5
        addq      $4, %rax                                      #90.5
        subps     %xmm15, %xmm14                                #88.5
        mulps     %xmm7, %xmm11                                 #88.5
        addps     %xmm15, %xmm4                                 #88.5
        movaps    0x30(%rsi), %xmm9                              #88.5
        movaps    0x40(%rsi), %xmm15                             #88.5
        shufps    $177, %xmm7, %xmm7                            #88.5
        mulps     %xmm8, %xmm9                                  #88.5
        mulps     %xmm15, %xmm7                                 #88.5
        shufps    $177, %xmm8, %xmm8                            #88.5
        subps     %xmm7, %xmm11                                #88.5
        mulps     %xmm15, %xmm8                                #88.5
        movaps    %xmm11, %xmm10                               #88.5
        addps     %xmm8, %xmm9                                 #88.5
        shufps    $238, %xmm14, %xmm6                          #88.5
        subps     %xmm9, %xmm11                                #88.5
        addps     %xmm9, %xmm10                                #88.5
        xorps     %xmm3, %xmm11                                #88.5
        movaps    %xmm2, %xmm3                                 #88.5
        shufps    $177, %xmm11, %xmm11                         #88.5
        subps     %xmm10, %xmm3                                #88.5
        addps     %xmm10, %xmm2                                #88.5
        addps     %xmm11, %xmm12                               #88.5
        subps     %xmm11, %xmm1                                #88.5
        shufps    $238, %xmm4, %xmm5                           #88.5
        movaps    %xmm5, 48(%r8,%r11,4)                        #88.5
        movaps    %xmm6, 32(%r8,%r11,4)                        #88.5
        movaps    %xmm2, (%r8,%r10,4)                          #88.5
        movaps    %xmm1, 16(%r8,%r10,4)                        #88.5
        movaps    %xmm3, 32(%r8,%r10,4)                        #88.5
        movaps    %xmm12, 48(%r8,%r10,4)                       #88.5

#ifdef __APPLE__
    .globl _leaf_oe
_leaf_oe:
#else
    .globl leaf_oe
leaf_oe:
#endif
        movaps    (%rsi), %xmm0                                 #59.5
LEAF_OE_const_2:
        movaps    0xFECA(%rdx,%rax,4), %xmm6                    #70.5
LEAF_OE_const_3:
        movaps    0xFECA(%rdx,%rax,4), %xmm8                    #70.5
        movaps    %xmm6, %xmm10                                 #70.5
        shufps    $228, %xmm8, %xmm10                           #70.5
        movaps    %xmm10, %xmm9                                 #70.5
        shufps    $228, %xmm6, %xmm8                            #70.5
LEAF_OE_const_0:
        movaps    0xFECA(%rdx,%rax,4), %xmm12                   #70.5
LEAF_OE_const_1:
        movaps    0xFECA(%rdx,%rax,4), %xmm7                    #70.5
        movaps    %xmm12, %xmm14                                #70.5
        movslq    (%rdi, %rax, 4), %r10                         #83.44
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
        movslq    8(%rdi, %rax, 4), %r11                        #83.59
        movlhps   %xmm11, %xmm13                                #70.5
        movaps    %xmm13, (%r8,%r10,4)                          #70.5
        movaps    0x30(%rsi), %xmm13                            #70.5
        movlhps   %xmm12, %xmm14                                #70.5
        movaps    0x40(%rsi), %xmm12                            #70.5
        mulps     %xmm5, %xmm13                                 #70.5
        shufps    $177, %xmm5, %xmm5                            #70.5
        mulps     %xmm12, %xmm5                                 #70.5
        movaps    %xmm14, 16(%r8,%r10,4)                        #70.5
        subps     %xmm5, %xmm13                                 #70.5
        movaps    0x30(%rsi), %xmm5                             #70.5
        mulps     %xmm4, %xmm5                                  #70.5
        shufps    $177, %xmm4, %xmm4                            #70.5
        mulps     %xmm12, %xmm4                                 #70.5
LEAF_OE_const_4:
        movaps    0xFECA(%rdx,%rax,4), %xmm9                    #70.5
        addps     %xmm4, %xmm5                                  #70.5
LEAF_OE_const_6:
        movaps    0xFECA(%rdx,%rax,4), %xmm7                    #70.5
        movaps    %xmm9, %xmm3                                  #70.5
LEAF_OE_const_7:
        movaps    0xFECA(%rdx,%rax,4), %xmm2                    #70.5
        movaps    %xmm7, %xmm6                                  #70.5
LEAF_OE_const_5:
        movaps    0xFECA(%rdx,%rax,4), %xmm15                   #70.5
        movaps    %xmm13, %xmm4                                 #70.5
        subps     %xmm2, %xmm7                                  #70.5
        addps     %xmm15, %xmm3                                 #70.5
        subps     %xmm15, %xmm9                                 #70.5
        addps     %xmm2, %xmm6                                  #70.5
        subps     %xmm5, %xmm13                                 #70.5
        addps     %xmm5, %xmm4                                  #70.5
        xorps     %xmm0, %xmm7                                  #70.5
        addq      $4, %rax                                      #72.5
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
        movaps    %xmm10, 32(%r8,%r10,4)                       #70.5
        movaps    %xmm11, 48(%r8,%r10,4)                       #70.5
        movaps    %xmm2, (%r8,%r11,4)                          #70.5
        movaps    %xmm3, 16(%r8,%r11,4)                        #70.5
        movaps    %xmm14, 32(%r8,%r11,4)                       #70.5
        movaps    %xmm4, 48(%r8,%r11,4)                        #70.5

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
        movaps    (%rsi), %xmm3                                 #34.3
        movq      0x20(%rcx), %rdi
#ifdef __APPLE__
    .globl	_x4
_x4:
#else	
    .globl	x4
x4:
#endif
        movaps    64(%r8), %xmm0                               #34.3
        movaps    96(%r8), %xmm1                               #34.3
        movaps    (%r8), %xmm7                                 #34.3
        movaps    (%rdi), %xmm4        #const
        movaps    %xmm7, %xmm9                                 #34.3
        movaps    %xmm4, %xmm6                                 #34.3
        movaps    16(%rdi), %xmm2      #const
        mulps     %xmm0, %xmm6                                 #34.3
        mulps     %xmm1, %xmm4                                 #34.3
        shufps    $177, %xmm0, %xmm0                           #34.3
        shufps    $177, %xmm1, %xmm1                           #34.3
        mulps     %xmm2, %xmm0                                 #34.3
        mulps     %xmm1, %xmm2                                 #34.3
        subps     %xmm0, %xmm6                                 #34.3
        addps     %xmm2, %xmm4                                 #34.3
        movaps    %xmm6, %xmm5                                 #34.3
        subps     %xmm4, %xmm6                                 #34.3
        addps     %xmm4, %xmm5                                 #34.3        
        movaps    32(%r8), %xmm8                               #34.3        
        xorps     %xmm3, %xmm6                                 #34.3
        shufps    $177, %xmm6, %xmm6                           #34.3        
        movaps    %xmm8, %xmm10                                #34.3
        movaps    112(%r8), %xmm12                             #34.3        
        subps     %xmm5, %xmm9                                 #34.3
        addps     %xmm5, %xmm7                                 #34.3
        addps     %xmm6, %xmm10                                #34.3
        subps     %xmm6, %xmm8                                 #34.3        
        movaps    %xmm7, (%r8)                                 #34.3
        movaps    %xmm8, 32(%r8)                               #34.3
        movaps    %xmm9, 64(%r8)                               #34.3
        movaps    %xmm10, 96(%r8)                              #34.3        
        movaps    32(%rdi), %xmm14    #const                   #34.3
        movaps    80(%r8), %xmm11                              #34.3
        movaps    %xmm14, %xmm0                                #34.3
        movaps    48(%rdi), %xmm13    #const                   #34.3
        mulps     %xmm11, %xmm0                                #34.3
        mulps     %xmm12, %xmm14                               #34.3
        shufps    $177, %xmm11, %xmm11                         #34.3
        shufps    $177, %xmm12, %xmm12                         #34.3
        mulps     %xmm13, %xmm11                               #34.3
        mulps     %xmm12, %xmm13                               #34.3
        subps     %xmm11, %xmm0                                #34.3
        addps     %xmm13, %xmm14                               #34.3        
        movaps    %xmm0, %xmm15                                #34.3
        subps     %xmm14, %xmm0                                #34.3
        addps     %xmm14, %xmm15                               #34.3
        xorps     %xmm3, %xmm0                                 #34.3
        movaps    16(%r8), %xmm1                               #34.3
        movaps    48(%r8), %xmm2                               #34.3
        movaps    %xmm1, %xmm4                                 #34.3
        shufps    $177, %xmm0, %xmm0                           #34.3
        movaps    %xmm2, %xmm5                                 #34.3
        addps     %xmm15, %xmm1                                #34.3
        subps     %xmm0, %xmm2                                 #34.3
        subps     %xmm15, %xmm4                                #34.3
        addps     %xmm0, %xmm5                                 #34.3
        movaps    %xmm1, 16(%r8)                               #34.3
        movaps    %xmm2, 48(%r8)                               #34.3
        movaps    %xmm4, 80(%r8)                               #34.3
        movaps    %xmm5, 112(%r8)                              #34.3
        ret

# _x8_soft + 6 needs to be 16 byte aligned
#ifdef __APPLE__
    .globl	_x8_soft
_x8_soft:
#else
    .globl	x8_soft
x8_soft:
#endif
        # rax, rcx, rdx, r8, r10, r11 (r9 not used)
        # rbx, rdi, rsi
        
        # input
        movq      %rdi, %rax

        # output
        movq      %r8, %rcx

        # loop stop (output + output_stride)
        leaq      (%r8, %rbx), %rdx

        # 3 * output_stride
        leaq      (%rbx, %rbx, 2), %rsi

        # 5 * output_stride
        leaq      (%rbx, %rbx, 4), %r10

        # 7 * output_stride
        leaq      (%rsi, %rbx, 4), %r11
        
X8_soft_loop:
        # input + 0 * input_stride
        movaps    (%rax), %xmm9

        # output + 2 * output_stride
        movaps    (%rcx, %rbx, 2), %xmm6

        movaps    %xmm9, %xmm11

        # output + 3 * output_stride
        movaps    (%rcx, %rsi), %xmm7

        # input + 1 * input_stride
        movaps    16(%rax), %xmm8

        mulps     %xmm6, %xmm11
        mulps     %xmm7, %xmm9
        shufps    $177, %xmm6, %xmm6
        mulps     %xmm8, %xmm6
        shufps    $177, %xmm7, %xmm7
        subps     %xmm6, %xmm11
        mulps     %xmm7, %xmm8
        movaps    %xmm11, %xmm10
        addps     %xmm8, %xmm9

        # input + 2 * input_stride
        movaps    32(%rax), %xmm15

        addps     %xmm9, %xmm10
        subps     %xmm9, %xmm11

        # output + 0 * output_stride
        movaps    (%rcx), %xmm5

        movaps    %xmm15, %xmm6

        # output + 4 * output_stride
        movaps    (%rcx, %rbx, 4), %xmm12

        movaps    %xmm5, %xmm2

        # output + 6 * output_stride
        movaps    (%rcx, %rsi, 2), %xmm13

        xorps     %xmm3, %xmm11   #const

        # input + 3 * input_stride
        movaps    48(%rax), %xmm14

        subps     %xmm10, %xmm2
        mulps     %xmm12, %xmm6
        addps     %xmm10, %xmm5
        mulps     %xmm13, %xmm15

        # input + 4 * input_stride
        movaps    64(%rax), %xmm10

        movaps    %xmm5, %xmm0
        shufps    $177, %xmm12, %xmm12
        shufps    $177, %xmm13, %xmm13
        mulps     %xmm14, %xmm12
        mulps     %xmm13, %xmm14
        subps     %xmm12, %xmm6
        addps     %xmm14, %xmm15

        # output + 5 * output_stride
        movaps    (%rcx, %r10), %xmm7

        movaps    %xmm10, %xmm13

        # output + 7 * output_stride
        movaps    (%rcx, %r11), %xmm8

        movaps    %xmm6, %xmm12

        # input + 5 * input_stride
        movaps    80(%rax), %xmm9

        # input + 6 * input_stride
        addq      $96, %rax

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

        # output + 1 * output_stride
        movaps    (%rcx, %rbx), %xmm4

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
        xorps     %xmm3, %xmm13   #const
        addps     %xmm11, %xmm4
        subps     %xmm11, %xmm14
        shufps    $177, %xmm13, %xmm13

        # output + 0 * output_stride
        movaps    %xmm5, (%rcx)

        # output + 1 * output_stride
        movaps    %xmm4, (%rcx, %rbx)

        # output + 2 * output_stride
        movaps    %xmm2, (%rcx, %rbx, 2)

        subps     %xmm13, %xmm1
        addps     %xmm13, %xmm6

        # output + 3 * output_stride
        movaps    %xmm1, (%rcx, %rsi)

        # output + 4 * output_stride
        movaps    %xmm0, (%rcx, %rbx, 4)

        # output + 5 * output_stride
        movaps    %xmm14, (%rcx, %r10)

        # output + 6 * output_stride
        movaps    %xmm12, (%rcx, %rsi, 2)

        # output + 7 * output_stride
        movaps    %xmm6, (%rcx, %r11)

        # output + 8 * output_stride
        addq      $16, %rcx

        cmpq      %rdx, %rcx
        jne       X8_soft_loop
        ret

#ifdef __APPLE__
    .globl	_x8_soft_end
_x8_soft_end:
#else
    .globl	x8_soft_end
x8_soft_end:

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
    .long   0x00000000,0x80000000,0x00000000,0x80000000
    .long   0x3f3504f3,0x3f3504f3,0x3f3504f3,0x3f3504f3
    .long   0xbf3504f3,0x3f3504f3,0xbf3504f3,0x3f3504f3
    .long   0x3f800000,0x3f800000,0x3f3504f3,0x3f3504f3
    .long   0x00000000,0x00000000,0xbf3504f3,0x3f3504f3
#ifdef __APPLE__
    .globl _sse_constants_inv
_sse_constants_inv:
#else
    .globl sse_constants_inv
sse_constants_inv:
#endif
    .long   0x80000000,0x00000000,0x80000000,0x00000000
    .long   0x3f3504f3,0x3f3504f3,0x3f3504f3,0x3f3504f3
    .long   0x3f3504f3,0xbf3504f3,0x3f3504f3,0xbf3504f3
    .long   0x3f800000,0x3f800000,0x3f3504f3,0x3f3504f3
    .long   0x00000000,0x00000000,0x3f3504f3,0xbf3504f3
