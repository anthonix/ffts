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
#ifndef __NEON_FLOAT_H__
#define __NEON_FLOAT_H__

#include <arm_neon.h>

//#define VL 4
#define __INLINE static inline __attribute__((always_inline))

typedef float32x4_t V;

typedef float32x4x2_t VS;

#if defined(complex)
	typedef complex float cdata_t;
#else
	typedef float cdata_t[2];
#endif
	typedef float data_t;

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
__INLINE void FMA(V *Rd, V Rn, V Rm) {
	*Rd = vmlaq_f32(*Rd, Rn, Rm);
//  __asm__ ("vmla.f32 %q0,%q1,%q2\n\t"
//           : "+w" (*Rd)
//           : "w" (Rn), "w" (Rm)
//           //: "0"
//           );
    
}
__INLINE void FMS(V *Rd, V Rn, V Rm) {
	*Rd = vmlsq_f32(*Rd, Rn, Rm);
//	__asm__ ("vmls.f32 %q0,%q1,%q2\n\t"
//           : "+w" (*Rd)
//           : "w" (Rn), "w" (Rm)
//          // : "0"
//           );
}

__INLINE VS VSMUL(VS *d, VS *w) {
    VS t;
    t.val[0] = vmulq_f32(d->val[0], w->val[0]);
    t.val[1] = vmulq_f32(d->val[0], w->val[1]);
//  t.val[0] = vmlsq_f32(t.val[0], d->val[1], w->val[1]);
//  t.val[1] = vmlaq_f32(t.val[1], d->val[1], w->val[0]);
    FMS(&t.val[0], d->val[1], w->val[1]);
    FMA(&t.val[1], d->val[1], w->val[0]);
    return t;
}	
__INLINE VS VSMULJ(VS *d, VS *w) {
    VS t;
    t.val[0] = vmulq_f32(d->val[0], w->val[0]);
    t.val[1] = vmulq_f32(d->val[1], w->val[0]);
//  t.val[0] = vmlaq_f32(t.val[0], d->val[1], w->val[1]);
//  t.val[1] = vmlsq_f32(t.val[1], d->val[0], w->val[1]);
    FMA(&t.val[0], d->val[1], w->val[1]);
    FMS(&t.val[1], d->val[0], w->val[1]);
    return t;
}	
__INLINE VS VSADD(VS *a, VS *b) {
    VS r; 
    r.val[0] = vaddq_f32(a->val[0], b->val[0]); 
    r.val[1] = vaddq_f32(a->val[1], b->val[1]); 
    return r;
}
__INLINE VS VSSUB(VS *a, VS *b) {
    VS r; 
    r.val[0] = vsubq_f32(a->val[0], b->val[0]); 
    r.val[1] = vsubq_f32(a->val[1], b->val[1]); 
    return r;
}
__INLINE VS VSSUB_MULI(VS *a, VS *b) {
    VS r; 
    r.val[0] = vaddq_f32(a->val[0], b->val[1]); 
    r.val[1] = vsubq_f32(a->val[1], b->val[0]); 
    return r;
}
__INLINE VS VSADD_MULI(VS *a, VS *b) {
    VS r; 
    r.val[0] = vsubq_f32(a->val[0], b->val[1]); 
    r.val[1] = vaddq_f32(a->val[1], b->val[0]); 
    return r;
}

__INLINE void VSK_N(VS w, VS *r0, VS *r1, VS *r2, VS *r3) {
	VS uk, uk2, zk_p, zk_n, zk, zk_d;
	uk   = *r0; uk2  = *r1;
  zk_p = VSMUL(r2, &w);
  zk_n = VSMULJ(r3, &w);
	
	zk   = VSADD(&zk_p, &zk_n);
  zk_d = VSSUB(&zk_p, &zk_n);
  
	*r2 = VSSUB(&uk, &zk);
  *r0 = VSADD(&uk, &zk);
  *r3 = VSADD_MULI(&uk2, &zk_d);
  *r1 = VSSUB_MULI(&uk2, &zk_d);
}


__INLINE float32x2x2_t HVS_ADD(float32x2x2_t a, float32x2x2_t b) {
	float32x2x2_t rval;
	rval.val[0] = vadd_f32(a.val[0], b.val[0]);
	rval.val[1] = vadd_f32(a.val[1], b.val[1]);
	return rval;
}
__INLINE float32x2x2_t HVS_SUB(float32x2x2_t a, float32x2x2_t b) {
	float32x2x2_t rval;
	rval.val[0] = vsub_f32(a.val[0], b.val[0]);
	rval.val[1] = vsub_f32(a.val[1], b.val[1]);
	return rval;
}
__INLINE float32x2x2_t HVS_SUB_MULI(float32x2x2_t a, float32x2x2_t b) {
	float32x2x2_t rval;
	rval.val[0] = vadd_f32(a.val[0], b.val[1]);
	rval.val[1] = vsub_f32(a.val[1], b.val[0]);
	return rval;
}
__INLINE float32x2x2_t HVS_ADD_MULI(float32x2x2_t a, float32x2x2_t b) {
	float32x2x2_t rval;
	rval.val[0] = vsub_f32(a.val[0], b.val[1]);
	rval.val[1] = vadd_f32(a.val[1], b.val[0]);
	return rval;
}
__INLINE float32x2x2_t HVS_MUL(float32x2x2_t d, float32x2x2_t w) {
    float32x2x2_t t;
		t.val[0] = vmul_f32(d.val[0], w.val[0]);
		t.val[1] = vmul_f32(d.val[0], w.val[1]);
		t.val[0] = vmls_f32(t.val[0], d.val[1], w.val[1]);
		t.val[1] = vmla_f32(t.val[1], d.val[1], w.val[0]);
    return t;
}
__INLINE float32x2x2_t HVS_MULJ(float32x2x2_t d, float32x2x2_t w) {
    float32x2x2_t t;
		t.val[0] = vmul_f32(d.val[0], w.val[0]);
		t.val[1] = vmul_f32(d.val[1], w.val[0]);
		t.val[0] = vmla_f32(t.val[0], d.val[1], w.val[1]);
		t.val[1] = vmls_f32(t.val[1], d.val[0], w.val[1]);
    return t;
}
__INLINE void HVS_K_N(float32x2x2_t w, float32x2x2_t *r0, float32x2x2_t *r1, float32x2x2_t *r2, float32x2x2_t *r3) {
    float32x2x2_t uk, uk2, zk_p, zk_n, zk, zk_d;
		uk   = *r0; uk2  = *r1;
		zk_p = HVS_MUL(*r2, w);
    zk_n = HVS_MULJ(*r3, w);
    zk   = HVS_ADD(zk_p, zk_n);
    zk_d = HVS_SUB(zk_p, zk_n);
    
    *r2 = HVS_SUB(uk, zk);
    *r0 = HVS_ADD(uk, zk);
    *r3 = HVS_ADD_MULI(uk2, zk_d);
    *r1 = HVS_SUB_MULI(uk2, zk_d);
}

