
	.globl	_neon_x4
	.align	2
_neon_x4:
	push {r4,r5,r6}
	add r3, r0, #0
	add r4, r3, r1, lsl #1
	add r5, r3, r1, lsl #2
	add r6, r4, r1, lsl #2


	vld1.32 {q2,q3}, [r2, :128]
	vld1.32 {q8,q9}, [r3, :128]
	vld1.32 {q10,q11}, [r4, :128]
	vld1.32 {q12,q13}, [r5, :128]
	vld1.32 {q14,q15}, [r6, :128]
	
	vmul.f32	q1, q12, q2
	vmul.f32	q0, q14, q2
	vmul.f32	q4, q15, q2
	vmul.f32	q12, q12, q3
	vmls.f32 q1,q13,q3
	vmla.f32 q0,q15,q3
	vmls.f32 q4,q14,q3
	vadd.f32	q15, q1, q0
	vmla.f32 q12,q13,q2
	vsub.f32	q13, q12, q4
	vadd.f32	q14, q12, q4
	vsub.f32	q12, q1, q0
	vadd.f32	q0, q8, q15
	vadd.f32	q1, q9, q14
	vadd.f32	q2, q10, q13
	vsub.f32	q4, q8, q15
	vsub.f32	q3, q11, q12
	
	vst1.32 {q0,q1}, [r3, :128]
	vsub.f32	q5, q9, q14
	vsub.f32	q6, q10, q13
	vadd.f32	q7, q11, q12
	vst1.32 {q2,q3}, [r4, :128]
	vst1.32 {q4,q5}, [r5, :128]
	vst1.32 {q6,q7}, [r6, :128]
	pop {r4,r5,r6}
	bx lr

	.globl _neon_x8
	.align 2
_neon_x8:
	push {r4,r5,r6,r7,r8,r9,r10,r11}
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
	vld1.32 {q10,q11}, [r5, :128]
	vld1.32 {q12,q13}, [r4, :128]
	vld1.32 {q14,q15}, [r6, :128]
	adds	r11, r11, #1
	vld1.32 {q2,q3}, [r12, :128]!
	vmul.f32	q1, q10, q3
	vmul.f32	q9, q15, q2
	vmul.f32	q0, q14, q2
	vmul.f32	q10, q10, q2
	vmla.f32 q1,q11,q2
	vmls.f32 q9,q14,q3
	vmla.f32 q0,q15,q3
	vsub.f32	q14, q1, q9
	vmls.f32 q10,q11,q3
	vsub.f32	q11, q10, q0
	vadd.f32	q10, q10, q0
	vadd.f32	q4, q12, q14
	vsub.f32	q5, q13, q11
	vsub.f32	q6, q12, q14
	vadd.f32	q7, q13, q11
	vst1.32 {q4,q5}, [r4, :128]
	vadd.f32	q9, q1, q9
	vld1.32 {q2,q3}, [r3, :128]
	vst1.32 {q6,q7}, [r6, :128]
	vld1.32 {q12,q13}, [r7, :128]
	vld1.32 {q14,q15}, [r9, :128]
	vmov q1, q3
	vmov q8, q2
	vld1.32 {q2,q3}, [r12, :128]!
	vmul.f32	q0, q12, q2
	vmul.f32	q11, q14, q2
	vmul.f32	q4, q15, q2
	vmul.f32	q12, q12, q3
	vmls.f32 q0,q13,q3
	vmla.f32 q11,q15,q3
	vadd.f32	q15, q8, q10
	vmls.f32 q4,q14,q3
	vmla.f32 q12,q13,q2
	vadd.f32	q14, q0, q11
	vsub.f32	q8, q8, q10
	vsub.f32	q10, q12, q4
	vadd.f32	q13, q12, q4
	vadd.f32	q12, q1, q9
	vsub.f32	q9, q1, q9
	vsub.f32	q11, q0, q11
	vadd.f32	q0, q15, q14
	vadd.f32	q1, q12, q13
	vsub.f32	q4, q15, q14
	vadd.f32	q2, q8, q10
	vsub.f32	q5, q12, q13
	vst1.32 {q0,q1}, [r3, :128]!
	vsub.f32	q3, q9, q11
	vsub.f32	q6, q8, q10
	vadd.f32	q7, q9, q11
	vst1.32 {q2,q3}, [r5, :128]!
	vst1.32 {q4,q5}, [r7, :128]!
	vst1.32 {q6,q7}, [r9, :128]!
	vld1.32 {q8,q9}, [r4, :128]
	vld1.32 {q10,q11}, [r6, :128]
	vld1.32 {q12,q13}, [r8, :128]
	vld1.32 {q14,q15}, [r10, :128]
	vld1.32 {q2,q3}, [r12, :128]!
	vmul.f32	q4, q14, q2
	vmul.f32	q5, q12, q2
	vmul.f32	q6, q15, q2
	vmul.f32	q7, q12, q3
	vmla.f32 q4,q15,q3
	vmls.f32 q5,q13,q3
	vmls.f32 q6,q14,q3
	vadd.f32	q12, q5, q4
	vmla.f32 q7,q13,q2
	vsub.f32	q14, q5, q4
	vadd.f32	q13, q7, q6
	vsub.f32	q15, q7, q6
	vadd.f32	q0, q8, q12
	vadd.f32	q1, q9, q13
	vadd.f32	q2, q10, q15
	vsub.f32	q3, q11, q14
	vsub.f32	q4, q8, q12
	vst1.32 {q0,q1}, [r4, :128]!
	vsub.f32	q5, q9, q13
	vsub.f32	q6, q10, q15
	vst1.32 {q2,q3}, [r6, :128]!
	vadd.f32	q7, q11, q14
	vst1.32 {q4,q5}, [r8, :128]!
	vst1.32 {q6,q7}, [r10, :128]!
	bne neon_x8_loop	

	pop {r4,r5,r6,r7,r8,r9,r10,r11}
	bx lr

	.globl _neon_x8_t
	.align 2
