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


#ifndef __MACROS_H__
#define __MACROS_H__

#include "../config.h"
#include "types.h"

#ifdef __ARM_NEON__
	//#include "neon_float.h"
	#include "neon.h"
#include <arm_neon.h>

typedef float32x4_t V;

typedef float32x4x2_t VS;

//#include <complex.h>
//#include <stdalign.h>


#define ADD vaddq_f32
#define SUB vsubq_f32
#define MUL vmulq_f32
#define VADD vaddq_f32
#define VSUB vsubq_f32
#define VMUL vmulq_f32
#define VXOR(x,y) (vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(x), vreinterpretq_u32_f32(y))))
#define VST vst1q_f32
#define VLD vld1q_f32 
#define VST2 vst2q_f32
#define VLD2 vld2q_f32 

#define VSWAPPAIRS(x) (vrev64q_f32(x))

#define VUNPACKHI(a,b) (vcombine_f32(vget_high_f32(a), vget_high_f32(b)))
#define VUNPACKLO(a,b) (vcombine_f32(vget_low_f32(a), vget_low_f32(b)))

#define VBLEND(x,y) (vcombine_f32(vget_low_f32(x), vget_high_f32(y)))

static inline V VLIT4(data_t f3, data_t f2, data_t f1, data_t f0) {
    data_t __attribute__ ((aligned(16))) d[4] = {f0, f1, f2, f3};
    return VLD(d);
}

#define VDUPRE(r) vcombine_f32(vdup_lane_f32(vget_low_f32(r),0), vdup_lane_f32(vget_high_f32(r),0))
#define VDUPIM(r) vcombine_f32(vdup_lane_f32(vget_low_f32(r),1), vdup_lane_f32(vget_high_f32(r),1))

#define FFTS_MALLOC(d,a) (valloc(d))
#define FFTS_FREE(d) (free(d))

__INLINE void STORESPR(data_t * addr,  VS p) {

	vst1q_f32(addr, p.val[0]);
	vst1q_f32(addr + 4, p.val[1]);
	
//__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
//								 		: 
//	                	: "r" (addr), "w" (p.val[0]), "w" (p.val[1])
//                 		: "memory");
}

#else 
	#include "sse_float.h"
#endif

#include "cp_sse.h"


cdata_t SCALAR_MULI_SIGN;
V MULI_SIGN;
V LEAFLUT[12];

__INLINE V IMULI(V a) {
	return VSWAPPAIRS(VXOR(a, MULI_SIGN));
}

__INLINE void 
S_4(V r0, V r1, V r2, V r3, data_t * restrict o0, data_t * restrict o1, data_t * restrict o2, data_t * restrict o3) {
 	V t0, t1, t2, t3; 
  VST(o0, r0); VST(o1, r1); VST(o2, r2); VST(o3, r3);
}
__INLINE void S_2(V r0, V r1, data_t * restrict o0, data_t * restrict o1) {
  VST(o0, r0); VST(o1, r1);
}

__INLINE void L_S2(const data_t * restrict i0, const data_t * restrict i1, V * restrict r0, V * restrict r1) {
  V t0, t1;
  t0 = VLD(i0);    t1 = VLD(i1);    
  *r0 = VADD(t0, t1);
  *r1 = VSUB(t0, t1);
}
__INLINE void 
L_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3;
  t0 = VLD(i0);    
  t1 = VLD(i1);    
  t2 = VLD(i2);    
  t3 = VLD(i3);    
  *r0 = VADD (t0, t1);
  *r1 = VSUB (t0, t1);
  *r2 = VADD (t2, t3);
  *r3 = VSUB (t2, t3);
}

__INLINE void 
L_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
  t4 = VADD (t0, t1);
  t5 = VSUB (t0, t1);
  t6 = VADD (t2, t3);
  t7 = IMULI(VSUB (t2, t3));
  *r0 = VADD (t4, t6);
  *r2 = VSUB (t4, t6);
  *r1 = VSUB (t5, t7);
  *r3 = VADD (t5, t7);
}

