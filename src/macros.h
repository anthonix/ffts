#ifndef __MACROS_H__
#define __MACROS_H__

#include "cp_sse.h"

#define __INLINE static inline __attribute__((always_inline))

#define VLIT4 _mm_set_ps

__INLINE __m128 IMULI(__m128 a) {
	__m128 temp = _mm_xor_ps(a, _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f));
	return _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(2,3,0,1));
}

__INLINE void 
S_4(__m128 r0, __m128 r1, __m128 r2, __m128 r3, data_t * restrict o0, data_t * restrict o1, data_t * restrict o2, data_t * restrict o3) {
 	__m128 t0, t1, t2, t3; 
  _mm_store_ps(o0, r0);
  _mm_store_ps(o1, r1);
  _mm_store_ps(o2, r2);
  _mm_store_ps(o3, r3);
}
__INLINE void S_2(__m128 r0, __m128 r1, data_t * restrict o0, data_t * restrict o1) {
  _mm_store_ps(o0, r0);
  _mm_store_ps(o1, r1);
}

__INLINE void L_S2(const data_t * restrict i0, const data_t * restrict i1, __m128 * restrict r0, __m128 * restrict r1) {
  __m128 t0, t1;
  t0 = _mm_load_ps(i0);    
  t1 = _mm_load_ps(i1);    
  *r0 = _mm_add_ps(t0, t1);
  *r1 = _mm_sub_ps(t0, t1);
}
__INLINE void 
L_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  __m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3;
  t0 = _mm_load_ps(i0);    
  t1 = _mm_load_ps(i1);    
  t2 = _mm_load_ps(i2);    
  t3 = _mm_load_ps(i3);    
  *r0 = _mm_add_ps (t0, t1);
  *r1 = _mm_sub_ps (t0, t1);
  *r2 = _mm_add_ps (t2, t3);
  *r3 = _mm_sub_ps (t2, t3);
}

__INLINE void 
L_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									__m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = _mm_load_ps(i0);    
  t1 = _mm_load_ps(i1);    
  t2 = _mm_load_ps(i2);    
  t3 = _mm_load_ps(i3);    
  t4 = _mm_add_ps (t0, t1);
  t5 = _mm_sub_ps (t0, t1);
  t6 = _mm_add_ps (t2, t3);
  t7 = IMULI(_mm_sub_ps (t2, t3));
  *r0 = _mm_add_ps (t4, t6);
  *r2 = _mm_sub_ps (t4, t6);
  *r1 = _mm_sub_ps (t5, t7);
  *r3 = _mm_add_ps (t5, t7);
}

__INLINE void 
K_0(__m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 uk, uk2, zk, zk_d;
	uk   = *r0; 
	uk2  = *r1;
  zk   = _mm_add_ps(*r2, *r3);
  zk_d = IMULI(_mm_sub_ps(*r2, *r3));
  *r0 = _mm_add_ps(uk, zk);
  *r2 = _mm_sub_ps(uk, zk);
  *r1 = _mm_sub_ps(uk2, zk_d);
  *r3 = _mm_add_ps(uk2, zk_d);
}

__INLINE __m128 IMUL(__m128 d, __m128 re, __m128 im) {
  re = _mm_mul_ps(re, d);                   
  im = _mm_mul_ps(im, _mm_shuffle_ps(d, d, _MM_SHUFFLE(2,3,0,1)));
  return _mm_sub_ps(re, im);  
}

__INLINE __m128 IMULJ(__m128 d, __m128 re, __m128 im) {
  re = _mm_mul_ps(re, d);                   
  im = _mm_mul_ps(im, _mm_shuffle_ps(d, d, _MM_SHUFFLE(2,3,0,1)));
  return _mm_add_ps(re, im);  
}

__INLINE void 
K_N(__m128 re, __m128 im, __m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
	__m128 uk, uk2, zk_p, zk_n, zk, zk_d;

	uk   = *r0;
	uk2  = *r1;
  zk_p = IMUL(*r2, re, im);
  zk_n = IMULJ(*r3, re, im);
  
	zk   = _mm_add_ps(zk_p, zk_n);
  zk_d = IMULI(_mm_sub_ps(zk_p, zk_n));

  *r2 = _mm_sub_ps(uk, zk);
  *r0 = _mm_add_ps(uk, zk);
  *r3 = _mm_add_ps(uk2, zk_d);
  *r1 = _mm_sub_ps(uk2, zk_d);
}

__INLINE void TX2(__m128 *a, __m128 *b) {
    __m128 TX2_t0 = _mm_shuffle_ps(*a, *b, _MM_SHUFFLE(1,0,1,0));
		__m128 TX2_t1 = _mm_shuffle_ps(*a, *b, _MM_SHUFFLE(3,2,3,2));
    *a = TX2_t0; *b = TX2_t1; 
}