_neon_x8_t:
	push {r4,r5,r6,r7,r8,r9,r10,r11}

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
	vld1.32 {q10,q11}, [r5, :128]
	vld1.32 {q12,q13}, [r4, :128]
	adds	r11, r11, #1
	vld1.32 {q14,q15}, [r6, :128]
	vld1.32 {q2,q3}, [r12, :128]!
	vmul.f32	q1, q10, q3
	vmul.f32	q9, q15, q2
	vmul.f32	q0, q14, q2
	vmul.f32	q10, q10, q2
	vmla.f32 q1,q11,q2
	vmls.f32 q9,q14,q3
	vmla.f32 q0,q15,q3
	vsub.f32	q14, q1, q9
	vmls.f32 q10,q11,q3
	vsub.f32	q11, q10, q0
	vadd.f32	q10, q10, q0
	vadd.f32	q4, q12, q14
	vsub.f32	q5, q13, q11
	vsub.f32	q6, q12, q14
	vadd.f32	q7, q13, q11
	vst1.32 {q4,q5}, [r4, :128]
	vadd.f32	q9, q1, q9
	vld1.32 {q2,q3}, [r3, :128]
	vst1.32 {q6,q7}, [r6, :128]
	vld1.32 {q12,q13}, [r7, :128]
	vld1.32 {q14,q15}, [r9, :128]
	vmov q1, q3
	vmov q8, q2
	vld1.32 {q2,q3}, [r12, :128]!
	@vld1.64	{d2, d3}, [sp]    @ 16-byte Reload
	vmul.f32	q0, q12, q2
	vmul.f32	q11, q14, q2
	vmul.f32	q4, q15, q2
	vmul.f32	q12, q12, q3
	vmls.f32 q0,q13,q3
	vmla.f32 q11,q15,q3
	vadd.f32	q15, q8, q10
	vmls.f32 q4,q14,q3
	vmla.f32 q12,q13,q2
	vadd.f32	q14, q0, q11
	vsub.f32	q8, q8, q10
	vsub.f32	q10, q12, q4
	vadd.f32	q13, q12, q4
	vadd.f32	q12, q1, q9
	vsub.f32	q9, q1, q9
	vsub.f32	q11, q0, q11
	vadd.f32	q0, q15, q14
	vadd.f32	q1, q12, q13
	vsub.f32	q4, q15, q14
	vadd.f32	q2, q8, q10
	vsub.f32	q5, q12, q13
	vst2.32 {q0,q1}, [r3, :128]!
	vsub.f32	q3, q9, q11
	vsub.f32	q6, q8, q10
	vadd.f32	q7, q9, q11
	vst2.32 {q2,q3}, [r5, :128]!
	vst2.32 {q4,q5}, [r7, :128]!
	vst2.32 {q6,q7}, [r9, :128]!
	vld1.32 {q8,q9}, [r4, :128]
	vld1.32 {q10,q11}, [r6, :128]
	vld1.32 {q12,q13}, [r8, :128]
	vld1.32 {q14,q15}, [r10, :128]
	vld1.32 {q2,q3}, [r12, :128]!
	vmul.f32	q4, q14, q2
	vmul.f32	q5, q12, q2
	vmul.f32	q6, q15, q2
	vmul.f32	q7, q12, q3
	vmla.f32 q4,q15,q3
	vmls.f32 q5,q13,q3
	vmls.f32 q6,q14,q3
	vadd.f32	q12, q5, q4
	vmla.f32 q7,q13,q2
	vsub.f32	q14, q5, q4
	vadd.f32	q13, q7, q6
	vsub.f32	q15, q7, q6
	vadd.f32	q0, q8, q12
	vadd.f32	q1, q9, q13
	vadd.f32	q2, q10, q15
	vsub.f32	q3, q11, q14
	vsub.f32	q4, q8, q12
	vst2.32 {q0,q1}, [r4, :128]!
	vsub.f32	q5, q9, q13
	vsub.f32	q6, q10, q15
	vst2.32 {q2,q3}, [r6, :128]!
	vadd.f32	q7, q11, q14
	vst2.32 {q4,q5}, [r8, :128]!
	vst2.32 {q6,q7}, [r10, :128]!
	bne neon_x8_t_loop	

	pop {r4,r5,r6,r7,r8,r9,r10,r11}
	bx lr
	
	.globl	_neon_end
	.align	2
_neon_end:
	bx lr