__INLINE void 
K_0(V *r0, V *r1, V *r2, V *r3) {
  V uk, uk2, zk, zk_d;
	uk   = *r0; uk2  = *r1;
  zk   = VADD(*r2, *r3);
  zk_d = IMULI(VSUB(*r2, *r3));
  *r0 = VADD(uk, zk);
  *r2 = VSUB(uk, zk);
  *r1 = VSUB(uk2, zk_d);
  *r3 = VADD(uk2, zk_d);
}

__INLINE V IMUL(V d, V re, V im) {
  re = VMUL(re, d);                   
  im = VMUL(im, VSWAPPAIRS(d));
  return VSUB(re, im);  
}

__INLINE V IMULJ(V d, V re, V im) {
  re = VMUL(re, d);                   
  im = VMUL(im, VSWAPPAIRS(d));
  return VADD(re, im);  
}

__INLINE void 
K_N(V re, V im, V *r0, V *r1, V *r2, V *r3) {
	V uk, uk2, zk_p, zk_n, zk, zk_d;
	uk   = *r0; uk2  = *r1;
  zk_p = IMUL(*r2, re, im);
  zk_n = IMULJ(*r3, re, im);
	
	zk   = VADD(zk_p, zk_n);
  zk_d = IMULI(VSUB(zk_p, zk_n));
  
	*r2 = VSUB(uk, zk);
  *r0 = VADD(uk, zk);
  *r3 = VADD(uk2, zk_d);
  *r1 = VSUB(uk2, zk_d);
}

__INLINE void TX2(V *a, V *b) {
    V TX2_t0 = VUNPACKLO(*a, *b);
		V TX2_t1 = VUNPACKHI(*a, *b);
    *a = TX2_t0; *b = TX2_t1; 
}

