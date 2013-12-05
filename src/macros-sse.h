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

#ifndef __SSE_FLOAT_H__
#define __SSE_FLOAT_H__

#include <xmmintrin.h>

//#define VL 4

typedef __m128 V;

#define VADD _mm_add_ps
#define VSUB _mm_sub_ps
#define VMUL _mm_mul_ps
//#define VLIT4 _mm_set_ps
#define VXOR _mm_xor_ps
#define VST _mm_store_ps
#define VLD _mm_load_ps

#define VSWAPPAIRS(x) (_mm_shuffle_ps(x,x,_MM_SHUFFLE(2,3,0,1)))

#define VUNPACKHI(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(3,2,3,2)))
#define VUNPACKLO(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(1,0,1,0)))

#define VBLEND(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(3,2,1,0)))

#define VLIT4 _mm_set_ps

#define VDUPRE(r) (_mm_shuffle_ps(r,r,_MM_SHUFFLE(2,2,0,0)))
#define VDUPIM(r) (_mm_shuffle_ps(r,r,_MM_SHUFFLE(3,3,1,1)))

#define FFTS_MALLOC(d,a) (_mm_malloc(d,a))
#define FFTS_FREE(d) (_mm_free(d))

__INLINE V IMULI(int inv, V a) {
	if(inv) return VSWAPPAIRS(VXOR(a, VLIT4(0.0f, -0.0f, 0.0f, -0.0f)));
	else    return VSWAPPAIRS(VXOR(a, VLIT4(-0.0f, 0.0f, -0.0f, 0.0f)));
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

#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