__INLINE void 
LEAF_EE(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  __m128 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;

	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
  K_0(&r0,&r2,&r4,&r6);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r1,&r3,&r5,&r7);
  L_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8,&r9,&r10,&r11);
  L_4(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12,&r13,&r14,&r15);
  K_0(&r0,&r4,&r8,&r12);
  K_N(_mm_set_ps(0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011),_mm_set_ps(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0.38268343236508978177923268049199,-0.38268343236508978177923268049199),&r1,&r5,&r9,&r13);
  TX2(&r0,&r1);
  TX2(&r4,&r5);
  TX2(&r8,&r9);
  TX2(&r12,&r13);
  S_4(r0,r4,r8,r12,out0+0,out0+8,out0+16,out0+24);
  S_4(r1,r5,r9,r13,out1+0,out1+8,out1+16,out1+24);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r2,&r6,&r10,&r14);
  K_N(_mm_set_ps(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981),_mm_set_ps(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.92387953251128673848313610506011,-0.92387953251128673848313610506011),&r3,&r7,&r11,&r15);
  TX2(&r2,&r3);
  TX2(&r6,&r7);
  TX2(&r10,&r11);
  TX2(&r14,&r15);
  S_4(r2,r6,r10,r14,out0+4,out0+12,out0+20,out0+28);
  S_4(r3,r7,r11,r15,out1+4,out1+12,out1+20,out1+28);
	*is += 16;
}


__INLINE void 
LEAF_OO(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  __m128 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;
	
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0,&r1,&r2,&r3);
  L_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4,&r5,&r6,&r7);
  K_0(&r0,&r2,&r4,&r6);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r1,&r3,&r5,&r7);
  TX2(&r0,&r1);
  TX2(&r2,&r3);
  TX2(&r4,&r5);
  TX2(&r6,&r7);
  S_4(r0,r2,r4,r6,out0+0,out0+4,out0+8,out0+12);
  S_4(r1,r3,r5,r7,out1+0,out1+4,out1+8,out1+12);
  L_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8,&r9,&r10,&r11);
  L_2(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12,&r13,&r14,&r15);
  K_0(&r8,&r10,&r12,&r14);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r9,&r11,&r13,&r15);
  TX2(&r8,&r9);
  TX2(&r10,&r11);
  TX2(&r12,&r13);
  TX2(&r14,&r15);
  S_4(r8,r10,r12,r14,out0+16,out0+20,out0+24,out0+28);
  S_4(r9,r11,r13,r15,out1+16,out1+20,out1+24,out1+28);

	*is += 16;
}

__INLINE void 
L_4_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									__m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3, t4, t5, t6, t7;
	t0 = _mm_load_ps(i0);    t1 = _mm_load_ps(i1);    t2 = _mm_load_ps(i2);    t3 = _mm_load_ps(i3);    
	t4 = _mm_add_ps(t0, t1);
	t5 = _mm_sub_ps(t0, t1);
	t6 = _mm_add_ps(t2, t3);
	t7 = IMULI(_mm_sub_ps(t2, t3));
	t0 = _mm_add_ps(t4, t6);
	t2 = _mm_sub_ps(t4, t6);
	t1 = _mm_sub_ps(t5, t7);
	t3 = _mm_add_ps(t5, t7);
	TX2(&t0,&t1);
	TX2(&t2,&t3);
	*r0 = t0; *r2 = t1; *r1 = t2; *r3 = t3; }

__INLINE void 
L_2_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  __m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3, t4, t5, t6, t7;
	t0 = _mm_load_ps(i0);    t1 = _mm_load_ps(i1);    t2 = _mm_load_ps(i2);    t3 = _mm_load_ps(i3); t4 = _mm_add_ps(t0, t1);
	t5 = _mm_sub_ps(t0, t1);
	t6 = _mm_add_ps(t2, t3);
	t7 = _mm_sub_ps(t2, t3);
	TX2(&t4,&t5);
	TX2(&t6,&t7);
	*r0 = t4; *r2 = t5; *r1 = t6; *r3 = t7;
}

__INLINE void 
L_2_4(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  __m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = _mm_load_ps(i0);    t1 = _mm_load_ps(i1);    t2 = _mm_load_ps(i2);    t3 = _mm_load_ps(i3);    
  t4 = _mm_add_ps(t0, t1);
  t5 = _mm_sub_ps(t0, t1);
  t6 = _mm_add_ps(t2, t3);
  t7 = _mm_sub_ps(t2, t3);
  *r0 = _mm_shuffle_ps(t4, t5, _MM_SHUFFLE(1,0,1,0));
  *r1 = _mm_shuffle_ps(t6, t7, _MM_SHUFFLE(1,0,1,0));
  t5 = IMULI(t5);
  t0 = _mm_add_ps(t6, t4);
  t2 = _mm_sub_ps(t6, t4);
  t1 = _mm_sub_ps(t7, t5);
  t3 = _mm_add_ps(t7, t5);
  *r3 = _mm_shuffle_ps(t0, t1, _MM_SHUFFLE(3,2,3,2));
  *r2 = _mm_shuffle_ps(t2, t3, _MM_SHUFFLE(3,2,3,2));
}

