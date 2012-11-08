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

#include "ffts_real.h"

void ffts_free_1d_real(ffts_plan_t *p) {
	ffts_free(p->plans[0]);
	free(p->A);
	free(p->B);
	free(p->plans);
	free(p->buf);
	free(p);
}

void ffts_execute_1d_real(ffts_plan_t *p, const void *vin, void *vout) {
	float *out = (float *)vout;
	float *buf = (float *)p->buf;
	float *A = p->A;
	float *B = p->B;

	p->plans[0]->transform(p->plans[0], vin, buf);

	size_t N = p->N;
	buf[N] = buf[0];
	buf[N+1] = buf[1];

	size_t i;
	for(i=0;i<N/2;i++) {
		out[2*i]   = buf[2*i]*A[2*i] - buf[2*i+1]*A[2*i+1] + buf[N-2*i]*B[2*i] + buf[N-2*i+1]*B[2*i+1];
		out[2*i+1] = buf[2*i+1]*A[2*i] + buf[2*i]*A[2*i+1] + buf[N-2*i]*B[2*i+1] - buf[N-2*i+1]*B[2*i];
		out[2*N - 2*i] = out[2*i];
		out[2*N - 2*i + 1] = -out[2*i+1];
	}
	
	out[N] = buf[0] - buf[1];
	out[N+1] = 0.0f;
	
}

void ffts_execute_1d_real_inv(ffts_plan_t *p, const void *vin, void *vout) {
	float *out = (float *)vout;
	float *in = (float *)vin;
	float *buf = (float *)p->buf;
	float *A = p->A;
	float *B = p->B;
	size_t N = p->N;
	
	size_t i;
	for(i=0;i<N/2;i++) {
		buf[2*i]   = in[2*i]*A[2*i] + in[2*i+1]*A[2*i+1] + in[N-2*i]*B[2*i] - in[N-2*i+1]*B[2*i+1];
		buf[2*i+1] = in[2*i+1]*A[2*i] - in[2*i]*A[2*i+1] - in[N-2*i]*B[2*i+1] - in[N-2*i+1]*B[2*i];
	}
	
	p->plans[0]->transform(p->plans[0], buf, out);
	
}

ffts_plan_t *ffts_init_1d_real(size_t N, int sign) {
	ffts_plan_t *p = malloc(sizeof(ffts_plan_t));

	if(sign < 0) p->transform = &ffts_execute_1d_real;
	else         p->transform = &ffts_execute_1d_real_inv;
	
	p->destroy = &ffts_free_1d_real;
	p->N = N;
	p->rank = 1;
	p->plans = malloc(sizeof(ffts_plan_t **) * 1);

	p->plans[0] = ffts_init_1d(N/2, sign); 

	p->buf = valloc(sizeof(float) * 2 * (N/2) + 1);

	p->A = valloc(sizeof(float) * N);
	p->B = valloc(sizeof(float) * N);

	int i;
	for (i = 0; i < N/2; i++) {
		p->A[2 * i]     = 0.5 * (1.0 - sin (2 * PI / (double) N * (double) i));
		p->A[2 * i + 1] = 0.5 * (-1.0 * cos (2 * PI / (double) N * (double) i));
		p->B[2 * i]     = 0.5 * (1.0 + sin (2 * PI / (double) N * (double) i));
		p->B[2 * i + 1] = 0.5 * (1.0 * cos (2 * PI / (double) N * (double) i));
	}
	
	return p;
}