/*
__INLINE void 
LEAF_EE(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;

	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
  K_0(&r0,&r2,&r4,&r6);
  K_N(LEAFLUT[0],LEAFLUT[1],&r1,&r3,&r5,&r7);
  L_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8,&r9,&r10,&r11);
  L_4(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12,&r13,&r14,&r15);
  K_0(&r0,&r4,&r8,&r12);
  K_N(LEAFLUT[2],LEAFLUT[3],&r1,&r5,&r9,&r13);
  TX2(&r0,&r1); TX2(&r4,&r5); TX2(&r8,&r9); TX2(&r12,&r13);
  S_4(r0,r4,r8,r12,out0+0,out0+8,out0+16,out0+24);
  S_4(r1,r5,r9,r13,out1+0,out1+8,out1+16,out1+24);
  K_N(LEAFLUT[0],LEAFLUT[1],&r2,&r6,&r10,&r14);
  K_N(LEAFLUT[4],LEAFLUT[5],&r3,&r7,&r11,&r15);
  TX2(&r2,&r3); TX2(&r6,&r7); TX2(&r10,&r11); TX2(&r14,&r15);
  S_4(r2,r6,r10,r14,out0+4,out0+12,out0+20,out0+28);
  S_4(r3,r7,r11,r15,out1+4,out1+12,out1+20,out1+28);
	*is += 16;
}


__INLINE void 
LEAF_OO(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;
	
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
  K_0(&r0,&r2,&r4,&r6);
  K_N(LEAFLUT[0],LEAFLUT[1],&r1,&r3,&r5,&r7);
  TX2(&r0,&r1); TX2(&r2,&r3); TX2(&r4,&r5); TX2(&r6,&r7);
  S_4(r0,r2,r4,r6,out0+0,out0+4,out0+8,out0+12);
  S_4(r1,r3,r5,r7,out1+0,out1+4,out1+8,out1+12);
  L_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8,&r9,&r10,&r11);
  L_2(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12,&r13,&r14,&r15);
  K_0(&r8,&r10,&r12,&r14);
  K_N(LEAFLUT[0],LEAFLUT[1],&r9,&r11,&r13,&r15);
  TX2(&r8,&r9); TX2(&r10,&r11); TX2(&r12,&r13); TX2(&r14,&r15);
  S_4(r8,r10,r12,r14,out0+16,out0+20,out0+24,out0+28);
  S_4(r9,r11,r13,r15,out1+16,out1+20,out1+24,out1+28);

	*is += 16;
}
*/
#ifdef __ARM_NEON__
__INLINE void 
S_4_1(V r0, V r1, V r2, V r3, data_t * restrict o0, data_t * restrict o1, data_t * restrict o2, data_t * restrict o3) {
	register V p0 __asm__ ("q0") = r0; register V p1 __asm__ ("q1") = r1; register V p2 __asm__ ("q2") = r2; register V p3 __asm__ ("q3") = r3;
	__asm__ __volatile__ ("vst4.32 {%q1,%q2}, [%0, :128]!\n\t"
												"vst4.32 {%q3,%q4}, [%0, :128]!\n\t"
									 		: 
		                	: "r" (o0), "w" (p0), "w" (p1), "w" (p2), "w" (p3)
	                 		: "memory");
}
__INLINE void 
S_4_2(V r0, V r1, V r2, V r3, data_t * restrict o0, data_t * restrict o1, data_t * restrict o2, data_t * restrict o3) {
	register V p0 __asm__ ("q4") = r0; register V p1 __asm__ ("q5") = r1; register V p2 __asm__ ("q6") = r2; register V p3 __asm__ ("q7") = r3;
	__asm__ __volatile__ ("vst4.32 {%q1,%q2}, [%0, :128]!\n\t"
												"vst4.32 {%q3,%q4}, [%0, :128]!\n\t"
									 		: 
		                	: "r" (o0), "w" (p0), "w" (p1), "w" (p2), "w" (p3)
	                 		: "memory");
}
/*
__INLINE void 
LEAF_EE8(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0,r1,r2,r3,r4,r5,r6,r7;
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
  K_0(&r0,&r2,&r4,&r6);
  K_N(LEAFLUT[0], LEAFLUT[1],&r1,&r3,&r5,&r7);
	
	register V p0 __asm__ ("q0") = r0; 
	register V p1 __asm__ ("q1") = r2; 
	register V p2 __asm__ ("q2") = r4;
	register V p3 __asm__ ("q3") = r6;
	register V p4 __asm__ ("q4") = r1; 
	register V p5 __asm__ ("q5") = r3; 
	register V p6 __asm__ ("q6") = r5;
	register V p7 __asm__ ("q7") = r7;
  
	__asm__ __volatile__ ("vswp %f1,%e6\n\t"
												"vswp %f2,%e7\n\t"
												"vswp %f3,%e8\n\t"
												"vswp %f4,%e9\n\t"
												"vst4.32 {%q1,%q2}, [%0, :128]!\n\t"
												"vst4.32 {%q3,%q4}, [%0, :128]!\n\t"
												"vst4.32 {%q6,%q7}, [%5, :128]!\n\t"
												"vst4.32 {%q8,%q9}, [%5, :128]!\n\t"
									 		: 
		                	: "r" (out0), "w" (p0), "w" (p1), "w" (p2), "w" (p3), 
											  "r" (out1), "w" (p4), "w" (p5), "w" (p6), "w" (p7)
	                 		: "memory");
//TX2(&r0,&r1); TX2(&r2,&r3); TX2(&r4,&r5); TX2(&r6,&r7);
//S_4_1(r0,r2,r4,r6,out0+0,out0+4,out0+8,out0+12);
//S_4_2(r1,r3,r5,r7,out1+0,out1+4,out1+8,out1+12);
	*is += 8;
}
__INLINE void 
LEAF_OO8(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
	V r0,r1,r2,r3,r4,r5,r6,r7;
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_4(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
	register V p0 __asm__ ("q0") = r0; 
	register V p1 __asm__ ("q1") = r2; 
	register V p2 __asm__ ("q2") = r4;
	register V p3 __asm__ ("q3") = r6;
	register V p4 __asm__ ("q4") = r1; 
	register V p5 __asm__ ("q5") = r3; 
	register V p6 __asm__ ("q6") = r5;
	register V p7 __asm__ ("q7") = r7;
	__asm__ __volatile__ ("vswp %f1,%e6\n\t"
												"vswp %f2,%e7\n\t"
												"vswp %f3,%e8\n\t"
												"vswp %f4,%e9\n\t"
												"vst4.32 {%q1,%q2}, [%0, :128]!\n\t"
												"vst4.32 {%q3,%q4}, [%0, :128]!\n\t"
												"vst4.32 {%q6,%q7}, [%5, :128]!\n\t"
												"vst4.32 {%q8,%q9}, [%5, :128]!\n\t"
									 		: 
		                	: "r" (out0), "w" (p0), "w" (p1), "w" (p2), "w" (p3), 
											  "r" (out1), "w" (p4), "w" (p5), "w" (p6), "w" (p7)
	                 		: "memory");
//TX2(&r0,&r1); TX2(&r2,&r3); TX2(&r4,&r5); TX2(&r6,&r7);
//S_4_1(r0,r2,r4,r6,out0+0,out0+4,out0+8,out0+12);
//S_4_2(r1,r3,r5,r7,out1+0,out1+4,out1+8,out1+12);
	*is += 8;
}
*/
#endif
__INLINE void 
L_4_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
	t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
	t4 = VADD(t0, t1);
	t5 = VSUB(t0, t1);
	t6 = VADD(t2, t3);
	t7 = IMULI(VSUB(t2, t3));
	t0 = VADD(t4, t6);
	t2 = VSUB(t4, t6);
	t1 = VSUB(t5, t7);
	t3 = VADD(t5, t7);
	TX2(&t0,&t1);
	TX2(&t2,&t3);
	*r0 = t0; *r2 = t1; *r1 = t2; *r3 = t3; }

