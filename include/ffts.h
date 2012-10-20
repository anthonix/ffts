/*
 
 This file is part of FFTS.
  
 Copyright (c) 2012, Anthony M. Blake
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

#ifndef __FFTS_H__
#define __FFTS_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
typedef size_t transform_index_t;
struct _ffts_plan_t {
	ptrdiff_t *offsets;
	void __attribute__ ((aligned(32))) *ws;
	void __attribute__ ((aligned(32))) *oe_ws, *eo_ws, *ee_ws;
	ptrdiff_t *is;
	size_t *ws_is;
	size_t i0, i1, n_luts;
	size_t N;
	void *lastlut;
	transform_index_t *transforms; 
	//transform_func_t transform;
	void (*transform)(struct _ffts_plan_t * , const float * , float * );
	void *transform_base;
	size_t transform_size;
	void *constants;
};
*/

struct _ffts_plan_t;
typedef struct _ffts_plan_t ffts_plan_t;

void ffts_execute(ffts_plan_t * , const void * , const void * );
ffts_plan_t *ffts_init(size_t N, int sign);
void ffts_free(ffts_plan_t *);

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