__INLINE void 
L_4_2(const data_t * restrict i0, const data_t * restrict i1, const data_t * restrict i2, const data_t * restrict i3, 
									  __m128 *r0, __m128 *r1, __m128 *r2, __m128 *r3) {
  __m128 t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = _mm_load_ps(i0);    
  t1 = _mm_load_ps(i1);    
  t6 = _mm_load_ps(i2);    
  t7 = _mm_load_ps(i3);    
	t2 = _mm_shuffle_ps(t6, t7, _MM_SHUFFLE(3,2,1,0));
  t3 = _mm_shuffle_ps(t7, t6, _MM_SHUFFLE(3,2,1,0));
	t4 = _mm_add_ps(t0, t1);
  t5 = _mm_sub_ps(t0, t1);
  t6 = _mm_add_ps(t2, t3);
  t7 = _mm_sub_ps(t2, t3);
  *r2 = _mm_shuffle_ps(t4, t5, _MM_SHUFFLE(3,2,3,2));
  *r3 = _mm_shuffle_ps(t6, t7, _MM_SHUFFLE(3,2,3,2));
 	t7 = IMULI(t7);
  t0 = _mm_add_ps(t4, t6);
  t2 = _mm_sub_ps(t4, t6);
  t1 = _mm_sub_ps(t5, t7);
  t3 = _mm_add_ps(t5, t7);
  *r0 = _mm_shuffle_ps(t0, t1, _MM_SHUFFLE(1,0,1,0));
  *r1 = _mm_shuffle_ps(t2, t3, _MM_SHUFFLE(1,0,1,0));
}

__INLINE void 
LEAF_OE(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  __m128 r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;

	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;
  
	L_4_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r24_25,&r26_27);
  L_2_4(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r30_31,&r28_29);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,out0+0,out0+4,out0+8,out0+12);
  L_4_4(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8_9,&r10_11,&r16_17,&r18_19);
  L_2_2(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12_13,&r14_15,&r20_21,&r22_23);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r8_9,&r10_11,&r12_13,&r14_15);
  S_4(r8_9,r10_11,r12_13,r14_15,out0+16,out0+20,out0+24,out0+28);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r16_17,&r18_19,&r20_21,&r22_23);
  K_N(_mm_set_ps(0.92387953251128673848313610506011,0.92387953251128673848313610506011,1,1),_mm_set_ps(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0,-0),&r16_17,&r20_21,&r24_25,&r28_29);
  S_4(r16_17,r20_21,r24_25,r28_29,out1+0,out1+8,out1+16,out1+24);
  K_N(_mm_set_ps(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r18_19,&r22_23,&r26_27,&r30_31);
  S_4(r18_19,r22_23,r26_27,r30_31,out1+4,out1+12,out1+20,out1+28);

	*is += 16;
}

__INLINE void 
LEAF_EO(size_t ** restrict is, const data_t * restrict in, size_t ** restrict out_offsets, data_t * restrict out) {
  __m128 r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;
	
	data_t *out0 = out + (*out_offsets)[0];
	data_t *out1 = out + (*out_offsets)[1];
	*out_offsets += 2;

  L_4_4(in+(*is)[0],in+(*is)[1],in+(*is)[2],in+(*is)[3],&r0_1,&r2_3,&r16_17,&r18_19);
  L_2_2(in+(*is)[4],in+(*is)[5],in+(*is)[6],in+(*is)[7],&r4_5,&r6_7,&r20_21,&r22_23);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r0_1,&r2_3,&r4_5,&r6_7);
  L_4_2(in+(*is)[8],in+(*is)[9],in+(*is)[10],in+(*is)[11],&r8_9,&r10_11,&r28_29,&r30_31);
  L_4_4(in+(*is)[12],in+(*is)[13],in+(*is)[14],in+(*is)[15],&r12_13,&r14_15,&r24_25,&r26_27);
  K_N(_mm_set_ps(0.92387953251128673848313610506011,0.92387953251128673848313610506011,1,1),_mm_set_ps(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0,-0),&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,out0+0,out0+8,out0+16,out0+24);
  K_N(_mm_set_ps(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.70710678118654757273731092936941,0.70710678118654757273731092936941),_mm_set_ps(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.70710678118654746171500846685376,-0.70710678118654746171500846685376),&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,out0+4,out0+12,out0+20,out0+28);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r16_17,&r18_19,&r20_21,&r22_23);
  S_4(r16_17,r18_19,r20_21,r22_23,out1+0,out1+4,out1+8,out1+12);
  K_N(_mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1),_mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0),&r24_25,&r26_27,&r28_29,&r30_31);
  S_4(r24_25,r26_27,r28_29,r30_31,out1+16,out1+20,out1+24,out1+28);

	*is += 16;
}


#endif