__INLINE void 
L_2_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
	t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3); t4 = VADD(t0, t1);
	t5 = VSUB(t0, t1);
	t6 = VADD(t2, t3);
	t7 = VSUB(t2, t3);
	TX2(&t4,&t5);
	TX2(&t6,&t7);
	*r0 = t4; *r2 = t5; *r1 = t6; *r3 = t7;
}

__INLINE void 
L_2_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
  t4 = VADD(t0, t1);
  t5 = VSUB(t0, t1);
  t6 = VADD(t2, t3);
  t7 = VSUB(t2, t3);
  *r0 = VUNPACKLO(t4, t5);
  *r1 = VUNPACKLO(t6, t7);
  t5 = IMULI(t5);
  t0 = VADD(t6, t4);
  t2 = VSUB(t6, t4);
  t1 = VSUB(t7, t5);
  t3 = VADD(t7, t5);
  *r3 = VUNPACKHI(t0, t1);
  *r2 = VUNPACKHI(t2, t3);
}

__INLINE void 
L_4_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = VLD(i0);    t1 = VLD(i1);    t6 = VLD(i2);    t7 = VLD(i3);    
	t2 = VBLEND(t6, t7);
  t3 = VBLEND(t7, t6);
	t4 = VADD(t0, t1);
  t5 = VSUB(t0, t1);
  t6 = VADD(t2, t3);
  t7 = VSUB(t2, t3);
  *r2 = VUNPACKHI(t4, t5);
  *r3 = VUNPACKHI(t6, t7);
 	t7 = IMULI(t7);
  t0 = VADD(t4, t6);
  t2 = VSUB(t4, t6);
  t1 = VSUB(t5, t7);
  t3 = VADD(t5, t7);
  *r0 = VUNPACKLO(t0, t1);
  *r1 = VUNPACKLO(t2, t3);
}
/*
__INLINE void 
LEAF_OE(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;

	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;
  
	L_4_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r24_25,&r26_27);
  L_2_4(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r30_31,&r28_29);
  K_N(LEAFLUT[6],LEAFLUT[7],&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,out0+0,out0+4,out0+8,out0+12);
  L_4_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8_9,&r10_11,&r16_17,&r18_19);
  L_2_2(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12_13,&r14_15,&r20_21,&r22_23);
  K_N(LEAFLUT[6],LEAFLUT[7],&r8_9,&r10_11,&r12_13,&r14_15);
  S_4(r8_9,r10_11,r12_13,r14_15,out0+16,out0+20,out0+24,out0+28);
  K_N(LEAFLUT[6],LEAFLUT[7],&r16_17,&r18_19,&r20_21,&r22_23);
  K_N(LEAFLUT[8],LEAFLUT[9],&r16_17,&r20_21,&r24_25,&r28_29);
  S_4(r16_17,r20_21,r24_25,r28_29,out1+0,out1+8,out1+16,out1+24);
  K_N(LEAFLUT[10],LEAFLUT[11],&r18_19,&r22_23,&r26_27,&r30_31);
  S_4(r18_19,r22_23,r26_27,r30_31,out1+4,out1+12,out1+20,out1+28);

	*is += 16;
}

__INLINE void 
LEAF_EO(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;
	
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r16_17,&r18_19);
  L_2_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r20_21,&r22_23);
  K_N(LEAFLUT[6],LEAFLUT[7],&r0_1,&r2_3,&r4_5,&r6_7);
  L_4_2(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8_9,&r10_11,&r28_29,&r30_31);
  L_4_4(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12_13,&r14_15,&r24_25,&r26_27);
  K_N(LEAFLUT[8],LEAFLUT[9],&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,out0+0,out0+8,out0+16,out0+24);
  K_N(LEAFLUT[10],LEAFLUT[11],&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,out0+4,out0+12,out0+20,out0+28);
  K_N(LEAFLUT[6],LEAFLUT[7],&r16_17,&r18_19,&r20_21,&r22_23);
  S_4(r16_17,r18_19,r20_21,r22_23,out1+0,out1+4,out1+8,out1+12);
  K_N(LEAFLUT[6],LEAFLUT[7],&r24_25,&r26_27,&r28_29,&r30_31);
  S_4(r24_25,r26_27,r28_29,r30_31,out1+16,out1+20,out1+24,out1+28);

	*is += 16;
}
*/
#ifdef __ARM_NEON__
/*
__INLINE void 
LEAF_OE8(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4_2(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r12_13,&r14_15);
  L_4_4(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r8_9,&r10_11);
  S_4_1(r0_1,r2_3,r4_5,r6_7,out0+0,out0+4,out0+8,out0+12);
  K_N(LEAFLUT[6],LEAFLUT[7],&r8_9,&r10_11,&r12_13,&r14_15);
  S_4_2(r8_9,r10_11,r12_13,r14_15,out1+0,out1+4,out1+8,out1+12);
	*is += 8;
}
__INLINE void 
LEAF_EO8(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r8_9,&r10_11);
  L_2_4(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r14_15,&r12_13);
  S_4_1(r8_9,r10_11,r12_13,r14_15,out1+0,out1+4,out1+8,out1+12);
  K_N(LEAFLUT[6],LEAFLUT[7],&r0_1,&r2_3,&r4_5,&r6_7);
  S_4_2(r0_1,r2_3,r4_5,r6_7,out0+0,out0+4,out0+8,out0+12);

	*is += 8;
}*/
#endif
__INLINE void 
firstpass_32(const data_t * restrict in, data_t * restrict out, ffts_plan_t * restrict p) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;
  float *LUT8 = p->ws + p->ws_is[0];
  float *LUT16 = p->ws + p->ws_is[1];
  float *LUT32 = p->ws + p->ws_is[2];

  L_4_4(in+0,in+32,in+16,in+48,&r0_1,&r2_3,&r16_17,&r18_19);
  L_2_2(in+8,in+40,in+56,in+24,&r4_5,&r6_7,&r20_21,&r22_23);
  K_N(VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  L_4_2(in+4,in+36,in+20,in+52,&r8_9,&r10_11,&r28_29,&r30_31);
  L_4_4(in+60,in+28,in+12,in+44,&r12_13,&r14_15,&r24_25,&r26_27);
  K_N(VLD(LUT16),VLD(LUT16+4),&r0_1,&r4_5,&r8_9,&r12_13);
  K_N(VLD(LUT16+8),VLD(LUT16+12),&r2_3,&r6_7,&r10_11,&r14_15);
  K_N(VLD(LUT8),VLD(LUT8+4),&r16_17,&r18_19,&r20_21,&r22_23);
  K_N(VLD(LUT8),VLD(LUT8+4),&r24_25,&r26_27,&r28_29,&r30_31);
  K_N(VLD(LUT32),VLD(LUT32+4),&r0_1,&r8_9,&r16_17,&r24_25);
  S_4(r0_1,r8_9,r16_17,r24_25,out+0,out+16,out+32,out+48);
  K_N(VLD(LUT32+8),VLD(LUT32+12),&r2_3,&r10_11,&r18_19,&r26_27);
  S_4(r2_3,r10_11,r18_19,r26_27,out+4,out+20,out+36,out+52);
  K_N(VLD(LUT32+16),VLD(LUT32+20),&r4_5,&r12_13,&r20_21,&r28_29);
  S_4(r4_5,r12_13,r20_21,r28_29,out+8,out+24,out+40,out+56);
  K_N(VLD(LUT32+24),VLD(LUT32+28),&r6_7,&r14_15,&r22_23,&r30_31);
  S_4(r6_7,r14_15,r22_23,r30_31,out+12,out+28,out+44,out+60);

}

