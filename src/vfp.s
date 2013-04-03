/*
 
 This file is part of FFTS -- The Fastest Fourier Transform in the South
  
 Copyright (c) 2012, 2013 Anthony M. Blake <amb@anthonix.com>
 Copyright (c) 2012, 2013 The University of Waikato 
 
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


@ assumes r0 = out 
@         r1 = in ? 
@
@         r12 = offsets
@         r3-r10 = data pointers
@         r11 = loop iterations
@         r2 = const pointer
@       & lr = temps

	.align 4
#ifdef __APPLE__
	.globl	_vfp_e
_vfp_e:
#else
	.globl	vfp_e
vfp_e:
#endif
_vfp_e_loop:
	vldr s9, [r8] @ x5 
	vldr s18, [r4] @ x1 
	vldr	s12, [r8, #4]
	vldr	s7, [r2] 
	vldr s4, [r9] @ x6 
	vldr	s14, [r9, #4]
	vldr	s16, [r2, #8]
	vldr	s6, [r10, #4]
	vldr s0, [r10] @ x7 
	vldr s20, [r3] @ x0 
	vldr s11, [r7] @ x4 
	vldr	s5, [r7, #4]
	vsub.f32	s8, s4, s0
	vsub.f32	s1, s14, s6
	vsub.f32	s10, s5, s12
	vsub.f32	s3, s11, s9
	vldr	s30, [r5, #4]
	vldr	s26, [r6, #4]
	vadd.f32	s0, s4, s0
	vadd.f32	s4, s11, s9
	vldr s24, [r6] @ x3 
	vldr s28, [r5] @ x2 
	vadd.f32	s14, s14, s6
	vadd.f32	s5, s5, s12
	vadd.f32	s2, s20, s18
	vsub.f32	s6, s20, s18
	vmul.f32	s9, s3, s7
	vmul.f32	s11, s3, s16
	vmul.f32	s15, s8, s7
	vmul.f32	s13, s10, s16
	vmul.f32	s20, s10, s7
	vmul.f32	s22, s1, s16
	vadd.f32	s12, s30, s26
	vsub.f32	s3, s30, s26
	vmul.f32	s26, s8, s16
	vldr	s16, [r4, #4]
	vmul.f32	s30, s1, s7
	vldr	s7, [r3, #4]
    add r3, r3, #8
    add r4, r4, #8
    add r5, r5, #8
    add r6, r6, #8
    add r7, r7, #8
    add r8, r8, #8
    add r9, r9, #8
    add r10, r10, #8
	vsub.f32	s10, s9, s13
	vadd.f32	s18, s28, s24
	vadd.f32	s8, s15, s22
	vadd.f32	s22, s7, s16
	vsub.f32	s13, s28, s24
	vsub.f32	s16, s7, s16
	vadd.f32	s28, s20, s11
	vsub.f32	s26, s30, s26
	vadd.f32	s1, s4, s0
	vadd.f32	s9, s2, s18
	vsub.f32	s11, s2, s18
	vadd.f32	s15, s22, s12
	vadd.f32	s7, s5, s14
	vsub.f32	s18, s5, s14
	vsub.f32	s5, s4, s0
	vsub.f32	s22, s22, s12
	vadd.f32	s12, s10, s8
	vadd.f32	s2, s6, s3
	vsub.f32	s0, s6, s3
	vsub.f32	s8, s10, s8
	vsub.f32	s4, s16, s13
	vadd.f32	s14, s16, s13
	vadd.f32	s3, s28, s26
	vsub.f32	s6, s28, s26
	vadd.f32	s20, s9, s1
	vsub.f32	s16, s9, s1
	vadd.f32	s10, s2, s12
	vadd.f32	s24, s15, s7
	vsub.f32	s13, s15, s7
	vadd.f32	s28, s11, s18
	vsub.f32	s7, s11, s18
	vadd.f32	s9, s0, s6
ldr lr, [r12], #4
add lr, r0, lr, lsl #2
subs	r11, r11, #1
	vstr	s20, [lr]
	vsub.f32	s6, s0, s6
	vsub.f32	s26, s22, s5
	vadd.f32	s1, s22, s5
	vstr	s24, [lr, #4]
	vadd.f32	s5, s4, s3
	vsub.f32	s11, s14, s8
	vstr	s28, [lr, #16]
	vsub.f32	s2, s2, s12
	vsub.f32	s4, s4, s3
	vadd.f32	s0, s14, s8
	vstr	s26, [lr, #20]
	vstr	s16, [lr, #32]
	vstr	s13, [lr, #36]
	vstr	s7, [lr, #48]
	vstr	s1, [lr, #52]
	vstr	s10, [lr, #8]
	vstr	s5, [lr, #12]
	vstr	s9, [lr, #24]
	vstr	s11, [lr, #28]
	vstr	s2, [lr, #40]
	vstr	s4, [lr, #44]
	vstr	s6, [lr, #56]
	vstr	s0, [lr, #60]
	bne _vfp_e_loop

@ assumes r0 = out 
@         r1 = in ? 
@
@         r12 = offsets
@         r3-r10 = data pointers
@         r11 = loop iterations
@         r2 & lr = temps
	.align 4
#ifdef __APPLE__
	.globl	_vfp_o
_vfp_o:
#else
	.globl	vfp_o
vfp_o:
#endif
 _vfp_o_loop: 
		vldr                          s2, [r4]
    vldr                          s0, [r4, #4]
		vldr                          s12, [r5]
    vldr                          s4, [r6]
    vldr                          s14, [r6, #4]
		vldr                          s3, [r5, #4]
		vadd.f32                      s6, s12, s4
    vldr                          s7, [r3]
    vldr                          s5, [r3, #4]
   subs r11, r11, #1   
	 ldr r2, [r12], #4
	 add r2, r0, r2, lsl #2
    vadd.f32                      s8, s3, s14
    vadd.f32                      s10, s7, s2
    vadd.f32                      s1, s5, s0
    vsub.f32                      s14, s3, s14
    vsub.f32                      s3, s7, s2
    vsub.f32                      s2, s12, s4
    vsub.f32                      s0, s5, s0
    vadd.f32                      s9, s10, s6
    vsub.f32                      s10, s10, s6
    vadd.f32                      s7, s1, s8
    vadd.f32                      s5, s3, s14
    vsub.f32                      s4, s3, s14
    vldr                          s3, [r9, #4]
    vsub.f32                      s12, s0, s2
    vadd.f32                      s0, s0, s2
    vldr                          s2, [r8]
    vsub.f32                      s6, s1, s8
    vstr                          s9, [r2]
    vstr                          s7, [r2, #4]
    vstr                          s5, [r2, #8]
    vstr                          s12, [r2, #12]
    vstr                          s10, [r2, #16]
    vstr                          s6, [r2, #20]
    vstr                          s4, [r2, #24]
    vstr                          s0, [r2, #28]
    vldr                          s0, [r8, #4]
    vldr                          s12, [r9]
    vldr                          s4, [r10]
    vldr                          s14, [r10, #4]
    vldr                          s7, [r7]
    vadd.f32                      s6, s12, s4
    vldr                          s5, [r7, #4]
    add r3, r3, #8
    add r4, r4, #8
    add r5, r5, #8
    add r6, r6, #8
    add r7, r7, #8
    add r8, r8, #8
    add r9, r9, #8
    add r10, r10, #8
    vadd.f32                      s8, s3, s14
    vadd.f32                      s10, s7, s2
    vadd.f32                      s1, s5, s0
    vsub.f32                      s14, s3, s14
    vsub.f32                      s3, s7, s2
    vsub.f32                      s2, s12, s4
    vsub.f32                      s0, s5, s0
    vadd.f32                      s9, s10, s6
    vsub.f32                      s10, s10, s6
    vadd.f32                      s7, s1, s8
    vadd.f32                      s5, s3, s14
    vsub.f32                      s6, s1, s8
    vsub.f32                      s12, s0, s2
    vsub.f32                      s4, s3, s14
    vadd.f32                      s0, s0, s2
    vstr                          s9, [r2, #32]
    vstr                          s7, [r2, #36]
    vstr                          s5, [r2, #40]
    vstr                          s12, [r2, #44]
    vstr                          s10, [r2, #48]
    vstr                          s6, [r2, #52]
    vstr                          s4, [r2, #56]
    vstr                          s0, [r2, #60]
    bne _vfp_o_loop
	
	
	.align	4
#ifdef __APPLE__
	.globl	_vfp_x4
_vfp_x4:
#else
	.globl	vfp_x4
vfp_x4:
#endif
	add r3, r0, #0
	add r7, r2, #0
	add r4, r0, r1, lsl #1
	add r5, r0, r1, lsl #2
	add r6, r4, r1, lsl #2
	mov r11, #4	
_vfp_x4_loop:

	vldr	s8, [r3, #0]
	vldr	s9, [r3, #4]
	vldr	s10, [r4, #0]
	vldr	s11, [r4, #4]
	vldr	s12, [r5, #0]
	vldr	s13, [r5, #4]
	vldr	s14, [r6, #0]
	vldr	s15, [r6, #4]
	vldr	s2, [r7, #0]
	vldr	s3, [r7, #4]
    add r7, r7, #8	
   subs r11, r11, #1   
	vmul.f32	s0, s13, s3
	vmul.f32	s5, s12, s2
	vmul.f32	s1, s14, s2
	vmul.f32	s4, s14, s3
	vmul.f32	s14, s12, s3
	vmul.f32	s13, s13, s2
	vmul.f32	s12, s15, s3
	vmul.f32	s2, s15, s2
	vsub.f32	s0, s5, s0
	vadd.f32	s13, s13, s14
	vadd.f32	s12, s12, s1
	vsub.f32	s1, s2, s4
	vadd.f32	s15, s0, s12
	vsub.f32	s12, s0, s12
	vadd.f32	s14, s13, s1
	vsub.f32	s13, s13, s1
	vadd.f32	s0, s8, s15
	vadd.f32	s1, s9, s14
	vadd.f32	s2, s10, s13  @
	vsub.f32	s4, s8, s15
	vsub.f32	s3, s11, s12  @
	vstr	s0, [r3, #0]
	vstr	s1, [r3, #4]
	 add r3, r3, #8 
	vsub.f32	s5, s9, s14
	vsub.f32	s6, s10, s13  @
	vadd.f32	s7, s11, s12  @
	vstr	s2, [r4, #0]
	vstr	s3, [r4, #4]
	 add r4, r4, #8 
	vstr	s4, [r5, #0]
	vstr	s5, [r5, #4]
	 add r5, r5, #8 
	vstr	s6, [r6, #0]
	vstr	s7, [r6, #4]
	 add r6, r6, #8 
	bne _vfp_x4_loop	
	bx lr
	
	.align 4
#ifdef __APPLE__
	.globl	_vfp_x8
_vfp_x8:
#else
	.globl	vfp_x8
vfp_x8:
#endif
	mov r11, #0
	add r3, r0, #0           @ data0
	add r5, r0, r1, lsl #1   @ data2
	add r4, r0, r1           @ data1
	add r7, r5, r1, lsl #1   @ data4
	add r6, r5, r1           @ data3
	add r9, r7, r1, lsl #1   @ data6
	add r8, r7, r1           @ data5
	add r10, r9, r1          @ data7
	add r12, r2, #0          @ LUT

	sub r11, r11, r1, lsr #3
_vfp_x8_loop: 
	vldr	s2, [r12, #0]
	vldr	s3, [r12, #4]
	add r12, r12, #8
	vldr	s14, [r6, #0]
	vldr	s15, [r6, #4]
	vldr	s10, [r5, #0]
	vldr	s11, [r5, #4]
  adds	r11, r11, #1
  vmul.f32	s12, s15, s2
  vmul.f32	s8, s14, s3
  vmul.f32	s13, s14, s2
  vmul.f32	s9, s10, s3
  vmul.f32	s1, s10, s2
  vmul.f32	s0, s11, s2
  vmul.f32	s14, s11, s3
  vmul.f32	s15, s15, s3
	vldr	s2, [r12, #0]
	vldr	s3, [r12, #4]
	add r12, r12, #8
  vsub.f32	s10, s12, s8
  vadd.f32	s11, s0, s9
  vadd.f32	s8, s15, s13
	vldr	s12, [r4, #0]
	vldr	s13, [r4, #4]
  vsub.f32	s9, s1, s14
  vsub.f32	s15, s11, s10
  vsub.f32	s14, s9, s8
  vadd.f32	s4, s12, s15  @
  vsub.f32	s6, s12, s15  @ 
  vsub.f32	s5, s13, s14  @
  vadd.f32	s7, s13, s14  @
	vldr	s14, [r9, #0]
	vldr	s15, [r9, #4]
	vldr	s12, [r7, #0]
	vldr	s13, [r7, #4]
  vmul.f32	s1, s14, s2
  vmul.f32	s0, s14, s3
	vstr	s4, [r4, #0]
	vstr	s5, [r4, #4]
  vmul.f32	s14, s15, s3
  vmul.f32	s4, s15, s2
  vadd.f32	s15, s9, s8
	vstr	s6, [r6, #0]
	vstr	s7, [r6, #4]
  vmul.f32	s8, s12, s3
  vmul.f32	s5, s13, s3
  vmul.f32	s12, s12, s2
  vmul.f32	s9, s13, s2
  vadd.f32	s14, s14, s1
  vsub.f32	s13, s4, s0
  vadd.f32	s0, s9, s8
	vldr	s8, [r3, #0]
	vldr	s9, [r3, #4]
  vadd.f32	s1, s11, s10
  vsub.f32	s12, s12, s5
  vadd.f32	s11, s8, s15
  vsub.f32	s8, s8, s15
  vadd.f32	s2, s12, s14
  vsub.f32	s10, s0, s13
  vadd.f32	s15, s0, s13
  vadd.f32	s13, s9, s1
  vsub.f32	s9, s9, s1
  vsub.f32	s12, s12, s14
  vadd.f32	s0, s11, s2
  vadd.f32	s1, s13, s15
  vsub.f32	s4, s11, s2
  vadd.f32	s2, s8, s10  @
  vsub.f32	s3, s9, s12  @
	vstr	s0, [r3, #0]
	vstr	s1, [r3, #4]
	add r3, r3, #8
  vsub.f32	s5, s13, s15
	vldr	s14, [r10, #0]
	vldr	s15, [r10, #4]
  vadd.f32	s7, s9, s12  @
	vldr	s12, [r8, #0]
	vldr	s13, [r8, #4]
	vstr	s2, [r5, #0]
	vstr	s3, [r5, #4]
	add r5, r5, #8
	vldr	s2, [r12, #0]
	vldr	s3, [r12, #4]
	add r12, r12, #8
  vsub.f32	s6, s8, s10  @
  vmul.f32	s8, s14, s2
	vstr	s4, [r7, #0]
	vstr	s5, [r7, #4]
	add r7, r7, #8
  vmul.f32	s10, s15, s3
  vmul.f32	s9, s13, s3
  vmul.f32	s11, s12, s2
  vmul.f32	s14, s14, s3
	vstr	s6, [r9, #0]
	vstr	s7, [r9, #4]
	add r9, r9, #8
  vmul.f32	s15, s15, s2
  vmul.f32	s12, s12, s3
  vmul.f32	s13, s13, s2
  vadd.f32	s10, s10, s8
  vsub.f32	s11, s11, s9
	vldr	s8, [r4, #0]
	vldr	s9, [r4, #4]
  vsub.f32	s14, s15, s14
  vadd.f32	s15, s13, s12
  vadd.f32	s13, s11, s10
  vadd.f32	s12, s15, s14
  vsub.f32	s15, s15, s14
  vsub.f32	s14, s11, s10
	vldr	s10, [r6, #0]
	vldr	s11, [r6, #4]
  vadd.f32	s0, s8, s13
  vadd.f32	s1, s9, s12
  vadd.f32	s2, s10, s15  @
  vsub.f32	s3, s11, s14  @
  vsub.f32	s4, s8, s13
	vstr	s0, [r4, #0]
	vstr	s1, [r4, #4]
	add r4, r4, #8
  vsub.f32	s5, s9, s12
  vsub.f32	s6, s10, s15  @
	vstr	s2, [r6, #0]
	vstr	s3, [r6, #4]
	add r6, r6, #8
  vadd.f32	s7, s11, s14  @
	vstr	s4, [r8, #0]
	vstr	s5, [r8, #4]
	add r8, r8, #8
	vstr	s6, [r10, #0]
	vstr	s7, [r10, #4]
	add r10, r10, #8
	bne _vfp_x8_loop	
	bx lr
	
	.align 4
#ifdef __APPLE__
	.globl	_vfp_end
_vfp_end:
#else
	.globl	vfp_end
vfp_end:
#endif
	bx lr
