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
#ifndef __CP_SSE_H__
#define __CP_SSE_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
//#include <stdalign.h>

//#include "codegen.h"
#include "types.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209

static const __attribute__ ((aligned(64))) float w_data[16] = {0.70710678118654757273731092936941,0.70710678118654746171500846685376, -0.70710678118654757273731092936941,-0.70710678118654746171500846685376,
																								  1.0f,0.70710678118654757273731092936941f, -0.0f,-0.70710678118654746171500846685376,
																									0.70710678118654757273731092936941,0.70710678118654746171500846685376, 0.70710678118654757273731092936941,0.70710678118654746171500846685376,
																									1.0f,0.70710678118654757273731092936941f, 0.0f,0.70710678118654746171500846685376};

__INLINE float W_re(float N, float k) { return cos(-2.0f * PI * k / N); }
__INLINE float W_im(float N, float k) { return sin(-2.0f * PI * k / N); }

typedef size_t transform_index_t;

//typedef void (*transform_func_t)(float *data, size_t N, float *LUT);
typedef void (*transform_func_t)(float *data, size_t N, float *LUT);

typedef struct _ffts_plan_t ffts_plan_t;

struct _ffts_plan_t {
	ptrdiff_t *offsets;
	void  *ws;
	void  *oe_ws, *eo_ws, *ee_ws;
	ptrdiff_t *is;
	size_t *ws_is;
	size_t i0, i1, n_luts;
	size_t N;
	void *lastlut;
	transform_index_t *transforms; 
	//transform_func_t transform;
	void (*transform)(ffts_plan_t * , const void * , void * );
	void *transform_base;
	size_t transform_size;
	void *constants;
	
	// multi-dimensional stuff:
	struct _ffts_plan_t **plans;
	int rank;
	size_t *Ns, *Ms;
	void *buf;

	void *transpose_buf;

	void (*destroy)(ffts_plan_t *);

	float *A, *B;
};


void ffts_free(ffts_plan_t *);
ffts_plan_t *ffts_init_1d(size_t N, int sign); 
void ffts_execute(ffts_plan_t *, const void *, void *);
#endif
