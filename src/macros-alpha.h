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

#ifndef __MACROS_ALPHA_H__
#define __MACROS_ALPHA_H__

#include <math.h>

#ifdef __alpha__
#define restrict
#endif

typedef struct {float r1, i1, r2, i2;} V;

#define FFTS_MALLOC(d,a) malloc(d)
#define FFTS_FREE(d) free(d)

#define VLIT4(f3,f2,f1,f0) ((V){f0,f1,f2,f3})

static inline V VADD(V x, V y)
{
    V z;
    z.r1 = x.r1 + y.r1;
    z.i1 = x.i1 + y.i1;
    z.r2 = x.r2 + y.r2;
    z.i2 = x.i2 + y.i2;
    return z;
}


static inline V VSUB(V x, V y)
{
    V z;
    z.r1 = x.r1 - y.r1;
    z.i1 = x.i1 - y.i1;
    z.r2 = x.r2 - y.r2;
    z.i2 = x.i2 - y.i2;
    return z;
}


static inline V VMUL(V x, V y)
{
    V z;
    z.r1 = x.r1 * y.r1;
    z.i1 = x.i1 * y.i1;
    z.r2 = x.r2 * y.r2;
    z.i2 = x.i2 * y.i2;
    return z;
}

static inline V VXOR(V x, V y)
{
    V r;
    r.r1 = (uint32_t)x.r1 ^ (uint32_t)y.r1;
    r.i1 = (uint32_t)x.i1 ^ (uint32_t)y.i1;
    r.r2 = (uint32_t)x.r2 ^ (uint32_t)y.r2;
    r.i2 = (uint32_t)x.i2 ^ (uint32_t)y.i2;
    return r;
}

static inline V VSWAPPAIRS(V x)
{
    V z;
    z.r1 = x.i1;
    z.i1 = x.r1;
    z.r2 = x.i2;
    z.i2 = x.r2;
    return z;
}


static inline V VBLEND(V x, V y)
{
    V z;
    z.r1 = x.r1;
    z.i1 = x.i1;
    z.r2 = y.r2;
    z.i2 = y.i2;
    return z;
}

static inline V VUNPACKHI(V x, V y)
{
    V z;
    z.r1 = x.r2;
    z.i1 = x.i2;
    z.r2 = y.r2;
    z.i2 = y.i2;
    return z;
}

static inline V VUNPACKLO(V x, V y)
{
    V z;
    z.r1 = x.r1;
    z.i1 = x.i1;
    z.r2 = y.r1;
    z.i2 = y.i1;
    return z;
}

static inline V VDUPRE(V x)
{
    V z;
    z.r1 = x.r1;
    z.i1 = x.r1;
    z.r2 = x.r2;
    z.i2 = x.r2;
    return z;
}

static inline V VDUPIM(V x)
{
    V z;
    z.r1 = x.i1;
    z.i1 = x.i1;
    z.r2 = x.i2;
    z.i2 = x.i2;
    return z;
}

static inline V IMUL(V d, V re, V im)
{
    re = VMUL(re, d);
    im = VMUL(im, VSWAPPAIRS(d));
    return VSUB(re, im);  
}


static inline V IMULJ(V d, V re, V im)
{
    re = VMUL(re, d);
    im = VMUL(im, VSWAPPAIRS(d));
    return VADD(re, im);
}

static inline V MULI(int inv, V x)
{
    V z;

    if (inv) {
	z.r1 = -x.r1;
	z.i1 = x.i1;
	z.r2 = -x.r2;
	z.i2 = x.i2;
    }else{
	z.r1 = x.r1;
	z.i1 = -x.i1;
	z.r2 = x.r2;
	z.i2 = -x.i2;
    }
    return z;
}


static inline V IMULI(int inv, V x)
{
    return VSWAPPAIRS(MULI(inv, x));
}


static inline V VLD(const void *s)
{
    V *d = (V *)s;
    return *d;
}


static inline void VST(void *d, V s)
{
    V *r = (V *)d;
    *r = s;
}

#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