__INLINE void 
firstpass_16(ffts_plan_t * restrict p, const data_t * restrict in, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
  float *LUT8 = p->ws ;//+ p->ws_is[0];
//  float *LUT16 = ((float *)p->ws) + 8;//(p->ws_is[1]*4);

  L_4_4(in+0,in+16,in+8,in+24,&r0_1,&r2_3,&r8_9,&r10_11);
  L_2_4(in+4,in+20,in+28,in+12,&r4_5,&r6_7,&r14_15,&r12_13);
  K_N(VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(VLD(LUT8+8),VLD(LUT8+12),&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,out+0,out+8,out+16,out+24);
  K_N(VLD(LUT8+16),VLD(LUT8+20),&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,out+4,out+12,out+20,out+28);
}

__INLINE void 
firstpass_8(ffts_plan_t * restrict p, const data_t * restrict in, data_t * restrict out) {
  V r0_1,r2_3,r4_5,r6_7;
  float *LUT8 = p->ws + p->ws_is[0];
	L_4_2(in+0,in+8,in+4,in+12,&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,out+0,out+4,out+8,out+12);
}

__INLINE void 
firstpass_4_f(ffts_plan_t * restrict p, const data_t * restrict in, data_t * restrict out) {
  cdata_t *i = (cdata_t *)in, *o = (cdata_t *)out;
	cdata_t t0, t1, t2, t3, t4, t5, t6, t7;
	t0[0] = in[0]; t0[1] = in[1];
	t1[0] = in[4]; t1[1] = in[5];
	t2[0] = in[2]; t2[1] = in[3];
	t3[0] = in[6]; t3[1] = in[7];
	
	t4[0] = t0[0] + t1[0]; t4[1] = t0[1] + t1[1];
	t5[0] = t0[0] - t1[0]; t5[1] = t0[1] - t1[1];
	t6[0] = t2[0] + t3[0]; t6[1] = t2[1] + t3[1];
	t7[0] = t2[0] - t3[0]; t7[1] = t2[1] - t3[1];

	out[0] = t4[0] + t6[0]; out[1] = t4[1] + t6[1];
	out[4] = t4[0] - t6[0]; out[5] = t4[1] - t6[1];
	out[2] = t5[0] + t7[1]; out[3] = t5[1] - t7[0];
	out[6] = t5[0] - t7[1]; out[7] = t5[1] + t7[0];
/*  t0 = i[0]; t1 = i[2]; t2 = i[1]; t3 = i[3];
  t4 = CADD(t0,t1);
  t5 = CSUB(t0,t1);
  t6 = CADD(t2,t3);
  t7 = CSUB(t2,t3);
	t7 = (creal(t7))*I - (cimag(t7));
	o[0] = t4 + t6;
  o[2] = t4 - t6;
  o[1] = t5 - t7;
  o[3] = t5 + t7;
*/
}
__INLINE void 
firstpass_4_b(ffts_plan_t * restrict p, const data_t * restrict in, data_t * restrict out) {
  cdata_t *i = (cdata_t *)in, *o = (cdata_t *)out;
	cdata_t t0, t1, t2, t3, t4, t5, t6, t7;
	t0[0] = in[0]; t0[1] = in[1];
	t1[0] = in[4]; t1[1] = in[5];
	t2[0] = in[2]; t2[1] = in[3];
	t3[0] = in[6]; t3[1] = in[7];
	
	t4[0] = t0[0] + t1[0]; t4[1] = t0[1] + t1[1];
	t5[0] = t0[0] - t1[0]; t5[1] = t0[1] - t1[1];
	t6[0] = t2[0] + t3[0]; t6[1] = t2[1] + t3[1];
	t7[0] = t2[0] - t3[0]; t7[1] = t2[1] - t3[1];

	out[0] = t4[0] + t6[0]; out[1] = t4[1] + t6[1];
	out[4] = t4[0] - t6[0]; out[5] = t4[1] - t6[1];
	out[2] = t5[0] - t7[1]; out[3] = t5[1] + t7[0];
	out[6] = t5[0] + t7[1]; out[7] = t5[1] - t7[0];
/*	cdata_t t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = i[0]; t1 = i[2]; t2 = i[1]; t3 = i[3];
  t4 = t0 + t1;
  t5 = t0 - t1;
  t6 = t2 + t3;
  t7 = (t2 - t3);
	t7 = -(creal(t7))*I + (cimag(t7));
	o[0] = t4 + t6;
  o[2] = t4 - t6;
  o[1] = t5 - t7;
  o[3] = t5 + t7;
*/
}
__INLINE void 
firstpass_2(ffts_plan_t * restrict p, const data_t * restrict in, data_t * restrict out) {
  cdata_t t0, t1, r0,r1;
	t0[0] = in[0]; t0[1] = in[1];
	t1[0] = in[2]; t1[1] = in[3];
	r0[0] = t0[0] + t1[0];
	r0[1] = t0[1] + t1[1];
	r1[0] = t0[0] - t1[0];
	r1[1] = t0[1] - t1[1];
	out[0] = r0[0]; out[1] = r0[1];
	out[2] = r1[0]; out[3] = r1[1];

//  t0 = ((cdata_t *)in)[0]; t1 = ((cdata_t *)in)[1];
//  r0 = t0 + t1; r1 = t0 - t1;
//	((cdata_t *)out)[0] = r0;
//	((cdata_t *)out)[1] = r1;
}
/*
__INLINE void X_8(data_t * restrict data0, size_t N, const data_t * restrict LUT) {
	data_t *data2 = data0 + 2*N/4;
	data_t *data4 = data0 + 4*N/4;
	data_t *data6 = data0 + 6*N/4;
	data_t *data1 = data0 + 1*N/4;
	data_t *data3 = data0 + 3*N/4;
	data_t *data5 = data0 + 5*N/4;
	data_t *data7 = data0 + 7*N/4;
	size_t k, n4 = N/4;

	for(k=N/8/2;k>0;--k) {
      V r0, r1, r2, r3, r4, r5, r6, r7;
			r0 = VLD(data0);
			r1 = VLD(data1);
			r2 = VLD(data2);
			r3 = VLD(data3);
			K_N(VLD(LUT), VLD(LUT+4), &r0, &r1, &r2, &r3);
			r4 = VLD(data4);
			r6 = VLD(data6);
			K_N(VLD(LUT+8), VLD(LUT+12), &r0, &r2, &r4, &r6);
			r5 = VLD(data5);
			r7 = VLD(data7);
			K_N(VLD(LUT+16), VLD(LUT+20), &r1, &r3, &r5, &r7);
			LUT += 24;
			VST(data0, r0); data0 += 4;
			VST(data1, r1); data1 += 4;
			VST(data2, r2); data2 += 4;
			VST(data3, r3); data3 += 4;
			VST(data4, r4); data4 += 4;
			VST(data5, r5); data5 += 4;
			VST(data6, r6); data6 += 4;
			VST(data7, r7); data7 += 4;
	}
}

__INLINE void X_4(data_t * restrict data, size_t N, const data_t * restrict LUT) {

	size_t i;
	for(i=0;i<N/4/2;i++) {
		V uk = VLD(data);
		V uk2 = VLD(data + 2*N/4);
		V zk_p = VLD(data + 4*N/4);
		V zk_n = VLD(data + 6*N/4);

		K_N(VLD(LUT), VLD(LUT+4), &uk, &uk2, &zk_p, &zk_n);

		VST(data, uk); 
		VST(data + 2*N/4, uk2); 
		VST(data + 4*N/4, zk_p);
		VST(data + 6*N/4, zk_n);

		LUT += 8;
		data += 4;
	}
}
*/
#endif