typedef union {
	float32x4_t f32x4;
	float32x2x2_t f32x2x2;
} float_mixed_t;

__INLINE void VSWP(float32x2x2_t *a, float32x2x2_t *b) {
//float32x2_t tmp = a->val[1];
//a->val[1] = b->val[0];
//b->val[0] = tmp; 
    __asm__ ("vswp %0,%1\n\t"
             : "+w" (a->val[1]), "+w" (b->val[0])
             :
                 );
}

static const __attribute__ ((aligned(16))) float ee_w_data[4] = {0.70710678118654757273731092936941,0.70710678118654746171500846685376,
    -0.70710678118654757273731092936941,-0.70710678118654746171500846685376};
__INLINE void LEAF_EE8_SPLIT(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

	float32x2x2_t r0, r1, r2, r3, r4, r5, r6, r7;
	float32x2x2_t t0, t1, t2, t3, t4, t5, t6, t7;

	t0 = vld2_f32(in + (*is)[0]); t1 = vld2_f32(in + (*is)[1]); t2 = vld2_f32(in + (*is)[2]); t3 = vld2_f32(in + (*is)[3]); 

	t4 = HVS_ADD (t0, t1);
	t5 = HVS_SUB (t0, t1);
	t6 = HVS_ADD (t2, t3);
	t7 = HVS_SUB (t2, t3);
	r0 = HVS_ADD (t4, t6);
	r2 = HVS_SUB (t4, t6);
	r1 = HVS_SUB_MULI(t5, t7);
	r3 = HVS_ADD_MULI(t5, t7);
    
	t0 = vld2_f32(in + (*is)[4]); t1 = vld2_f32(in + (*is)[5]); t2 = vld2_f32(in + (*is)[6]); t3 = vld2_f32(in + (*is)[7]); 
	r4 = HVS_ADD (t0, t1);
	r5 = HVS_SUB (t0, t1);
	r6 = HVS_ADD (t2, t3);
	r7 = HVS_SUB (t2, t3);
	t0 = r0; t1 = r2;
	t2 = HVS_ADD(r4, r6);
	t3 = HVS_SUB(r4, r6);
	r0 = HVS_ADD(t0, t2);
	r4 = HVS_SUB(t0, t2);
	r2 = HVS_SUB_MULI(t1, t3);
	r6 = HVS_ADD_MULI(t1, t3);

	float32x4_t w = vld1q_f32(ee_w_data);
	float32x2x2_t ww;
	ww.val[0] = vget_low_f32(w);
	ww.val[1] = vget_high_f32(w);

	HVS_K_N(ww,&r1,&r3,&r5,&r7);

//vst2_f32(out0,   r0);
//vst2_f32(out0+4, r2);
//vst2_f32(out0+8, r4);
//vst2_f32(out0+12, r6);

//vst2_f32(out1,   r1);
//vst2_f32(out1+4, r3);
//vst2_f32(out1+8, r5);
//vst2_f32(out1+12, r7);

	float32x2x2_t tt0, tt1, tt2, tt3, tt4, tt5, tt6, tt7;

	tt0 = vtrn_f32(r0.val[0], r0.val[1]);
	tt1 = vtrn_f32(r1.val[0], r1.val[1]);
	tt2 = vtrn_f32(r2.val[0], r2.val[1]);
	tt3 = vtrn_f32(r3.val[0], r3.val[1]);
	tt4 = vtrn_f32(r4.val[0], r4.val[1]);
	tt5 = vtrn_f32(r5.val[0], r5.val[1]);
	tt6 = vtrn_f32(r6.val[0], r6.val[1]);
	tt7 = vtrn_f32(r7.val[0], r7.val[1]);

//VSWP(&tt0.f32x2x2, &tt1.f32x2x2);	
//VSWP(&tt2.f32x2x2, &tt3.f32x2x2);	
//VSWP(&tt4.f32x2x2, &tt5.f32x2x2);	
//VSWP(&tt6.f32x2x2, &tt7.f32x2x2);	

	float32x4_t z0, z1, z2, z3, z4, z5, z6, z7;

	z0 = vcombine_f32(tt0.val[0], tt1.val[0]);
	z1 = vcombine_f32(tt0.val[1], tt1.val[1]);
	z2 = vcombine_f32(tt2.val[0], tt3.val[0]);
	z3 = vcombine_f32(tt2.val[1], tt3.val[1]);
	z4 = vcombine_f32(tt4.val[0], tt5.val[0]);
	z5 = vcombine_f32(tt4.val[1], tt5.val[1]);
	z6 = vcombine_f32(tt6.val[0], tt7.val[0]);
	z7 = vcombine_f32(tt6.val[1], tt7.val[1]);


	vst1q_f32(out0,    z0);
	vst1q_f32(out0+4,  z2);
	vst1q_f32(out0+8,  z4);
	vst1q_f32(out0+12, z6);
	
	vst1q_f32(out1,    z1);
	vst1q_f32(out1+4,  z3);
	vst1q_f32(out1+8,  z5);
	vst1q_f32(out1+12, z7);
/*
  vst1_f32(out0,    tt0.val[0]);
  vst1_f32(out0+2,  tt1.val[0]);
  vst1_f32(out0+4,  tt2.val[0]);
  vst1_f32(out0+6,  tt3.val[0]);
  vst1_f32(out0+8,  tt4.val[0]);
  vst1_f32(out0+10, tt5.val[0]);
  vst1_f32(out0+12, tt6.val[0]);
  vst1_f32(out0+14, tt7.val[0]);
  
  vst1_f32(out1,    tt0.val[1]);
  vst1_f32(out1+2,  tt1.val[1]);
  vst1_f32(out1+4,  tt2.val[1]);
  vst1_f32(out1+6,  tt3.val[1]);
  vst1_f32(out1+8,  tt4.val[1]);
  vst1_f32(out1+10, tt5.val[1]);
  vst1_f32(out1+12, tt6.val[1]);
  vst1_f32(out1+14, tt7.val[1]);
	*/
/*
	float32x4_t rr0 = vcombine_f32(r0.val[0], r0.val[1]);
	float32x4_t rr1 = vcombine_f32(r1.val[0], r1.val[1]);
	float32x4_t rr2 = vcombine_f32(r2.val[0], r2.val[1]);
	float32x4_t rr3 = vcombine_f32(r3.val[0], r3.val[1]);
	
	float32x4x2_t tmp0, tmp1, tmp2, tmp3;
	tmp0 = vtrnq_f32(rr0, rr2);
	tmp1 = vtrnq_f32(rr1, rr3);


	float32x2x2_t v0, v1, v2, v3;
	v0.val[0] = vget_low_f32(tmp0.val[0]);
	v0.val[1] = vget_high_f32(tmp0.val[0]);
	v1.val[0] = vget_low_f32(tmp0.val[1]);
	v1.val[1] = vget_high_f32(tmp0.val[1]);
	v2.val[0] = vget_low_f32(tmp1.val[0]);
	v2.val[1] = vget_high_f32(tmp1.val[0]);
	v3.val[0] = vget_low_f32(tmp1.val[1]);
	v3.val[1] = vget_high_f32(tmp1.val[1]);

	tmp2.val[0] = tmp0.val[0];
	tmp2.val[1] = tmp1.val[0];
	tmp3.val[0] = tmp0.val[1];
	tmp3.val[1] = tmp1.val[1];

//vst2q_f32(out0 , tmp2);
//vst2q_f32(out1 , tmp3);
  vst2_f32(out0,   v0);
  vst2_f32(out0+4, v1);
  vst2_f32(out1,   v2);
  vst2_f32(out1+4, v3);

	float32x4_t rr4 = vcombine_f32(r4.val[0], r4.val[1]);
	float32x4_t rr5 = vcombine_f32(r5.val[0], r5.val[1]);
	float32x4_t rr6 = vcombine_f32(r6.val[0], r6.val[1]);
	float32x4_t rr7 = vcombine_f32(r7.val[0], r7.val[1]);

	tmp0 = vtrnq_f32(rr4, rr6);
	tmp1 = vtrnq_f32(rr5, rr7);

	tmp2.val[0] = tmp0.val[0];
	tmp2.val[1] = tmp1.val[0];
	tmp3.val[0] = tmp0.val[1];
	tmp3.val[1] = tmp1.val[1];
	v0.val[0] = vget_low_f32(tmp0.val[0]);
	v0.val[1] = vget_high_f32(tmp0.val[0]);
	v1.val[0] = vget_low_f32(tmp0.val[1]);
	v1.val[1] = vget_high_f32(tmp0.val[1]);
	v2.val[0] = vget_low_f32(tmp1.val[0]);
	v2.val[1] = vget_high_f32(tmp1.val[0]);
	v3.val[0] = vget_low_f32(tmp1.val[1]);
	v3.val[1] = vget_high_f32(tmp1.val[1]);
  vst2_f32(out0+8,   v0);
  vst2_f32(out0+12, v1);
  vst2_f32(out1+8,   v1);
  vst2_f32(out1+12, v3);

//vst2q_f32(out0 + 8, tmp2);
//vst2q_f32(out1 + 8, tmp3);
//vst1q_f32(out0+8, tmp0.val[0]);
//vst1q_f32(out0+12,tmp0.val[1]);
//vst1q_f32(out1+8, tmp1.val[0]);
//vst1q_f32(out1+12,tmp1.val[1]);
	*/
	*is += 8;
}

