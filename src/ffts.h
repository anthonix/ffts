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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
//#include <stdalign.h>

//#include "codegen.h"
#include "types.h"


#ifdef __ANDROID__
#include <android/log.h>
#define LOG(s) __android_log_print(ANDROID_LOG_ERROR, "FFTS", s)
#else
#define LOG(s) fprintf(stderr, s)
#endif

#define PI 3.1415926535897932384626433832795028841971693993751058209

static const __attribute__ ((aligned(64))) float w_data[16] = {
	0.70710678118654757273731092936941,		0.70710678118654746171500846685376,
	-0.70710678118654757273731092936941,	-0.70710678118654746171500846685376,
    1.0f,									 0.70710678118654757273731092936941f,
	-0.0f,									-0.70710678118654746171500846685376,
	0.70710678118654757273731092936941,		0.70710678118654746171500846685376,
	0.70710678118654757273731092936941,		0.70710678118654746171500846685376,
	1.0f,									0.70710678118654757273731092936941f,
	0.0f,									0.70710678118654746171500846685376
};

__INLINE float W_re(float N, float k) { return cos(-2.0f * PI * k / N); }
__INLINE float W_im(float N, float k) { return sin(-2.0f * PI * k / N); }

typedef size_t transform_index_t;

//typedef void (*transform_func_t)(float *data, size_t N, float *LUT);
typedef void (*transform_func_t)(float *data, size_t N, float *LUT);

typedef struct _ffts_plan_t ffts_plan_t;

/**
 * Contains all the Information need to perform FFT
 *
 *
 * DO NOT CHANGE THE ORDER OF MEMBERS
 * ASSEMBLY CODE USES HARD CODED OFFSETS TO REFERENCE
 * SOME OF THESE VARIABES!!
 */
struct _ffts_plan_t {

	/**
	 *
	 */
	ptrdiff_t *offsets;
#ifdef DYNAMIC_DISABLED
	/**
	 * Twiddle factors
	 */
	void *ws;
	/**
	 * ee - 2 size x  size8
	 * oo - 2 x size4 in parallel
	 * oe -
	 */
	void  *oe_ws, *eo_ws, *ee_ws;
#else
	void __attribute__((aligned(32))) *ws;
	void __attribute__((aligned(32)))  *oe_ws, *eo_ws, *ee_ws;
#endif
	/**
	 * Pointer into an array of precomputed indexes for the input data array
	 */
	ptrdiff_t *is;

	/**
	 * Twiddle Factor Indexes
	 */
	size_t *ws_is;

	/**
	 * Size of the loops for the base cases
	 */
	size_t i0, i1, n_luts;

	/**
	 * Size fo the Transform
	 */
	size_t N;
	void *lastlut;
	/**
	 * Used in multidimensional Code ??
	 */
	transform_index_t *transforms;
	//transform_func_t transform;

	/**
	 * Pointer to the dynamically generated function
	 * that will execute the FFT
	 */
	void (*transform)(ffts_plan_t * , const void * , void * );

	/**
	 * Pointer to the base memory address of
	 * of the transform function
	 */
	void *transform_base;

	/**
	 * Size of the memory block contain the
	 * generated code
	 */
	size_t transform_size;

	/**
	 * Points to the cosnant variables used by
	 * the Assembly Code
	 */
	void *constants;

	// multi-dimensional stuff:
	struct _ffts_plan_t **plans;
	int rank;
	size_t *Ns, *Ms;
	void *buf;

	void *transpose_buf;

	/**
	 * Pointer to the destroy function
	 * to clean up the plan after use
	 * (differs for real and multi dimension transforms
	 */
	void (*destroy)(ffts_plan_t *);

	/**
	 * Coefficiants for the real valued transforms
	 */
	float *A, *B;

	size_t i2;
};


void ffts_free(ffts_plan_t *);
ffts_plan_t *ffts_init_1d(size_t N, int sign);
void ffts_execute(ffts_plan_t *, const void *, void *);
#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
