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

#define POSITIVE_SIGN 1
#define NEGATIVE_SIGN -1

struct _ffts_plan_t;
typedef struct _ffts_plan_t ffts_plan_t;

// Complex data is stored in the interleaved format (i.e, the real and imaginary 
// parts composing each element of complex data are stored adjacently in memory)
ffts_plan_t *ffts_init_1d(size_t N, int sign);
ffts_plan_t *ffts_init_2d(size_t N1, size_t N2, int sign);
ffts_plan_t *ffts_init_nd(int rank, size_t *Ns, int sign);

// For real transforms, sign == -1 implies a real-to-complex forwards tranform,
// and sign == 1 implies a complex-to-real backwards transform
// The output of a real-to-complex transform is N/2+1 complex numbers, where the
// redundant outputs have been omitted.
ffts_plan_t *ffts_init_1d_real(size_t N, int sign);
ffts_plan_t *ffts_init_2d_real(size_t N1, size_t N2, int sign);
ffts_plan_t *ffts_init_nd_real(int rank, size_t *Ns, int sign);

void ffts_execute(ffts_plan_t * , const void *input, void *output);
void ffts_free(ffts_plan_t *);

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