__INLINE void STORESPR(data_t * addr,  VS p) {
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
									 		: 
		                	: "r" (addr), "w" (p.val[0]), "w" (p.val[1])
	                 		: "memory");
}
__INLINE void STORESPRI(data_t * restrict * addr,  V p0, V p1) {
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
}
__INLINE void STORESPRI0(data_t * restrict *addr,  VS r) {
	register V p0 __asm__ ("q0") = r.val[0];
	register V p1 __asm__ ("q1") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRI1(data_t **addr,  VS r) {
	register V p0 __asm__ ("q2") = r.val[0];
	register V p1 __asm__ ("q3") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRI2(data_t **addr,  VS r) {
	register V p0 __asm__ ("q4") = r.val[0];
	register V p1 __asm__ ("q5") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRI3(data_t **addr,  VS r) {
	register V p0 __asm__ ("q6") = r.val[0];
	register V p1 __asm__ ("q7") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRIT0(data_t * restrict *addr,  VS r) {
	register V p0 __asm__ ("q0") = r.val[0];
	register V p1 __asm__ ("q1") = r.val[1];
	__asm__ __volatile__ ("vst2.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRIT1(data_t **addr,  VS r) {
	register V p0 __asm__ ("q2") = r.val[0];
	register V p1 __asm__ ("q3") = r.val[1];
	__asm__ __volatile__ ("vst2.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRIT2(data_t **addr,  VS r) {
	register V p0 __asm__ ("q4") = r.val[0];
	register V p1 __asm__ ("q5") = r.val[1];
	__asm__ __volatile__ ("vst2.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPRIT3(data_t **addr,  VS r) {
	register V p0 __asm__ ("q6") = r.val[0];
	register V p1 __asm__ ("q7") = r.val[1];
	__asm__ __volatile__ ("vst2.32 {%q1,%q2}, [%0, :128]!\n\t"
									 		: "+r" (*addr)
		                	: "w" (p0), "w" (p1)
	                 		: "memory");
	//STORESPRI(addr, p0, p1);
}
__INLINE void STORESPR0(data_t *addr,  VS r) {
	register V p0 __asm__ ("q0") = r.val[0];
	register V p1 __asm__ ("q1") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
									 		: 
		                	: "r" (addr), "w" (p0), "w" (p1)
	                 		: "memory");
}
__INLINE void STORESPR1(data_t *addr,  VS r) {
	register V p0 __asm__ ("q2") = r.val[0];
	register V p1 __asm__ ("q3") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
									 		: 
		                	: "r" (addr), "w" (p0), "w" (p1)
	                 		: "memory");
}
__INLINE void STORESPR2(data_t *addr,  VS r) {
	register V p0 __asm__ ("q4") = r.val[0];
	register V p1 __asm__ ("q5") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
									 		: 
		                	: "r" (addr), "w" (p0), "w" (p1)
	                 		: "memory");
}
__INLINE void STORESPR3(data_t *addr,  VS r) {
	register V p0 __asm__ ("q6") = r.val[0];
	register V p1 __asm__ ("q7") = r.val[1];
	__asm__ __volatile__ ("vst1.32 {%q1,%q2}, [%0, :128]\n\t"
									 		: 
		                	: "r" (addr), "w" (p0), "w" (p1)
	                 		: "memory");
}
__INLINE VS LOADSPR0(data_t *addr) {
	VS r;
	register V p0 __asm__ ("q8") ;
	register V p1 __asm__ ("q9") ;
	__asm__ __volatile__("vld1.32 {%q0,%q1}, [%2, :128]\n\t"
			: "=&w" (p0), "=&w" (p1)
			: "r" (addr)
			);
	r.val[0] = p0; r.val[1] = p1;
	return r;
}
__INLINE VS LOADSPR1(data_t *addr) {
	VS r;
	register V p0 __asm__ ("q10") ;
	register V p1 __asm__ ("q11") ;
	__asm__ __volatile__("vld1.32 {%q0,%q1}, [%2, :128]\n\t"
			: "=&w" (p0), "=&w" (p1)
			: "r" (addr)
			);
	r.val[0] = p0; r.val[1] = p1;
	return r;
}
__INLINE VS LOADSPR2(data_t *addr) {
	VS r;
	register V p0 __asm__ ("q12") ;
	register V p1 __asm__ ("q13") ;
	__asm__ __volatile__("vld1.32 {%q0,%q1}, [%2, :128]\n\t"
			: "=&w" (p0), "=&w" (p1)
			: "r" (addr)
			);
	r.val[0] = p0; r.val[1] = p1;
	return r;
}
__INLINE VS LOADSPR3(data_t *addr) {
	VS r;
	register V p0 __asm__ ("q14") ;
	register V p1 __asm__ ("q15") ;
	__asm__ __volatile__("vld1.32 {%q0,%q1}, [%2, :128]\n\t"
			: "=&w" (p0), "=&w" (p1)
			: "r" (addr)
			);
	r.val[0] = p0; r.val[1] = p1;
	return r;
}
__INLINE VS LOADSPRI(data_t * restrict * addr) {
	VS r;
	register V p0 __asm__ ("q2") ;
	register V p1 __asm__ ("q3") ;
	__asm__ __volatile__("vld1.32 {%q0,%q1}, [%2, :128]!\n\t"
			: "=&w" (p0), "=&w" (p1), "+r" (*addr)
			: 
			);
	r.val[0] = p0; r.val[1] = p1;
	return r;
}

__INLINE void X_4_SPLIT(data_t * restrict data, size_t N, data_t * restrict LUT) {

//size_t i;
//for(i=0;i<N/4/2/2;i++) {
		VS uk = LOADSPR0(data);
		VS uk2 = LOADSPR1(data + 2*N/4);
		VS zk_p = LOADSPR2(data + 4*N/4);
		VS zk_n = LOADSPR3(data + 6*N/4);

		VSK_N(LOADSPRI(&LUT), &uk, &uk2, &zk_p, &zk_n);

		STORESPR0(data, uk); 
		STORESPR1(data + 2*N/4, uk2); 
		STORESPR2(data + 4*N/4, zk_p);
		STORESPR3(data + 6*N/4, zk_n);

//		LUT += 8;
//		data += 8;
//	}
}

__INLINE void X_8_SPLIT(data_t * restrict data0, size_t N, data_t * restrict LUT) {
	data_t *data2 = data0 + 2*N/4;
	data_t *data4 = data0 + 4*N/4;
	data_t *data6 = data0 + 6*N/4;
	data_t *data1 = data0 + 1*N/4;
	data_t *data3 = data0 + 3*N/4;
	data_t *data5 = data0 + 5*N/4;
	data_t *data7 = data0 + 7*N/4;
	size_t k, n4 = N/4;

	for(k=N/8/2/2;k>0;--k) {
      VS r0, r1, r2, r3, r4, r5, r6, r7,w;
			r0 = LOADSPR0(data0);
			r2 = LOADSPR1(data2);
			r1 = LOADSPR2(data1);
			r3 = LOADSPR3(data3);
			VSK_N(LOADSPRI(&LUT), &r0, &r1, &r2, &r3);
			STORESPR2(data1, r1);
			STORESPR3(data3, r3);
			r4 = LOADSPR2(data4);
			r6 = LOADSPR3(data6);
			VSK_N(LOADSPRI(&LUT), &r0, &r2, &r4, &r6);
			STORESPRI0(&data0, r0); //data0 += 8;
			STORESPRI1(&data2, r2); //data2 += 8;
			STORESPRI2(&data4, r4); //data4 += 8;
			STORESPRI3(&data6, r6); //data6 += 8;
			r1 = LOADSPR0(data1);
			r3 = LOADSPR1(data3);
			r5 = LOADSPR2(data5);
			r7 = LOADSPR3(data7);
			VSK_N(LOADSPRI(&LUT), &r1, &r3, &r5, &r7);
	//		LUT += 24;
			STORESPRI0(&data1, r1); //data1 += 8;
			STORESPRI1(&data3, r3); //data3 += 8;
			STORESPRI2(&data5, r5); //data5 += 8;
			STORESPRI3(&data7, r7); //data7 += 8;
	}
}

__INLINE void X_8_SPLIT_T(data_t * restrict data0, size_t N, data_t * restrict LUT) {
	data_t *data2 = data0 + 2*N/4;
	data_t *data4 = data0 + 4*N/4;
	data_t *data6 = data0 + 6*N/4;
	data_t *data1 = data0 + 1*N/4;
	data_t *data3 = data0 + 3*N/4;
	data_t *data5 = data0 + 5*N/4;
	data_t *data7 = data0 + 7*N/4;
	size_t k, n4 = N/4;

	for(k=N/8/2/2;k>0;--k) {
      VS r0, r1, r2, r3, r4, r5, r6, r7,w;
			r0 = LOADSPR0(data0);
			r2 = LOADSPR1(data2);
			r1 = LOADSPR2(data1);
			r3 = LOADSPR3(data3);
			VSK_N(LOADSPRI(&LUT), &r0, &r1, &r2, &r3);
			STORESPR2(data1, r1);
			STORESPR3(data3, r3);
			r4 = LOADSPR2(data4);
			r6 = LOADSPR3(data6);
			VSK_N(LOADSPRI(&LUT), &r0, &r2, &r4, &r6);
			STORESPRIT0(&data0, r0); //data0 += 8;
			STORESPRIT1(&data2, r2); //data2 += 8;
			STORESPRIT2(&data4, r4); //data4 += 8;
			STORESPRIT3(&data6, r6); //data6 += 8;
			r1 = LOADSPR0(data1);
			r3 = LOADSPR1(data3);
			r5 = LOADSPR2(data5);
			r7 = LOADSPR3(data7);
			VSK_N(LOADSPRI(&LUT), &r1, &r3, &r5, &r7);
			STORESPRIT0(&data1, r1); //data1 += 8;
			STORESPRIT1(&data3, r3); //data3 += 8;
			STORESPRIT2(&data5, r5); //data5 += 8;
			STORESPRIT3(&data7, r7); //data7 += 8;
	}
}
__INLINE V LOAD2I(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]!\n\t"
             : "=w" (o), "+r" (*addr)
             : 
             ); 

    return o;
}
__INLINE V LOAD2I_0(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag0\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_1(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag1\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_2(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag2\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_3(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag3\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_4(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag4\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_5(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag5\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_6(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag6\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOAD2I_7(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld2.32 {%q0}, [%1, :128]! @tag7\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}



__INLINE V LOADI(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld1.32 {%q0}, [%1, :128]!\n\t" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOADI_2(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld1.32 {%q0}, [%1, :128]!\n\t  @tag2" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V LOADI_3(const data_t **addr) {
    float32x4_t o;
    __asm__ ("vld1.32 {%q0}, [%1, :128]!\n\t  @tag3" : "=w" (o), "+r" (*addr) : ); 
    return o;
}
__INLINE V HSP_MUL(V *d, const V *w) {
    V t;
    t = vcombine_f32(vmul_f32(vget_low_f32(*d), vget_low_f32(*w)), 
                     vmul_f32(vget_low_f32(*d), vget_high_f32(*w)));
    t = vcombine_f32(vmls_f32(vget_low_f32(t), vget_high_f32(*d), vget_high_f32(*w)), 
                     vmla_f32(vget_high_f32(t), vget_high_f32(*d), vget_low_f32(*w)));
    return t;
}
__INLINE V HSP_MULJ(V *d, const V *w) {
    V t;
    t = vcombine_f32(vmul_f32(vget_low_f32(*d), vget_low_f32(*w)), 
                     vmul_f32(vget_high_f32(*d), vget_low_f32(*w)));
    t = vcombine_f32(vmla_f32(vget_low_f32(t), vget_high_f32(*d), vget_high_f32(*w)), 
                     vmls_f32(vget_high_f32(t), vget_low_f32(*d), vget_high_f32(*w)));
    return t;
}
__INLINE V HSP_SUB_MULI(V *a, V *b) {
    return vcombine_f32(vadd_f32(vget_low_f32(*a), vget_high_f32(*b)), vsub_f32(vget_high_f32(*a), vget_low_f32(*b)));
}
__INLINE V HSP_ADD_MULI(V *a, V *b) {
    return vcombine_f32(vsub_f32(vget_low_f32(*a), vget_high_f32(*b)), vadd_f32(vget_high_f32(*a), vget_low_f32(*b)));
}

__INLINE void K_N_HSP(const V *w, V *r0, V *r1, V *r2, V *r3) {
    V uk, uk2, zk_p, zk_n, zk, zk_d;
    
	uk   = *r0;
	uk2  = *r1;
    zk_p = HSP_MUL(r2, w);
    zk_n = HSP_MULJ(r3, w);
    zk   = ADD(zk_p, zk_n);
    zk_d = SUB(zk_p, zk_n);
    
    *r2 = SUB(uk, zk);
    *r0 = ADD(uk, zk);
    *r3 = HSP_ADD_MULI(&uk2, &zk_d);
    *r1 = HSP_SUB_MULI(&uk2, &zk_d);
}

__INLINE void neon_shl8_ee(data_t *restrict out0, data_t *restrict out1,const data_t **restrict i0,const data_t **restrict i1,const data_t **restrict i2,const data_t **restrict i3,const data_t **restrict i4,const data_t **restrict i5,const data_t **restrict i6,const data_t **restrict i7) {
    
    V r0, r1, r2, r3, r4, r5, r6, r7;
    V t0, t1, t2, t3, t4, t5, t6, t7;
    
    
    t0 = LOAD2I_0(i0);    
    t1 = LOAD2I_1(i1);    
    t2 = LOAD2I_2(i2);    
    t3 = LOAD2I_3(i3);    
    t4 = ADD (t0, t1);
    t5 = SUB (t0, t1);
    t6 = ADD (t2, t3);
    t7 = SUB (t2, t3);
    r0 = ADD (t4, t6);
    r2 = SUB (t4, t6);
    r1 = HSP_SUB_MULI(&t5, &t7);
    r3 = HSP_ADD_MULI(&t5, &t7);
    
    t0 = LOAD2I_4(i4);    
    t1 = LOAD2I_5(i5);    
    t2 = LOAD2I_6(i6);    
    t3 = LOAD2I_7(i7);    
    r4 = ADD (t0, t1);
    r5 = SUB (t0, t1);
    r6 = ADD (t2, t3);
    r7 = SUB (t2, t3);
    
    t0 = r0; t1 = r2;
    t2 = ADD(r4, r6);
    t3 = SUB(r4, r6);
    r0 = ADD(t0, t2);
    r4 = SUB(t0, t2);
    r2 = HSP_SUB_MULI(&t1, &t3);
    r6 = HSP_ADD_MULI(&t1, &t3);
    
    V w = vld1q_f32(ee_w_data);
    
    K_N_HSP(&w,&r1,&r3,&r5,&r7);
    V uk, uk2, zk, zk_d;
   
    float32x4x2_t tmp1 = vtrnq_f32(r0, r2);
    r0 = tmp1.val[0];
    r2 = tmp1.val[1];
    float32x4x2_t tmp4 = vtrnq_f32(r1, r3);
    r1 = tmp4.val[0];
    r3 = tmp4.val[1];
    register V tt0 __asm__ ("q0") = r0;
    register V tt1 __asm__ ("q1") = r1;
    register V tt2 __asm__ ("q2") = r2;
    register V tt3 __asm__ ("q3") = r3;
    __asm__ __volatile__ ("vst2.32 {q0,q1}, [%0, :128]!\n\t" : "+&r" (out0): "w"(tt0), "w"(tt1) : "memory");
    __asm__ __volatile__ ("vst2.32 {q2,q3}, [%0, :128]!\n\t" : "+&r" (out1): "w"(tt2), "w"(tt3) : "memory");
    
    float32x4x2_t tmp2 = vtrnq_f32(r4, r6);
    r4 = tmp2.val[0];
    r6 = tmp2.val[1];
    float32x4x2_t tmp3 = vtrnq_f32(r5, r7);
    r5 = tmp3.val[0];
    r7 = tmp3.val[1];
    register V tt4 __asm__ ("q4") = r4;
    register V tt5 __asm__ ("q5") = r5;
    register V tt6 __asm__ ("q6") = r6;
    register V tt7 __asm__ ("q7") = r7;
    
    __asm__ __volatile__ ("vst2.32 {q4,q5}, [%0, :128]!\n\t" : "+&r" (out0): "w"(tt4), "w"(tt5) : "memory");
    __asm__ __volatile__ ("vst2.32 {q6,q7}, [%0, :128]!\n\t" : "+&r" (out1): "w"(tt6), "w"(tt7) : "memory");
  
}

__INLINE void neon_shl8_oo(data_t *restrict out0, data_t *restrict out1,const data_t **restrict i0,const data_t **restrict i1,const data_t **restrict i2,const data_t **restrict i3,const data_t **restrict i4,const data_t **restrict i5,const data_t **restrict i6,const data_t **restrict i7) {
    
    V r0, r1, r2, r3, r4, r5, r6, r7;
    V t0, t1, t2, t3, t4, t5, t6, t7; 
    
    t0 = LOAD2I_0(i0);    
    t1 = LOAD2I_1(i1);    
    t2 = LOAD2I_2(i2);    
    t3 = LOAD2I_3(i3);    
    t4 = ADD (t0, t1);
    t5 = SUB (t0, t1);
    t6 = ADD (t2, t3);
    t7 = SUB (t2, t3);
    r0 = ADD (t4, t6);
    r2 = SUB (t4, t6);
    r1 = HSP_SUB_MULI(&t5, &t7);
    r3 = HSP_ADD_MULI(&t5, &t7);
    
    float32x4x2_t tmp1 = vtrnq_f32(r0, r2);
    r0 = tmp1.val[0];
    r2 = tmp1.val[1];
    float32x4x2_t tmp4 = vtrnq_f32(r1, r3);
    r1 = tmp4.val[0];
    r3 = tmp4.val[1];
    register V tt0 __asm__ ("q0") = r0;
    register V tt1 __asm__ ("q1") = r1;
    register V tt2 __asm__ ("q2") = r2;
    register V tt3 __asm__ ("q3") = r3;
    __asm__ __volatile__ ("vst2.32 {q0,q1}, [%0, :128]!\n\t" : "+&r" (out0): "w"(tt0), "w"(tt1) : "memory");
    __asm__ __volatile__ ("vst2.32 {q2,q3}, [%0, :128]!\n\t" : "+&r" (out1): "w"(tt2), "w"(tt3) : "memory");

    
    
    t0 = LOAD2I_4(i4);    
    t1 = LOAD2I_5(i5);    
    t2 = LOAD2I_6(i6);    
    t3 = LOAD2I_7(i7);    
    t4 = ADD (t0, t1);
    t5 = SUB (t0, t1);
    t6 = ADD (t2, t3);
    t7 = SUB (t2, t3);
    r4 = ADD (t4, t6);
    r6 = SUB (t4, t6);
    r5 = HSP_SUB_MULI(&t5, &t7);
    r7 = HSP_ADD_MULI(&t5, &t7);
      
     float32x4x2_t tmp2 = vtrnq_f32(r4, r6);
    r4 = tmp2.val[0];
    r6 = tmp2.val[1];
    float32x4x2_t tmp3 = vtrnq_f32(r5, r7);
    r5 = tmp3.val[0];
    r7 = tmp3.val[1];
    
    
    register V tt4 __asm__ ("q4") = r4;
    register V tt5 __asm__ ("q5") = r5;
    register V tt6 __asm__ ("q6") = r6;
    register V tt7 __asm__ ("q7") = r7;
    
    __asm__ __volatile__ ("vst2.32 {q4,q5}, [%0, :128]!\n\t" : "+&r" (out0): "w"(tt4), "w"(tt5) : "memory");
    __asm__ __volatile__ ("vst2.32 {q6,q7}, [%0, :128]!\n\t" : "+&r" (out1): "w"(tt6), "w"(tt7) : "memory");

    
    
}

static const __attribute__ ((aligned(16))) data_t eo_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};


__INLINE void neon_shl8_eo(data_t *restrict out0, data_t *restrict out1,const data_t **restrict i0,const data_t **restrict i1,const data_t **restrict i2,const data_t **restrict i3,const data_t **restrict i4,const data_t **restrict i5,const data_t **restrict i6,const data_t **restrict i7) {
    /*
    register V r0_1   __asm__ ("q0");
    register V r2_3   __asm__ ("q1");
    register V r4_5   __asm__ ("q2");
    register V r6_7   __asm__ ("q3");
   */
    const V w = vld1q_f32(eo_w_data);
    
    V r0_1, r2_3, r4_5, r6_7;
    
    register V r8_9   __asm__ ("q4");
    register V r10_11 __asm__ ("q5");
    register V r12_13 __asm__ ("q6");
    register V r14_15 __asm__ ("q7");
    
    {
    V t0, t1, t2, t3, t4, t5, t6, t7;
    t0 = LOAD2I_0(i0);    
    t1 = LOAD2I_1(i1);    
    t2 = LOAD2I_2(i2);    
    t3 = LOAD2I_3(i3);    
    t4 = ADD(t0, t1);
    t5 = SUB(t0, t1);
    t6 = ADD(t2, t3);
    t7 = SUB(t2, t3);
    
    t0 = ADD(t4, t6);
    t2 = SUB(t4, t6);
    t1 = HSP_SUB_MULI(&t5, &t7);
    t3 = HSP_ADD_MULI(&t5, &t7);
    
    float32x4x2_t tmp1 = vtrnq_f32(t0, t1);
        t0 = tmp1.val[0];
        t1 = tmp1.val[1];
    float32x4x2_t tmp2 = vtrnq_f32(t2, t3);
        t2 = tmp2.val[0];
        t3 = tmp2.val[1];

    r0_1 = t0;
    r2_3 = t2;
    r8_9 = t1;
    r10_11 = t3;
        __asm__ __volatile__ ("vswp d9,d10\n\t"
                              "vst1.32 {d8,d9,d10,d11}, [%0, :128]!\n\t"
//                              "vst1.32 {d8,d9}, [%0, :128]!\n\t"
//                              "vst1.32 {d10,d11}, [%0, :128]!\n\t"
                              : "+&r" (out1)
                              : "w" (r8_9), "w" (r10_11)
                              : "memory");

    }
    {
        V t0, t1, t2, t3, t4, t5, t6, t7;
        t0 = LOAD2I_4(i4);    
        t1 = LOAD2I_5(i5);    
        t2 = LOAD2I_6(i6);    
        t3 = LOAD2I_7(i7);    
        //t2 = HALFBLEND(t6, t7);
        //t3 = HALFBLEND(t7, t6);
        t4 = ADD(t0, t1);
        t5 = SUB(t0, t1);
        t6 = ADD(t2, t3);
        t7 = SUB(t2, t3);
        float32x4x2_t tmp1 = vtrnq_f32(t4, t5);
        r4_5 = tmp1.val[0];
        float32x4x2_t tmp2 = vtrnq_f32(t6, t7);
        r6_7 = tmp2.val[0];
        //t5 = MULI(t5);
        t0 = ADD(t6, t4);
        t2 = SUB(t6, t4);
        t1 = HSP_SUB_MULI(&t7, &t5);
        t3 = HSP_ADD_MULI(&t7, &t5);
        
        float32x4x2_t tmp3 = vtrnq_f32(t0, t1);
        r12_13 = tmp3.val[1];
        float32x4x2_t tmp4 = vtrnq_f32(t2, t3);
        r14_15 = tmp4.val[1];
        

        __asm__ __volatile__ ("vswp d13, d14\n\t"
                              "vst1.32 {d12,d13,d14,d15}, [%0, :128]!\n\t"
//                              "vst1.32 {d12,d13}, [%0, :128]!\n\t"
//                              "vst1.32 {d14,d15}, [%0, :128]!\n\t"
                              : "+&r" (out1)
                              : "w" (r12_13), "w" (r14_15)
                              : "memory");
      
        
    }
    
       K_N_HSP(&w,&r0_1,&r2_3,&r4_5,&r6_7);

    register V t0   __asm__ ("q0") = r0_1;
    register V t1   __asm__ ("q1") = r2_3;
    register V t2   __asm__ ("q2") = r4_5;
    register V t3   __asm__ ("q3") = r6_7;
    
    __asm__ __volatile__ ("vswp d1, d2\n\t"
                          "vswp d5, d6\n\t"
                          "vstmia %0!, {q0-q3}\n\t"
//                          "vst1.32 {d0,d1}, [%0, :128]!\n\t"
//                          "vst1.32 {d2,d3}, [%0, :128]!\n\t"
//                          "vst1.32 {d4,d5}, [%0, :128]!\n\t"
//                          "vst1.32 {d6,d7}, [%0, :128]\n\t"
                          : "+&r" (out0)
                          : "w" (t0), "w" (t1), "w" (t2), "w" (t3)
                          : "memory");
    
}
static const __attribute__ ((aligned(16))) data_t oe_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};

__INLINE void neon_shl8_oe(data_t *restrict out0, data_t *restrict out1,const data_t **restrict i0,const data_t **restrict i1,const data_t **restrict i2,const data_t **restrict i3,const data_t **restrict i4,const data_t **restrict i5,const data_t **restrict i6,const data_t **restrict i7) {
    register V r0_1   __asm__ ("q0");
    register V r2_3   __asm__ ("q1");
    register V r4_5   __asm__ ("q2");
    register V r6_7   __asm__ ("q3");
    
    V r8_9, r10_11, r12_13, r14_15;
    const V w = vld1q_f32(oe_w_data);
    
       {
        V t0, t1, t2, t3, t4, t5, t6, t7;
    t0 = LOAD2I_0(i0);    
    t1 = LOAD2I_1(i1);    
    t6 = LOADI_2(i2);    
    t7 = LOADI_3(i3);    
    
        float32x2x2_t tmp0 = vtrn_f32(vget_low_f32(t6), vget_high_f32(t7));
        float32x2x2_t tmp1 = vtrn_f32(vget_low_f32(t7), vget_high_f32(t6));
        t2 = vcombine_f32(tmp0.val[0], tmp0.val[1]);
        t3 = vcombine_f32(tmp1.val[0], tmp1.val[1]);
    
	t4 = ADD(t0, t1);
    t5 = SUB(t0, t1);
    t6 = ADD(t2, t3);
    t7 = SUB(t2, t3);
        float32x4x2_t tmp2 = vtrnq_f32(t4, t5);
        r12_13 = tmp2.val[1];
        float32x4x2_t tmp3 = vtrnq_f32(t6, t7);
        r14_15 = tmp3.val[1];
        
    t0 = ADD(t4, t6);
    t2 = SUB(t4, t6);
    t1 = HSP_SUB_MULI(&t5, &t7);
    t3 = HSP_ADD_MULI(&t5, &t7);
        float32x4x2_t tmp4 = vtrnq_f32(t0, t1);
        r0_1 = tmp4.val[0];
        float32x4x2_t tmp5 = vtrnq_f32(t2, t3);
        r2_3 = tmp5.val[0];
        __asm__ __volatile__ ("vswp d1, d2\n\t"
                              "vst1.32 {q0, q1}, [%0, :128]!\n\t"
//                              "vst1.32 {q1}, [%0, :128]!\n\t"
                              : "+&r" (out0)
                              : "w" (r0_1), "w" (r2_3)
                              : "memory");
    }
    {
        V t0, t1, t2, t3, t4, t5, t6, t7;
        t0 = LOAD2I_4(i4);    
        t1 = LOAD2I_5(i5);    
        t2 = LOAD2I_6(i6);    
        t3 = LOAD2I_7(i7);    
        t4 = ADD(t0, t1);
        t5 = SUB(t0, t1);
        t6 = ADD(t2, t3);
        t7 = SUB(t2, t3);
        t0 = ADD(t4, t6);
        t2 = SUB(t4, t6);
        t1 = HSP_SUB_MULI(&t5, &t7);
        t3 = HSP_ADD_MULI(&t5, &t7);
       
        float32x4x2_t tmp0 = vtrnq_f32(t0, t1);
        r4_5 = tmp0.val[0];
        r8_9 = tmp0.val[1];
        float32x4x2_t tmp1 = vtrnq_f32(t2, t3);
        r6_7 = tmp1.val[0];
        r10_11 = tmp1.val[1];

        
        __asm__ __volatile__ ("vswp d5, d6\n\t"
                              "vst1.32 {q2, q3}, [%0, :128]!\n\t"
//                              "vst1.32 {q3}, [%0, :128]!\n\t"
                              : "+&r" (out0)
                              : "w" (r4_5), "w" (r6_7)
                              : "memory");
        
    }
    
    K_N_HSP(&w,&r8_9,&r10_11,&r12_13,&r14_15);
    register V t0 __asm__ ("q4") = r8_9;
    register V t1 __asm__ ("q5") = r10_11;
    register V t2 __asm__ ("q6") = r12_13;
    register V t3 __asm__ ("q7") = r14_15;
    
    __asm__ __volatile__ ("vswp d9, d10\n\t"
                          "vswp d13, d14\n\t"
                          "vstmia %0!, {q4-q7}\n\t"
//                          "vst1.32 {q4}, [%0, :128]!\n\t"
//                          "vst1.32 {q5}, [%0, :128]!\n\t"
//                          "vst1.32 {q6}, [%0, :128]!\n\t"
//                          "vst1.32 {q7}, [%0, :128]\n\t"
                          : "+&r" (out1)
                          : "w" (t0), "w" (t1), "w" (t2), "w" (t3)
                          : "memory");
    
 
}
#endif
