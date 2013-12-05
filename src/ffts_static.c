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
#include "ffts_static.h"

void ffts_static_rec_i(ffts_plan_t *p, float *data, size_t N) {
	if(N > 16) {
		size_t N1 = N >> 1;
		size_t N2 = N >> 2;
		size_t N3 = N >> 3;
		float *ws = ((float *)(p->ws)) + (p->ws_is[__builtin_ctzl(N)-4] << 1);

		ffts_static_rec_i(p, data, N2);
		ffts_static_rec_i(p, data + N1, N3);
		ffts_static_rec_i(p, data + N1 + N2, N3);
		ffts_static_rec_i(p, data + N, N2);
		ffts_static_rec_i(p, data + N + N1, N2);

  	if(N == p->N) {
  		neon_static_x8_t_i(data, N, ws);
  	}else{
  		neon_static_x8_i(data, N, ws); 
  	}

	}else if(N==16){
	  neon_static_x4_i(data, N, p->ws);
	}

}
void ffts_static_rec_f(ffts_plan_t *p, float *data, size_t N) {
	if(N > 16) {
		size_t N1 = N >> 1;
		size_t N2 = N >> 2;
		size_t N3 = N >> 3;
		float *ws = ((float *)(p->ws)) + (p->ws_is[__builtin_ctzl(N)-4] << 1);

		ffts_static_rec_f(p, data, N2);
		ffts_static_rec_f(p, data + N1, N3);
		ffts_static_rec_f(p, data + N1 + N2, N3);
		ffts_static_rec_f(p, data + N, N2);
		ffts_static_rec_f(p, data + N + N1, N2);

  	if(N == p->N) {
  		neon_static_x8_t_f(data, N, ws);
  	}else{
  		neon_static_x8_f(data, N, ws); 
  	}

	}else if(N==16){
	  neon_static_x4_f(data, N, p->ws);
	}

}

void ffts_static_transform_f(ffts_plan_t *p, const void *in, void *out) {

	if(__builtin_ctzl(p->N) & 1) 
		neon_static_o_f(p, in, out);
	else 
		neon_static_e_f(p, in, out);
	ffts_static_rec_f(p, out, p->N);
}


void ffts_static_transform_i(ffts_plan_t *p, const void *in, void *out) {

	if(__builtin_ctzl(p->N) & 1) 
		neon_static_o_i(p, in, out);
	else 
		neon_static_e_i(p, in, out);
	ffts_static_rec_i(p, out, p->N);
}
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
