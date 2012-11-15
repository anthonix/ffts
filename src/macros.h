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

__INLINE V VLIT4(data_t f3, data_t f2, data_t f1, data_t f0) {
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

#include "ffts.h"


//cdata_t SCALAR_MULI_SIGN;
//V MULI_SIGN;
//V LEAFLUT[12];

__INLINE V IMULI(int inv, V a) {
	if(inv) return VSWAPPAIRS(VXOR(a, VLIT4(0.0f, -0.0f, 0.0f, -0.0f)));
	else    return VSWAPPAIRS(VXOR(a, VLIT4(-0.0f, 0.0f, -0.0f, 0.0f)));
}

__INLINE void 
S_4(V r0, V r1, V r2, V r3, data_t * restrict o0, data_t * restrict o1, data_t * restrict o2, data_t * restrict o3) {
 	V t0, t1, t2, t3; 
  VST(o0, r0); VST(o1, r1); VST(o2, r2); VST(o3, r3);
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
K_N(int inv, V re, V im, V *r0, V *r1, V *r2, V *r3) {
	V uk, uk2, zk_p, zk_n, zk, zk_d;
	uk   = *r0; uk2  = *r1;
  zk_p = IMUL(*r2, re, im);
  zk_n = IMULJ(*r3, re, im);
	
	zk   = VADD(zk_p, zk_n);
  zk_d = IMULI(inv, VSUB(zk_p, zk_n));
  
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

__INLINE void 
L_4_4(int inv, const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
	t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
	t4 = VADD(t0, t1);
	t5 = VSUB(t0, t1);
	t6 = VADD(t2, t3);
	t7 = IMULI(inv, VSUB(t2, t3));
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
L_2_4(int inv, const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  V *r0, V *r1, V *r2, V *r3) {
  V t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
  t4 = VADD(t0, t1);
  t5 = VSUB(t0, t1);
  t6 = VADD(t2, t3);
  t7 = VSUB(t2, t3);
  *r0 = VUNPACKLO(t4, t5);
  *r1 = VUNPACKLO(t6, t7);
  t5 = IMULI(inv, t5);
  t0 = VADD(t6, t4);
  t2 = VSUB(t6, t4);
  t1 = VSUB(t7, t5);
  t3 = VADD(t7, t5);
  *r3 = VUNPACKHI(t0, t1);
  *r2 = VUNPACKHI(t2, t3);
}

__INLINE void 
L_4_2(int inv, const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
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
 	t7 = IMULI(inv, t7);
  t0 = VADD(t4, t6);
  t2 = VSUB(t4, t6);
  t1 = VSUB(t5, t7);
  t3 = VADD(t5, t7);
  *r0 = VUNPACKLO(t0, t1);
  *r1 = VUNPACKLO(t2, t3);
}

__INLINE void 
firstpass_16_f(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
  float *LUT8 = p->ws ;

  L_4_4(0, din+0,din+16,din+8,din+24,&r0_1,&r2_3,&r8_9,&r10_11);
  L_2_4(0, din+4,din+20,din+28,din+12,&r4_5,&r6_7,&r14_15,&r12_13);
  K_N(0, VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(0, VLD(LUT8+8),VLD(LUT8+12),&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,dout+0,dout+8,dout+16,dout+24);
  K_N(0, VLD(LUT8+16),VLD(LUT8+20),&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,dout+4,dout+12,dout+20,dout+28);
}

__INLINE void 
firstpass_16_b(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
  V r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
  float *LUT8 = p->ws ;

  L_4_4(1, din+0,din+16,din+8,din+24,&r0_1,&r2_3,&r8_9,&r10_11);
  L_2_4(1, din+4,din+20,din+28,din+12,&r4_5,&r6_7,&r14_15,&r12_13);
  K_N(1, VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(1, VLD(LUT8+8),VLD(LUT8+12),&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,dout+0,dout+8,dout+16,dout+24);
  K_N(1, VLD(LUT8+16),VLD(LUT8+20),&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,dout+4,dout+12,dout+20,dout+28);
}

__INLINE void 
firstpass_8_f(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
  V r0_1,r2_3,r4_5,r6_7;
  float *LUT8 = p->ws + p->ws_is[0];
	L_4_2(0, din+0,din+8,din+4,din+12,&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(0, VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,dout+0,dout+4,dout+8,dout+12);
}

__INLINE void 
firstpass_8_b(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
  V r0_1,r2_3,r4_5,r6_7;
  float *LUT8 = p->ws + p->ws_is[0];
	L_4_2(1, din+0,din+8,din+4,din+12,&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(1, VLD(LUT8),VLD(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,dout+0,dout+4,dout+8,dout+12);
}

__INLINE void 
firstpass_4_f(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
	cdata_t t0, t1, t2, t3, t4, t5, t6, t7;
	t0[0] = din[0]; t0[1] = din[1];
	t1[0] = din[4]; t1[1] = din[5];
	t2[0] = din[2]; t2[1] = din[3];
	t3[0] = din[6]; t3[1] = din[7];
	
	t4[0] = t0[0] + t1[0]; t4[1] = t0[1] + t1[1];
	t5[0] = t0[0] - t1[0]; t5[1] = t0[1] - t1[1];
	t6[0] = t2[0] + t3[0]; t6[1] = t2[1] + t3[1];
	t7[0] = t2[0] - t3[0]; t7[1] = t2[1] - t3[1];

	dout[0] = t4[0] + t6[0]; dout[1] = t4[1] + t6[1];
	dout[4] = t4[0] - t6[0]; dout[5] = t4[1] - t6[1];
	dout[2] = t5[0] + t7[1]; dout[3] = t5[1] - t7[0];
	dout[6] = t5[0] - t7[1]; dout[7] = t5[1] + t7[0];
}
__INLINE void 
firstpass_4_b(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
	cdata_t t0, t1, t2, t3, t4, t5, t6, t7;
	t0[0] = din[0]; t0[1] = din[1];
	t1[0] = din[4]; t1[1] = din[5];
	t2[0] = din[2]; t2[1] = din[3];
	t3[0] = din[6]; t3[1] = din[7];
	
	t4[0] = t0[0] + t1[0]; t4[1] = t0[1] + t1[1];
	t5[0] = t0[0] - t1[0]; t5[1] = t0[1] - t1[1];
	t6[0] = t2[0] + t3[0]; t6[1] = t2[1] + t3[1];
	t7[0] = t2[0] - t3[0]; t7[1] = t2[1] - t3[1];

	dout[0] = t4[0] + t6[0]; dout[1] = t4[1] + t6[1];
	dout[4] = t4[0] - t6[0]; dout[5] = t4[1] - t6[1];
	dout[2] = t5[0] - t7[1]; dout[3] = t5[1] + t7[0];
	dout[6] = t5[0] + t7[1]; dout[7] = t5[1] - t7[0];
}
__INLINE void 
firstpass_2(ffts_plan_t *  p, const void *  in, void *  out) {
  const data_t *din = (const data_t *)in;
	data_t *dout = (data_t *)out;
	cdata_t t0, t1, r0,r1;
	t0[0] = din[0]; t0[1] = din[1];
	t1[0] = din[2]; t1[1] = din[3];
	r0[0] = t0[0] + t1[0];
	r0[1] = t0[1] + t1[1];
	r1[0] = t0[0] - t1[0];
	r1[1] = t0[1] - t1[1];
	dout[0] = r0[0]; dout[1] = r0[1];
	dout[2] = r1[0]; dout[3] = r1[1];
}
#endif
