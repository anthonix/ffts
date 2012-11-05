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

#include "ffts_nd.h"


void ffts_free_nd(ffts_plan_t *p) {
	free(p->Ns);
	free(p->Ms);

	int i;
	for(i=0;i<p->rank;i++) {
		ffts_free(p->plans[i]);
	}

	free(p->plans);
	free(p->buf);
	free(p);
}

inline void ffts_transpose(uint64_t *in, uint64_t *out, int w, int h) {

	size_t i,j;

	for(i=0;i<w;i+=2) {
		for(j=0;j<h;j+=2) {
#ifdef __ARM_NEON__
//			out[i*h + j] = in[j*w + i];
			float32x4_t Q0 = vld1q_f32(in + j*w + i);
			float32x4_t Q1 = vld1q_f32(in + j*w + i + w);

			float32x2x2_t t0;
			float32x2x2_t t1;
			t0.val[0] = vget_low_f32(Q0);
			t0.val[1] = vget_high_f32(Q0);
			t1.val[0] = vget_low_f32(Q1);
			t1.val[1] = vget_high_f32(Q1);

			__asm__ ("vswp %0,%1\n\t"
					: "+w" (t0.val[1]), "+w" (t1.val[0])
					:
					);

			Q0 = vcombine_f32(t0.val[0], t0.val[1]);
			Q1 = vcombine_f32(t1.val[0], t1.val[1]);
			vst1q_f32(out + i*h + j, Q0);
			vst1q_f32(out + i*h + j + h, Q1);
#else
			__m128d q0 = _mm_load_pd(in + j*w + i);
			__m128d q1 = _mm_load_pd(in + j*w + i + w);
			__m128d t0 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(0, 0));
			__m128d t1 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(1, 1));
			_mm_store_pd(out + i*h + j, t0);
			_mm_store_pd(out + i*h + j + h, t1);
#endif
		}
	}
}

void ffts_execute_nd(ffts_plan_t *p, const void *  in, void *  out) {

	uint64_t *din = in;
	uint64_t *buf = p->buf;
	uint64_t *dout = out;

	size_t i,j;
	for(i=0;i<p->Ns[0];i++) {
		ffts_execute(p->plans[0], din + (i * p->Ms[0]), buf + (i * p->Ms[0]));	
	}
	ffts_transpose(buf, out, p->Ms[0], p->Ns[0]);	

	for(i=1;i<p->rank;i++) {
		for(j=0;j<p->Ns[i];j++) { 
			ffts_execute(p->plans[i], dout + (j * p->Ms[i]), buf + (j * p->Ms[i]));	
		}
		ffts_transpose(buf, dout, p->Ms[i], p->Ns[i]);	
	}
}

ffts_plan_t *ffts_init_nd(int rank, size_t *Ns, int sign) {
	size_t vol = 1;

	ffts_plan_t *p = malloc(sizeof(ffts_plan_t));

	p->transform = &ffts_execute_nd;
	p->destroy = &ffts_free_nd;

	p->rank = rank;
	p->Ns = malloc(sizeof(size_t) * rank);
	p->Ms = malloc(sizeof(size_t) * rank);
	p->plans = malloc(sizeof(ffts_plan_t **) * rank);
	int i;
	for(i=0;i<rank;i++) {
		p->Ns[i] = Ns[i];
		vol *= Ns[i];	
	}
	p->buf = malloc(sizeof(float) * 2 * vol);

	for(i=0;i<rank;i++) {
		p->Ms[i] = vol / p->Ns[i];
		p->plans[i] = ffts_init_1d(p->Ms[i], sign); 
	}

	return p;
}


ffts_plan_t *ffts_init_2d(size_t N1, size_t N2, int sign) {
	size_t Ns[2];
	Ns[0] = N1;
	Ns[1] = N2;
	return ffts_init_nd(2, Ns, sign);
}
