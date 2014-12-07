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

#ifndef FFTS_MACROS_ALPHA_H
#define FFTS_MACROS_ALPHA_H

#include <string.h>

typedef union {
    struct {
        float r1;
        float i1;
        float r2;
        float i2;
    } r;
    uint32_t u[4];
} V;

#define FFTS_MALLOC(d,a) malloc(d)
#define FFTS_FREE(d) free(d)

static FFTS_ALWAYS_INLINE V VLIT4(float f3, float f2, float f1, float f0)
{
    V z;

    z.r.r1 = f0;
    z.r.i1 = f1;
    z.r.r2 = f2;
    z.r.i2 = f3;

    return z;
}

static FFTS_ALWAYS_INLINE V VADD(V x, V y)
{
    V z;

    z.r.r1 = x.r.r1 + y.r.r1;
    z.r.i1 = x.r.i1 + y.r.i1;
    z.r.r2 = x.r.r2 + y.r.r2;
    z.r.i2 = x.r.i2 + y.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V VSUB(V x, V y)
{
    V z;

    z.r.r1 = x.r.r1 - y.r.r1;
    z.r.i1 = x.r.i1 - y.r.i1;
    z.r.r2 = x.r.r2 - y.r.r2;
    z.r.i2 = x.r.i2 - y.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V VMUL(V x, V y)
{
    V z;

    z.r.r1 = x.r.r1 * y.r.r1;
    z.r.i1 = x.r.i1 * y.r.i1;
    z.r.r2 = x.r.r2 * y.r.r2;
    z.r.i2 = x.r.i2 * y.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V VXOR(V x, V y)
{
    V z;

    z.u[0] = x.u[0] ^ y.u[0];
    z.u[1] = x.u[1] ^ y.u[1];
    z.u[2] = x.u[2] ^ y.u[2];
    z.u[3] = x.u[3] ^ y.u[3];

    return z;
}

static FFTS_ALWAYS_INLINE V VSWAPPAIRS(V x)
{
    V z;

    z.r.r1 = x.r.i1;
    z.r.i1 = x.r.r1;
    z.r.r2 = x.r.i2;
    z.r.i2 = x.r.r2;

    return z;
}

static FFTS_ALWAYS_INLINE V VBLEND(V x, V y)
{
    V z;

    z.r.r1 = x.r.r1;
    z.r.i1 = x.r.i1;
    z.r.r2 = y.r.r2;
    z.r.i2 = y.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V VUNPACKHI(V x, V y)
{
    V z;

    z.r.r1 = x.r.r2;
    z.r.i1 = x.r.i2;
    z.r.r2 = y.r.r2;
    z.r.i2 = y.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V VUNPACKLO(V x, V y)
{
    V z;

    z.r.r1 = x.r.r1;
    z.r.i1 = x.r.i1;
    z.r.r2 = y.r.r1;
    z.r.i2 = y.r.i1;

    return z;
}

static FFTS_ALWAYS_INLINE V VDUPRE(V x)
{
    V z;

    z.r.r1 = x.r.r1;
    z.r.i1 = x.r.r1;
    z.r.r2 = x.r.r2;
    z.r.i2 = x.r.r2;

    return z;
}

static FFTS_ALWAYS_INLINE V VDUPIM(V x)
{
    V z;

    z.r.r1 = x.r.i1;
    z.r.i1 = x.r.i1;
    z.r.r2 = x.r.i2;
    z.r.i2 = x.r.i2;

    return z;
}

static FFTS_ALWAYS_INLINE V IMUL(V d, V re, V im)
{
    re = VMUL(re, d);
    im = VMUL(im, VSWAPPAIRS(d));
    return VSUB(re, im);
}

static FFTS_ALWAYS_INLINE V IMULJ(V d, V re, V im)
{
    re = VMUL(re, d);
    im = VMUL(im, VSWAPPAIRS(d));
    return VADD(re, im);
}

static FFTS_ALWAYS_INLINE V MULI(int inv, V x)
{
    V z;

    if (inv) {
        z.r.r1 = -x.r.r1;
        z.r.i1 =  x.r.i1;
        z.r.r2 = -x.r.r2;
        z.r.i2 =  x.r.i2;
    } else {
        z.r.r1 =  x.r.r1;
        z.r.i1 = -x.r.i1;
        z.r.r2 =  x.r.r2;
        z.r.i2 = -x.r.i2;
    }

    return z;
}

static FFTS_ALWAYS_INLINE V IMULI(int inv, V x)
{
    return VSWAPPAIRS(MULI(inv, x));
}

static FFTS_ALWAYS_INLINE V VLD(const void *s)
{
    V z;
    memcpy(&z, s, sizeof(z));
    return z;
}

static FFTS_ALWAYS_INLINE void VST(void *d, V s)
{
    V *r = (V*) d;
    *r = s;
}

#endif /* FFTS_MACROS_ALPHA_H */