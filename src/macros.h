/*
 
 This file is part of FFTS -- The Fastest Fourier Transform in the South
  
 Copyright (c) 2013, Michael J. Cree <mcree@orcon.net.nz> 
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

#ifndef __MACROS_H__
#define __MACROS_H__

#ifdef HAVE_NEON
#include "macros-neon.h"
#else
#ifdef __alpha__
#include "macros-alpha.h"
#else
#ifdef __powerpc__
#include "macros-altivec.h"
#endif
#endif

#endif


#ifdef HAVE_VFP
#include "macros-alpha.h"
#endif
#ifdef HAVE_SSE
	#include "macros-sse.h"
#endif

static inline void TX2(V *a, V *b)
{
    V TX2_t0 = VUNPACKLO(*a, *b);
    V TX2_t1 = VUNPACKHI(*a, *b);
    *a = TX2_t0; *b = TX2_t1; 
}

static inline void K_N(int inv, V re, V im, V *r0, V *r1, V *r2, V *r3)
{
    V uk, uk2, zk_p, zk_n, zk, zk_d;
    uk   = *r0; uk2  = *r1;
    zk_p = IMUL(*r2, re, im);
    zk_n = IMULJ(*r3, re, im);

    zk   = VADD(zk_p, zk_n);
    zk_d = IMULI(inv, VSUB(zk_p, zk_n));
  
    *r2 = VSUB(uk, zk);
    *r0 = VADD(uk, zk);
    *r3 = VADD(uk2, zk_d);
    *r1 = VSUB(uk2, zk_d);
}


static inline void S_4(V r0, V r1, V r2, V r3, 
		       data_t * restrict o0, data_t * restrict o1,
		       data_t * restrict o2, data_t * restrict o3)
{
    VST(o0, r0); VST(o1, r1); VST(o2, r2); VST(o3, r3);
}


static inline void L_2_4(int inv, 
			 const data_t * restrict i0, const data_t * restrict i1,
			 const data_t * restrict i2, const data_t * restrict i3,
			 V *r0, V *r1, V *r2, V *r3)
{
    V t0, t1, t2, t3, t4, t5, t6, t7;

    t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);    
    t4 = VADD(t0, t1);
    t5 = VSUB(t0, t1);
    t6 = VADD(t2, t3);
    t7 = VSUB(t2, t3);
    *r0 = VUNPACKLO(t4, t5);
    *r1 = VUNPACKLO(t6, t7);
    t5 = IMULI(inv, t5);
    t0 = VADD(t6, t4);
    t2 = VSUB(t6, t4);
    t1 = VSUB(t7, t5);
    t3 = VADD(t7, t5);
    *r3 = VUNPACKHI(t0, t1);
    *r2 = VUNPACKHI(t2, t3);
}


static inline void L_4_4(int inv,  
			 const data_t * restrict i0, const data_t * restrict i1,
			 const data_t * restrict i2, const data_t * restrict i3,
			 V *r0, V *r1, V *r2, V *r3)
{
    V t0, t1, t2, t3, t4, t5, t6, t7;
 
    t0 = VLD(i0);    t1 = VLD(i1);    t2 = VLD(i2);    t3 = VLD(i3);   
    t4 = VADD(t0, t1);
    t5 = VSUB(t0, t1);
    t6 = VADD(t2, t3);
    t7 = IMULI(inv, VSUB(t2, t3));
    t0 = VADD(t4, t6);
    t2 = VSUB(t4, t6);
    t1 = VSUB(t5, t7);
    t3 = VADD(t5, t7);
    TX2(&t0, &t1);
    TX2(&t2, &t3);
    *r0 = t0; *r2 = t1; *r1 = t2; *r3 = t3; 
}



static inline void L_4_2(int inv,  
			 const data_t * restrict i0, const data_t * restrict i1,
			 const data_t * restrict i2, const data_t * restrict i3,
			 V *r0, V *r1, V *r2, V *r3)
{
    V t0, t1, t2, t3, t4, t5, t6, t7;

    t0 = VLD(i0);    t1 = VLD(i1);    t6 = VLD(i2);    t7 = VLD(i3);
    t2 = VBLEND(t6, t7);
    t3 = VBLEND(t7, t6);
    t4 = VADD(t0, t1);
    t5 = VSUB(t0, t1);
    t6 = VADD(t2, t3);
    t7 = VSUB(t2, t3);
    *r2 = VUNPACKHI(t4, t5);
    *r3 = VUNPACKHI(t6, t7); 
    t7 = IMULI(inv, t7);
    t0 = VADD(t4, t6);
    t2 = VSUB(t4, t6);
    t1 = VSUB(t5, t7);
    t3 = VADD(t5, t7);
    *r0 = VUNPACKLO(t0, t1);
    *r1 = VUNPACKLO(t2, t3);
}
#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
