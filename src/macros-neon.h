/*
 
 This file is part of FFTS -- The Fastest Fourier Transform in the South
  
 Copyright (c) 2012, 2013, Anthony M. Blake <amb@anthonix.com>
 
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
#ifndef __MACROS_NEON_H__
#define __MACROS_NEON_H__

#include "neon.h"
#include <arm_neon.h>

typedef float32x4_t V;

typedef float32x4x2_t VS;

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

__INLINE void STORESPR(data_t * addr,  VS p) {

	vst1q_f32(addr, p.val[0]);
	vst1q_f32(addr + 4, p.val[1]);
	
}

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
