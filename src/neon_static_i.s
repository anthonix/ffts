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
	.fpu	neon

	.align 4
#ifdef __APPLE__
	.globl	_neon_static_e_i
_neon_static_e_i:
#else
	.globl	neon_static_e_i
neon_static_e_i:
#endif
  push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  vstmdb	sp!, {d8-d15}
  ldr lr, [r0, #40]  @ this is p->N
  add	r3, r1, #0	
  add	r7, r1, lr 
  add	r5, r7, lr 
  add	r10, r5, lr
  add	r4, r10, lr	
  add	r8, r4, lr
  add	r6, r8, lr
  add	r9, r6, lr
  ldr	r12, [r0]
  add	r1, r0, #0
  add	r0, r2, #0	
  ldr	r2, [r1, #16]   @ this is p->ee_ws
  ldr	r11, [r1, #28]  @ this is p->i0	
	
	vld1.32	{d16, d17}, [r2, :128]
_neon_ee_loop:
	vld2.32 {q15}, [r10, :128]!
	vld2.32 {q13}, [r8, :128]!
	vld2.32 {q14}, [r7, :128]!
	vld2.32 {q9},  [r4, :128]!
	vld2.32 {q10}, [r3, :128]!
	vld2.32 {q11}, [r6, :128]!
	vld2.32 {q12}, [r5, :128]!
	vsub.f32	q1, q14, q13
	vld2.32 {q0}, [r9, :128]!
	subs	r11, r11, #1
	vsub.f32	q2, q0, q15
	vadd.f32	q0, q0, q15
	vmul.f32	d10, d2, d17
	vmul.f32	d11, d3, d16
	vmul.f32	d12, d3, d17
	vmul.f32	d6, d4, d17  
	vmul.f32	d7, d5, d16  
	vmul.f32	d8, d4, d16  
	vmul.f32	d9, d5, d17  
	vmul.f32	d13, d2, d16
	vsub.f32	d7, d7, d6
	vadd.f32	d11, d11, d10
	vsub.f32	q1, q12, q11
	vsub.f32	q2, q10, q9
	vadd.f32	d6, d9, d8
	vadd.f32	q4, q14, q13
	vadd.f32	q11, q12, q11
	vadd.f32	q12, q10, q9
	vsub.f32	d10, d13, d12
	vsub.f32	q7, q4, q0
	vsub.f32	q9, q12, q11
	vsub.f32	q13, q5, q3
	vadd.f32	d29, d5, d2  @
	vadd.f32	q5, q5, q3
	vadd.f32	q10, q4, q0
	vadd.f32	q11, q12, q11
	vsub.f32	d31, d5, d2  @
	vsub.f32	d28, d4, d3  @
	vadd.f32	d30, d4, d3  @
	vadd.f32	d5, d19, d14  @
	vadd.f32	d7, d31, d26  @
	vadd.f32	q1, q14, q5
	vadd.f32	q0, q11, q10
	vsub.f32	d6, d30, d27  @
	vsub.f32	d4, d18, d15  @
	vsub.f32	d13, d19, d14  @
	vadd.f32	d12, d18, d15  @
	vsub.f32	d15, d31, d26  @
	ldr r2, [r12], #4
	vtrn.32	q1, q3
	ldr lr, [r12], #4
	vtrn.32	q0, q2
	add r2, r0, r2, lsl #2
	vsub.f32	q4, q11, q10
	add lr, r0, lr, lsl #2
	vsub.f32	q5, q14, q5
	vadd.f32	d14, d30, d27 @
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_ee_loop

	ldr	r11, [r1, #12]
	vld2.32 {q9}, [r5, :128]! @tag2
	vld2.32 {q13}, [r3, :128]! @tag0
	vld2.32 {q12}, [r4, :128]! @tag1
	vld2.32 {q0}, [r7, :128]! @tag4
	vsub.f32	q11, q13, q12
	vld2.32 {q8}, [r6, :128]! @tag3
	vadd.f32	q12, q13, q12
	vsub.f32	q10, q9, q8
	vadd.f32	q8, q9, q8
	vadd.f32	q9, q12, q8
	vadd.f32	d9, d23, d20  @
	vsub.f32	d11, d23, d20  @
	vsub.f32	q8, q12, q8
	vsub.f32	d8, d22, d21  @
	vadd.f32	d10, d22, d21  @
	ldr r2, [r12], #4
	vld1.32	{d20, d21}, [r11, :128]
	ldr lr, [r12], #4
	vtrn.32	q9, q4
	add r2, r0, r2, lsl #2
	vtrn.32	q8, q5
	add lr, r0, lr, lsl #2
	vswp d9,d10
	vst1.32 {d8,d9,d10,d11}, [lr, :128]!
	vld2.32 {q13}, [r10, :128]! @tag7
	vld2.32 {q15}, [r9, :128]! @tag6
	vld2.32 {q11}, [r8, :128]! @tag5
	vsub.f32	q14, q15, q13
	vsub.f32	q12, q0, q11
	vadd.f32	q11, q0, q11
	vadd.f32	q13, q15, q13
	vadd.f32	d13, d29, d24  @
	vadd.f32	q15, q13, q11
	vsub.f32	d12, d28, d25  @
	vsub.f32	d15, d29, d24  @
	vadd.f32	d14, d28, d25  @
	vtrn.32	q15, q6
	vsub.f32	q15, q13, q11
	vtrn.32	q15, q7
	vswp d13, d14
	vst1.32 {d12,d13,d14,d15}, [lr, :128]!
	vtrn.32	q13, q14
	vtrn.32	q11, q12
	vmul.f32	d24, d26, d21
	vmul.f32	d28, d27, d20
	vmul.f32	d25, d26, d20
	vmul.f32	d26, d27, d21
	vmul.f32	d27, d22, d21
	vmul.f32	d30, d23, d20
	vmul.f32	d29, d23, d21
	vmul.f32	d22, d22, d20
	vsub.f32	d21, d28, d24
	vadd.f32	d20, d26, d25
	vadd.f32	d25, d30, d27
	vsub.f32	d24, d22, d29
	vadd.f32	q11, q12, q10
	vsub.f32	q10, q12, q10
	vadd.f32	q0, q9, q11
	vsub.f32	q2, q9, q11
	vadd.f32	d3, d17, d20  @
	vsub.f32	d7, d17, d20  @
	vsub.f32	d2, d16, d21  @
	vadd.f32	d6, d16, d21  @
	vswp d1, d2
	vswp d5, d6
	vstmia r2!, {q0-q3}

  add	r2, r7, #0	
  add	r7, r9, #0	
  add	r9, r2, #0	
  add	r2, r8, #0	
  add	r8, r10, #0	
  add	r10, r2, #0	
  ldr	r11, [r1, #32]  @ this is p->i1	
	cmp r11, #0
	beq _neon_oo_loop_exit
_neon_oo_loop:
	vld2.32 {q8}, [r6, :128]!
	vld2.32 {q9}, [r5, :128]!
	vld2.32 {q10}, [r4, :128]!
	vld2.32 {q13}, [r3, :128]!
	vadd.f32	q11, q9, q8
	vsub.f32	q8, q9, q8
	vsub.f32	q9, q13, q10
	vadd.f32	q12, q13, q10
	subs	r11, r11, #1
	vld2.32 {q10}, [r7, :128]!
	vld2.32 {q13}, [r9, :128]!
	vsub.f32	q2, q12, q11
	vsub.f32	d7, d19, d16  @
	vadd.f32	d3, d19, d16  @
	vadd.f32	d6, d18, d17  @
	vsub.f32	d2, d18, d17  @
	vld2.32 {q9}, [r8, :128]!
	vld2.32 {q8}, [r10, :128]!
	vadd.f32	q0, q12, q11
	vadd.f32	q11, q13, q8
	vadd.f32	q12, q10, q9
	vsub.f32	q8, q13, q8
	vsub.f32	q9, q10, q9
	vsub.f32	q6, q12, q11
	vadd.f32	q4, q12, q11
	vtrn.32	q0, q2
	ldr r2, [r12], #4
	vsub.f32	d15, d19, d16  @
	ldr lr, [r12], #4
	vadd.f32	d11, d19, d16  @
	vadd.f32	d14, d18, d17  @
	vsub.f32	d10, d18, d17  @
	add r2, r0, r2, lsl #2
	vtrn.32	q1, q3
	add lr, r0, lr, lsl #2
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_oo_loop
_neon_oo_loop_exit:

  add	r2, r3, #0
  add	r3, r7, #0
  add	r7, r2, #0
  add	r2, r4, #0
  add	r4, r8, #0
  add	r8, r2, #0
  add	r2, r5, #0
  add	r5, r9, #0
  add	r9, r2, #0
  add	r2, r6, #0
  add	r6, r10, #0
  add	r10, r2, #0
  add	r2, r9, #0	
  add	r9, r10, #0	
  add	r10, r2, #0	
  ldr	r2, [r1, #16]
  ldr	r11, [r1, #32]  @ this is p->i1	
	cmp r11, #0
	beq _neon_ee_loop2_exit

	vld1.32	{d16, d17}, [r2, :128]
_neon_ee_loop2:
	vld2.32 {q15}, [r10, :128]!
	vld2.32 {q13}, [r8, :128]!
	vld2.32 {q14}, [r7, :128]!
	vld2.32 {q9},  [r4, :128]!
	vld2.32 {q10}, [r3, :128]!
	vld2.32 {q11}, [r6, :128]!
	vld2.32 {q12}, [r5, :128]!
	vsub.f32	q1, q14, q13
	vld2.32 {q0}, [r9, :128]!
	subs	r11, r11, #1
	vsub.f32	q2, q0, q15
	vadd.f32	q0, q0, q15
	vmul.f32	d10, d2, d17
	vmul.f32	d11, d3, d16
	vmul.f32	d12, d3, d17
	vmul.f32	d6, d4, d17  
	vmul.f32	d7, d5, d16  
	vmul.f32	d8, d4, d16  
	vmul.f32	d9, d5, d17  
	vmul.f32	d13, d2, d16
	vsub.f32	d7, d7, d6
	vadd.f32	d11, d11, d10
	vsub.f32	q1, q12, q11
	vsub.f32	q2, q10, q9
	vadd.f32	d6, d9, d8
	vadd.f32	q4, q14, q13
	vadd.f32	q11, q12, q11
	vadd.f32	q12, q10, q9
	vsub.f32	d10, d13, d12
	vsub.f32	q7, q4, q0
	vsub.f32	q9, q12, q11
	vsub.f32	q13, q5, q3
	vadd.f32	d29, d5, d2  @
	vadd.f32	q5, q5, q3
	vadd.f32	q10, q4, q0
	vadd.f32	q11, q12, q11
	vsub.f32	d31, d5, d2  @
	vsub.f32	d28, d4, d3  @
	vadd.f32	d30, d4, d3  @
	vadd.f32	d5, d19, d14  @
	vadd.f32	d7, d31, d26  @
	vadd.f32	q1, q14, q5
	vadd.f32	q0, q11, q10
	vsub.f32	d6, d30, d27  @
	vsub.f32	d4, d18, d15  @
	vsub.f32	d13, d19, d14  @
	vadd.f32	d12, d18, d15  @
	vsub.f32	d15, d31, d26  @
	ldr r2, [r12], #4
	vtrn.32	q1, q3
	ldr lr, [r12], #4
	vtrn.32	q0, q2
	add r2, r0, r2, lsl #2
	vsub.f32	q4, q11, q10
	add lr, r0, lr, lsl #2
	vsub.f32	q5, q14, q5
	vadd.f32	d14, d30, d27 @
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_ee_loop2
_neon_ee_loop2_exit:

	vldmia	sp!, {d8-d15}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}




	.align 4
#ifdef __APPLE__
	.globl	_neon_static_o_i
_neon_static_o_i:
#else
	.globl	neon_static_o_i
neon_static_o_i:
#endif
  push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  vstmdb	sp!, {d8-d15}
  ldr lr, [r0, #40]  @ this is p->N
  add	r3, r1, #0	
  add	r7, r1, lr 
  add	r5, r7, lr 
  add	r10, r5, lr
  add	r4, r10, lr	
  add	r8, r4, lr
  add	r6, r8, lr
  add	r9, r6, lr
  ldr	r12, [r0]
  add	r1, r0, #0
  add	r0, r2, #0	
  ldr	r2, [r1, #16]   @ this is p->ee_ws
  ldr	r11, [r1, #28]  @ this is p->i0	
	
	vld1.32	{d16, d17}, [r2, :128]
_neon_ee_o_loop:
	vld2.32 {q15}, [r10, :128]!
	vld2.32 {q13}, [r8, :128]!
	vld2.32 {q14}, [r7, :128]!
	vld2.32 {q9},  [r4, :128]!
	vld2.32 {q10}, [r3, :128]!
	vld2.32 {q11}, [r6, :128]!
	vld2.32 {q12}, [r5, :128]!
	vsub.f32	q1, q14, q13
	vld2.32 {q0}, [r9, :128]!
	subs	r11, r11, #1
	vsub.f32	q2, q0, q15
	vadd.f32	q0, q0, q15
	vmul.f32	d10, d2, d17
	vmul.f32	d11, d3, d16
	vmul.f32	d12, d3, d17
	vmul.f32	d6, d4, d17  
	vmul.f32	d7, d5, d16  
	vmul.f32	d8, d4, d16  
	vmul.f32	d9, d5, d17  
	vmul.f32	d13, d2, d16
	vsub.f32	d7, d7, d6
	vadd.f32	d11, d11, d10
	vsub.f32	q1, q12, q11
	vsub.f32	q2, q10, q9
	vadd.f32	d6, d9, d8
	vadd.f32	q4, q14, q13
	vadd.f32	q11, q12, q11
	vadd.f32	q12, q10, q9
	vsub.f32	d10, d13, d12
	vsub.f32	q7, q4, q0
	vsub.f32	q9, q12, q11
	vsub.f32	q13, q5, q3
	vadd.f32	d29, d5, d2  @
	vadd.f32	q5, q5, q3
	vadd.f32	q10, q4, q0
	vadd.f32	q11, q12, q11
	vsub.f32	d31, d5, d2  @
	vsub.f32	d28, d4, d3  @
	vadd.f32	d30, d4, d3  @
	vadd.f32	d5, d19, d14  @
	vadd.f32	d7, d31, d26  @
	vadd.f32	q1, q14, q5
	vadd.f32	q0, q11, q10
	vsub.f32	d6, d30, d27  @
	vsub.f32	d4, d18, d15  @
	vsub.f32	d13, d19, d14  @
	vadd.f32	d12, d18, d15  @
	vsub.f32	d15, d31, d26  @
	ldr r2, [r12], #4
	vtrn.32	q1, q3
	ldr lr, [r12], #4
	vtrn.32	q0, q2
	add r2, r0, r2, lsl #2
	vsub.f32	q4, q11, q10
	add lr, r0, lr, lsl #2
	vsub.f32	q5, q14, q5
	vadd.f32	d14, d30, d27 @
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_ee_o_loop

  add	r2, r7, #0	
  add	r7, r9, #0	
  add	r9, r2, #0	
  add	r2, r8, #0	
  add	r8, r10, #0	
  add	r10, r2, #0	
  ldr	r11, [r1, #32]  @ this is p->i1	
	cmp r11, #0
	beq _neon_oo_o_loop_exit
_neon_oo_o_loop:
	vld2.32 {q8}, [r6, :128]!
	vld2.32 {q9}, [r5, :128]!
	vld2.32 {q10}, [r4, :128]!
	vld2.32 {q13}, [r3, :128]!
	vadd.f32	q11, q9, q8
	vsub.f32	q8, q9, q8
	vsub.f32	q9, q13, q10
	vadd.f32	q12, q13, q10
	subs	r11, r11, #1
	vld2.32 {q10}, [r7, :128]!
	vld2.32 {q13}, [r9, :128]!
	vsub.f32	q2, q12, q11
	vsub.f32	d7, d19, d16  @
	vadd.f32	d3, d19, d16  @
	vadd.f32	d6, d18, d17  @
	vsub.f32	d2, d18, d17  @
	vld2.32 {q9}, [r8, :128]!
	vld2.32 {q8}, [r10, :128]!
	vadd.f32	q0, q12, q11
	vadd.f32	q11, q13, q8
	vadd.f32	q12, q10, q9
	vsub.f32	q8, q13, q8
	vsub.f32	q9, q10, q9
	vsub.f32	q6, q12, q11
	vadd.f32	q4, q12, q11
	vtrn.32	q0, q2
	ldr r2, [r12], #4
	vsub.f32	d15, d19, d16  @
	ldr lr, [r12], #4
	vadd.f32	d11, d19, d16  @
	vadd.f32	d14, d18, d17  @
	vsub.f32	d10, d18, d17  @
	add r2, r0, r2, lsl #2
	vtrn.32	q1, q3
	add lr, r0, lr, lsl #2
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_oo_o_loop
_neon_oo_o_loop_exit:

	ldr	r11, [r1, #8]
	vld1.32 {q8}, [r5, :128]!
	vld1.32 {q10}, [r6, :128]!
	vld2.32 {q11}, [r4, :128]!
	vld2.32 {q13}, [r3, :128]!
	vld2.32 {q15}, [r10, :128]!
	vorr	d25, d17, d17
	vorr	d24, d20, d20
	vorr	d20, d16, d16
	vsub.f32	q9, q13, q11
	vadd.f32	q11, q13, q11
	ldr r2, [r12], #4
	vtrn.32	d24, d25
	ldr lr, [r12], #4
	vtrn.32	d20, d21
	add r2, r0, r2, lsl #2
	vsub.f32	q8, q10, q12
	add lr, r0, lr, lsl #2
	vadd.f32	q10, q10, q12
	vadd.f32	q0, q11, q10
	vadd.f32	d25, d19, d16  @
	vsub.f32	d27, d19, d16  @
	vsub.f32	q1, q11, q10
	vsub.f32	d24, d18, d17  @
	vadd.f32	d26, d18, d17  @
	vtrn.32	q0, q12
	vtrn.32	q1, q13
	vld1.32	{d24, d25}, [r11, :128]
	vswp d1, d2
	vst1.32 {q0, q1}, [r2, :128]!
	vld2.32 {q0}, [r9, :128]!
	vadd.f32	q1, q0, q15
	vld2.32 {q13}, [r8, :128]!
	vld2.32 {q14}, [r7, :128]!
	vsub.f32	q15, q0, q15
	vsub.f32	q0, q14, q13
	vadd.f32	q3, q14, q13
	vadd.f32	q2, q3, q1
	vadd.f32	d29, d1, d30  @
	vsub.f32	d27, d1, d30  @
	vsub.f32	q3, q3, q1
	vsub.f32	d28, d0, d31  @
	vadd.f32	d26, d0, d31  @
	vtrn.32	q2, q14
	vtrn.32	q3, q13
	vswp d5, d6
	vst1.32 {q2, q3}, [r2, :128]!
	vtrn.32	q11, q9
	vtrn.32	q10, q8
	vmul.f32	d20, d18, d25
	vmul.f32	d22, d19, d24
	vmul.f32	d21, d19, d25
	vmul.f32	d18, d18, d24
	vmul.f32	d19, d16, d25
	vmul.f32	d30, d17, d24
	vmul.f32	d23, d16, d24
	vmul.f32	d24, d17, d25
	vadd.f32	d17, d22, d20
	vsub.f32	d16, d18, d21
	vsub.f32	d21, d30, d19
	vadd.f32	d20, d24, d23
	vadd.f32	q9, q8, q10
	vsub.f32	q8, q8, q10
	vadd.f32	q4, q14, q9
	vsub.f32	q6, q14, q9
	vadd.f32	d11, d27, d16  @
	vsub.f32	d15, d27, d16  @
	vsub.f32	d10, d26, d17  @
	vadd.f32	d14, d26, d17  @
	vswp d9, d10
	vswp d13, d14
	vstmia lr!, {q4-q7}


  add	r2, r3, #0
  add	r3, r7, #0
  add	r7, r2, #0
  add	r2, r4, #0
  add	r4, r8, #0
  add	r8, r2, #0
  add	r2, r5, #0
  add	r5, r9, #0
  add	r9, r2, #0
  add	r2, r6, #0
  add	r6, r10, #0
  add	r10, r2, #0
  add	r2, r9, #0	
  add	r9, r10, #0	
  add	r10, r2, #0	
  ldr	r2, [r1, #16]
  ldr	r11, [r1, #32]  @ this is p->i1	
	cmp r11, #0
	beq _neon_ee_o_loop2_exit

	vld1.32	{d16, d17}, [r2, :128]
_neon_ee_o_loop2:
	vld2.32 {q15}, [r10, :128]!
	vld2.32 {q13}, [r8, :128]!
	vld2.32 {q14}, [r7, :128]!
	vld2.32 {q9},  [r4, :128]!
	vld2.32 {q10}, [r3, :128]!
	vld2.32 {q11}, [r6, :128]!
	vld2.32 {q12}, [r5, :128]!
	vsub.f32	q1, q14, q13
	vld2.32 {q0}, [r9, :128]!
	subs	r11, r11, #1
	vsub.f32	q2, q0, q15
	vadd.f32	q0, q0, q15
	vmul.f32	d10, d2, d17
	vmul.f32	d11, d3, d16
	vmul.f32	d12, d3, d17
	vmul.f32	d6, d4, d17  
	vmul.f32	d7, d5, d16  
	vmul.f32	d8, d4, d16  
	vmul.f32	d9, d5, d17  
	vmul.f32	d13, d2, d16
	vsub.f32	d7, d7, d6
	vadd.f32	d11, d11, d10
	vsub.f32	q1, q12, q11
	vsub.f32	q2, q10, q9
	vadd.f32	d6, d9, d8
	vadd.f32	q4, q14, q13
	vadd.f32	q11, q12, q11
	vadd.f32	q12, q10, q9
	vsub.f32	d10, d13, d12
	vsub.f32	q7, q4, q0
	vsub.f32	q9, q12, q11
	vsub.f32	q13, q5, q3
	vadd.f32	d29, d5, d2  @
	vadd.f32	q5, q5, q3
	vadd.f32	q10, q4, q0
	vadd.f32	q11, q12, q11
	vsub.f32	d31, d5, d2  @
	vsub.f32	d28, d4, d3  @
	vadd.f32	d30, d4, d3  @
	vadd.f32	d5, d19, d14  @
	vadd.f32	d7, d31, d26  @
	vadd.f32	q1, q14, q5
	vadd.f32	q0, q11, q10
	vsub.f32	d6, d30, d27  @
	vsub.f32	d4, d18, d15  @
	vsub.f32	d13, d19, d14  @
	vadd.f32	d12, d18, d15  @
	vsub.f32	d15, d31, d26  @
	ldr r2, [r12], #4
	vtrn.32	q1, q3
	ldr lr, [r12], #4
	vtrn.32	q0, q2
	add r2, r0, r2, lsl #2
	vsub.f32	q4, q11, q10
	add lr, r0, lr, lsl #2
	vsub.f32	q5, q14, q5
	vadd.f32	d14, d30, d27 @
	vst2.32 {q0,q1}, [r2, :128]!
	vst2.32 {q2,q3}, [lr, :128]!
	vtrn.32	q4, q6
	vtrn.32	q5, q7
	vst2.32 {q4,q5}, [r2, :128]!
	vst2.32 {q6,q7}, [lr, :128]!
	bne _neon_ee_o_loop2
_neon_ee_o_loop2_exit:

	vldmia	sp!, {d8-d15}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

  .align 4
#ifdef __APPLE__
  .globl _neon_static_x4_i
_neon_static_x4_i:
#else
  .globl neon_static_x4_i
neon_static_x4_i:
#endif
  add       r3, r0, #64
  vpush     {q4-q7}

  vld1.32   {q2,  q3},  [r1, :128]
  vld1.32   {q12, q13}, [r3, :128]!
  mov       r2, r0
  vmul.f32  q0,  q13, q3
  vld1.32   {q14, q15}, [r3, :128]
  vmul.f32  q5,  q12, q2
  vld1.32   {q8,  q9},  [r0, :128]!
  vmul.f32  q1,  q14, q2
  vld1.32   {q10, q11}, [r0, :128]
  vmul.f32  q4,  q14, q3
  vmul.f32  q14, q12, q3
  vmul.f32  q13, q13, q2
  vmul.f32  q12, q15, q3
  vmul.f32  q2,  q15, q2
  vsub.f32  q0,  q5,  q0
  vadd.f32  q13, q13, q14
  vadd.f32  q12, q12, q1
  vsub.f32  q1,  q2,  q4
  vadd.f32  q15, q0,  q12
  vsub.f32  q12, q0,  q12
  vadd.f32  q14, q13, q1
  vsub.f32  q13, q13, q1
  vadd.f32  q0,  q8,  q15
  vadd.f32  q1,  q9,  q14
  vsub.f32  q2,  q10, q13
  vsub.f32  q4,  q8,  q15
  vadd.f32  q3,  q11, q12

  vst1.32   {q0, q1}, [r2, :128]!

  vsub.f32  q5,  q9,  q14
  vadd.f32  q6,  q10, q13
  vsub.f32  q7,  q11, q12

  vst1.32   {q2, q3}, [r2, :128]!
  vst1.32   {q4, q5}, [r2, :128]!
  vst1.32   {q6, q7}, [r2, :128]

  vpop      {q4-q7}
  bx        lr

	.align 4
#ifdef __APPLE__
	.globl	_neon_static_x8_i
_neon_static_x8_i:
#else
	.globl	neon_static_x8_i
neon_static_x8_i:
#endif
  push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  vstmdb	sp!, {d8-d15}
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

	sub r11, r11, r1, lsr #5
neon_x8_loop: 
  vld1.32 {q2,q3}, [r12, :128]!
  vld1.32 {q14,q15}, [r6, :128]
  vld1.32 {q10,q11}, [r5, :128]
  adds	r11, r11, #1
  vmul.f32	q12, q15, q2
  vmul.f32	q8, q14, q3
  vmul.f32	q13, q14, q2
  vmul.f32	q9, q10, q3
  vmul.f32	q1, q10, q2
  vmul.f32	q0, q11, q2
  vmul.f32	q14, q11, q3
  vmul.f32	q15, q15, q3
  vld1.32 {q2,q3}, [r12, :128]!
  vsub.f32	q10, q12, q8
  vadd.f32	q11, q0, q9
  vadd.f32	q8, q15, q13
  vld1.32 {q12,q13}, [r4, :128]
  vsub.f32	q9, q1, q14
  vsub.f32	q15, q11, q10
  vsub.f32	q14, q9, q8
  vsub.f32	q4, q12, q15  @
  vadd.f32	q6, q12, q15  @ 
  vadd.f32	q5, q13, q14  @
  vsub.f32	q7, q13, q14  @
  vld1.32 {q14,q15}, [r9, :128]
  vld1.32 {q12,q13}, [r7, :128]
  vmul.f32	q1, q14, q2
  vmul.f32	q0, q14, q3
  vst1.32 {q4,q5}, [r4, :128]
  vmul.f32	q14, q15, q3
  vmul.f32	q4, q15, q2
  vadd.f32	q15, q9, q8
  vst1.32 {q6,q7}, [r6, :128]
  vmul.f32	q8, q12, q3
  vmul.f32	q5, q13, q3
  vmul.f32	q12, q12, q2
  vmul.f32	q9, q13, q2
  vadd.f32	q14, q14, q1
  vsub.f32	q13, q4, q0
  vadd.f32	q0, q9, q8
  vld1.32 {q8,q9}, [r3, :128]
  vadd.f32	q1, q11, q10
  vsub.f32	q12, q12, q5
  vadd.f32	q11, q8, q15
  vsub.f32	q8, q8, q15
  vadd.f32	q2, q12, q14
  vsub.f32	q10, q0, q13
  vadd.f32	q15, q0, q13
  vadd.f32	q13, q9, q1
  vsub.f32	q9, q9, q1
  vsub.f32	q12, q12, q14
  vadd.f32	q0, q11, q2
  vadd.f32	q1, q13, q15
  vsub.f32	q4, q11, q2
  vsub.f32	q2, q8, q10  @
  vadd.f32	q3, q9, q12  @
  vst1.32 {q0,q1}, [r3, :128]!
  vsub.f32	q5, q13, q15
  vld1.32 {q14,q15}, [r10, :128]
  vsub.f32	q7, q9, q12  @
  vld1.32 {q12,q13}, [r8, :128]
  vst1.32 {q2,q3}, [r5, :128]!
  vld1.32 {q2,q3}, [r12, :128]!
  vadd.f32	q6, q8, q10  @
  vmul.f32	q8, q14, q2
  vst1.32 {q4,q5}, [r7, :128]!
  vmul.f32	q10, q15, q3
  vmul.f32	q9, q13, q3
  vmul.f32	q11, q12, q2
  vmul.f32	q14, q14, q3
  vst1.32 {q6,q7}, [r9, :128]!
  vmul.f32	q15, q15, q2
  vmul.f32	q12, q12, q3
  vmul.f32	q13, q13, q2
  vadd.f32	q10, q10, q8
  vsub.f32	q11, q11, q9
  vld1.32 {q8,q9}, [r4, :128]
  vsub.f32	q14, q15, q14
  vadd.f32	q15, q13, q12
  vadd.f32	q13, q11, q10
  vadd.f32	q12, q15, q14
  vsub.f32	q15, q15, q14
  vsub.f32	q14, q11, q10
  vld1.32 {q10,q11}, [r6, :128]
  vadd.f32	q0, q8, q13
  vadd.f32	q1, q9, q12
  vsub.f32	q2, q10, q15  @
  vadd.f32	q3, q11, q14  @
  vsub.f32	q4, q8, q13
  vst1.32 {q0,q1}, [r4, :128]!
  vsub.f32	q5, q9, q12
  vadd.f32	q6, q10, q15  @
  vst1.32 {q2,q3}, [r6, :128]!
  vsub.f32	q7, q11, q14  @
  vst1.32 {q4,q5}, [r8, :128]!
  vst1.32 {q6,q7}, [r10, :128]!
	bne neon_x8_loop	

	vldmia	sp!, {d8-d15}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

	.align 4
#ifdef __APPLE__
	.globl	_neon_static_x8_t_i
_neon_static_x8_t_i:
#else
	.globl	neon_static_x8_t_i
neon_static_x8_t_i:
#endif
  push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
  vstmdb	sp!, {d8-d15}
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

	sub r11, r11, r1, lsr #5
neon_x8_t_loop: 
  vld1.32 {q2,q3}, [r12, :128]!
  vld1.32 {q14,q15}, [r6, :128]
  vld1.32 {q10,q11}, [r5, :128]
  adds	r11, r11, #1
  vmul.f32	q12, q15, q2
  vmul.f32	q8, q14, q3
  vmul.f32	q13, q14, q2
  vmul.f32	q9, q10, q3
  vmul.f32	q1, q10, q2
  vmul.f32	q0, q11, q2
  vmul.f32	q14, q11, q3
  vmul.f32	q15, q15, q3
  vld1.32 {q2,q3}, [r12, :128]!
  vsub.f32	q10, q12, q8
  vadd.f32	q11, q0, q9
  vadd.f32	q8, q15, q13
  vld1.32 {q12,q13}, [r4, :128]
  vsub.f32	q9, q1, q14
  vsub.f32	q15, q11, q10
  vsub.f32	q14, q9, q8
  vsub.f32	q4, q12, q15  @
  vadd.f32	q6, q12, q15  @
  vadd.f32	q5, q13, q14  @
  vsub.f32	q7, q13, q14  @
  vld1.32 {q14,q15}, [r9, :128]
  vld1.32 {q12,q13}, [r7, :128]
  vmul.f32	q1, q14, q2
  vmul.f32	q0, q14, q3
  vst1.32 {q4,q5}, [r4, :128]
  vmul.f32	q14, q15, q3
  vmul.f32	q4, q15, q2
  vadd.f32	q15, q9, q8
  vst1.32 {q6,q7}, [r6, :128]
  vmul.f32	q8, q12, q3
  vmul.f32	q5, q13, q3
  vmul.f32	q12, q12, q2
  vmul.f32	q9, q13, q2
  vadd.f32	q14, q14, q1
  vsub.f32	q13, q4, q0
  vadd.f32	q0, q9, q8
  vld1.32 {q8,q9}, [r3, :128]
  vadd.f32	q1, q11, q10
  vsub.f32	q12, q12, q5
  vadd.f32	q11, q8, q15
  vsub.f32	q8, q8, q15
  vadd.f32	q2, q12, q14
  vsub.f32	q10, q0, q13
  vadd.f32	q15, q0, q13
  vadd.f32	q13, q9, q1
  vsub.f32	q9, q9, q1
  vsub.f32	q12, q12, q14
  vadd.f32	q0, q11, q2
  vadd.f32	q1, q13, q15
  vsub.f32	q4, q11, q2
  vsub.f32	q2, q8, q10  @
  vadd.f32	q3, q9, q12  @
  vst2.32 {q0,q1}, [r3, :128]!
  vsub.f32	q5, q13, q15
  vld1.32 {q14,q15}, [r10, :128]
  vsub.f32	q7, q9, q12  @
  vld1.32 {q12,q13}, [r8, :128]
  vst2.32 {q2,q3}, [r5, :128]!
  vld1.32 {q2,q3}, [r12, :128]!
  vadd.f32	q6, q8, q10  @
  vmul.f32	q8, q14, q2
  vst2.32 {q4,q5}, [r7, :128]!
  vmul.f32	q10, q15, q3
  vmul.f32	q9, q13, q3
  vmul.f32	q11, q12, q2
  vmul.f32	q14, q14, q3
  vst2.32 {q6,q7}, [r9, :128]!
  vmul.f32	q15, q15, q2
  vmul.f32	q12, q12, q3
  vmul.f32	q13, q13, q2
  vadd.f32	q10, q10, q8
  vsub.f32	q11, q11, q9
  vld1.32 {q8,q9}, [r4, :128]
  vsub.f32	q14, q15, q14
  vadd.f32	q15, q13, q12
  vadd.f32	q13, q11, q10
  vadd.f32	q12, q15, q14
  vsub.f32	q15, q15, q14
  vsub.f32	q14, q11, q10
  vld1.32 {q10,q11}, [r6, :128]
  vadd.f32	q0, q8, q13
  vadd.f32	q1, q9, q12
  vsub.f32	q2, q10, q15  @
  vadd.f32	q3, q11, q14  @
  vsub.f32	q4, q8, q13
  vst2.32 {q0,q1}, [r4, :128]!
  vsub.f32	q5, q9, q12
  vadd.f32	q6, q10, q15  @
  vst2.32 {q2,q3}, [r6, :128]!
  vsub.f32	q7, q11, q14  @
  vst2.32 {q4,q5}, [r8, :128]!
  vst2.32 {q6,q7}, [r10, :128]!
	bne neon_x8_t_loop	
	
	vldmia	sp!, {d8-d15}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}


