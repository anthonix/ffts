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
#include "ffts_internal.h"
#include "macros.h"

#include <assert.h>

static const FFTS_ALIGN(16) data_t ffts_constants[16] = {
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    -0.70710678118654746171500846685376, 0.70710678118654746171500846685376,
    -0.70710678118654746171500846685376, 0.70710678118654746171500846685376,
    1.0,                                 1.0,
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    0.0,                                 0.0,
    -0.70710678118654746171500846685376, 0.70710678118654746171500846685376
};

static const FFTS_ALIGN(16) data_t ffts_constants_inv[16] = {
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    0.70710678118654746171500846685376, -0.70710678118654746171500846685376,
    0.70710678118654746171500846685376, -0.70710678118654746171500846685376,
    1.0,                                 1.0,
    0.70710678118654757273731092936941,  0.70710678118654757273731092936941,
    0.0,                                 0.0,
    0.70710678118654746171500846685376, -0.70710678118654746171500846685376
};

static FFTS_INLINE void K_0(int inv, V *r0, V *r1, V *r2, V *r3)
{
    V t0, t1, t2, t3;

    t0 = *r0;
    t1 = *r1;

    t2 = VADD(*r2, *r3);
    t3 = IMULI(inv, VSUB(*r2, *r3));

    *r0 = VADD(t0, t2);
    *r2 = VSUB(t0, t2);
    *r1 = VSUB(t1, t3);
    *r3 = VADD(t1, t3);
}

static FFTS_INLINE void L_2(const data_t *FFTS_RESTRICT i0,
                            const data_t *FFTS_RESTRICT i1,
                            const data_t *FFTS_RESTRICT i2,
                            const data_t *FFTS_RESTRICT i3,
                            V *r0,
                            V *r1,
                            V *r2,
                            V *r3)
{
    V t0, t1, t2, t3;

    t0 = VLD(i0);
    t1 = VLD(i1);
    t2 = VLD(i2);
    t3 = VLD(i3);

    *r0 = VADD(t0, t1);
    *r1 = VSUB(t0, t1);
    *r2 = VADD(t2, t3);
    *r3 = VSUB(t2, t3);
}

static FFTS_INLINE void L_4(int inv,
                            const data_t *FFTS_RESTRICT i0,
                            const data_t *FFTS_RESTRICT i1,
                            const data_t *FFTS_RESTRICT i2,
                            const data_t *FFTS_RESTRICT i3,
                            V *r0,
                            V *r1,
                            V *r2,
                            V *r3)
{
    V t0, t1, t2, t3, t4, t5, t6, t7;

    t0 = VLD(i0);
    t1 = VLD(i1);
    t2 = VLD(i2);
    t3 = VLD(i3);

    t4 = VADD(t0, t1);
    t5 = VSUB(t0, t1);
    t6 = VADD(t2, t3);
    t7 = IMULI(inv, VSUB(t2, t3));

    *r0 = VADD(t4, t6);
    *r2 = VSUB(t4, t6);
    *r1 = VSUB(t5, t7);
    *r3 = VADD(t5, t7);
}

static FFTS_INLINE void LEAF_EE(data_t *const FFTS_RESTRICT out,
                                const ptrdiff_t *FFTS_RESTRICT os,
                                const data_t *FFTS_RESTRICT in,
                                const ptrdiff_t *FFTS_RESTRICT is,
                                int inv)
{
    const data_t *FFTS_RESTRICT LUT = inv ? ffts_constants_inv : ffts_constants;

    V r0, r1, r2, r3, r4, r5, r6, r7;

    data_t *out0 = out + os[0];
    data_t *out1 = out + os[1];

    L_4(inv, in + is[0], in + is[1], in + is[2], in + is[3], &r0, &r1, &r2, &r3);
    L_2(in + is[4], in + is[5], in + is[6], in + is[7], &r4, &r5, &r6, &r7);

    K_0(inv, &r0, &r2, &r4, &r6);
    K_N(inv, VLD(LUT + 0), VLD(LUT + 4), &r1, &r3, &r5, &r7);
    TX2(&r0, &r1);
    TX2(&r2, &r3);
    TX2(&r4, &r5);
    TX2(&r6, &r7);

    S_4(r0, r2, r4, r6, out0 + 0, out0 + 4, out0 + 8, out0 + 12);
    S_4(r1, r3, r5, r7, out1 + 0, out1 + 4, out1 + 8, out1 + 12);
}

static FFTS_INLINE void LEAF_EE2(data_t *const FFTS_RESTRICT out,
                                 const ptrdiff_t *FFTS_RESTRICT os,
                                 const data_t *FFTS_RESTRICT in,
                                 const ptrdiff_t *FFTS_RESTRICT is,
                                 int inv)
{
    const data_t *FFTS_RESTRICT LUT = inv ? ffts_constants_inv : ffts_constants;

    V r0, r1, r2, r3, r4, r5, r6, r7;

    data_t *out0 = out + os[0];
    data_t *out1 = out + os[1];

    L_4(inv, in + is[6], in + is[7], in + is[4], in + is[5], &r0, &r1, &r2, &r3);
    L_2(in + is[0], in + is[1], in + is[3], in + is[2], &r4, &r5, &r6, &r7);

    K_0(inv, &r0, &r2, &r4, &r6);
    K_N(inv, VLD(LUT + 0), VLD(LUT + 4), &r1, &r3, &r5, &r7);
    TX2(&r0, &r1);
    TX2(&r2, &r3);
    TX2(&r4, &r5);
    TX2(&r6, &r7);

    S_4(r0, r2, r4, r6, out0 + 0, out0 + 4, out0 + 8, out0 + 12);
    S_4(r1, r3, r5, r7, out1 + 0, out1 + 4, out1 + 8, out1 + 12);
}

static FFTS_INLINE void LEAF_EO(data_t *const FFTS_RESTRICT out,
                                const ptrdiff_t *FFTS_RESTRICT os,
                                const data_t *FFTS_RESTRICT in,
                                const ptrdiff_t *FFTS_RESTRICT is,
                                int inv)
{
    const data_t *FFTS_RESTRICT LUT = inv ? ffts_constants_inv : ffts_constants;

    V r0, r1, r2, r3, r4, r5, r6, r7;

    data_t *out0 = out + os[0];
    data_t *out1 = out + os[1];

    L_4_4(inv, in + is[0], in + is[1], in + is[2], in + is[3], &r0, &r1, &r2, &r3);
    L_2_4(inv, in + is[4], in + is[5], in + is[6], in + is[7], &r4, &r5, &r6, &r7);

    S_4(r2, r3, r7, r6, out1 + 0, out1 + 4, out1 + 8, out1 + 12);
    K_N(inv, VLD(LUT + 8), VLD(LUT + 12), &r0, &r1, &r4, &r5);
    S_4(r0, r1, r4, r5, out0 + 0, out0 + 4, out0 + 8, out0 + 12);
}

static FFTS_INLINE void LEAF_OE(data_t *const FFTS_RESTRICT out,
                                const ptrdiff_t *FFTS_RESTRICT os,
                                const data_t *FFTS_RESTRICT in,
                                const ptrdiff_t *FFTS_RESTRICT is,
                                int inv)
{
    const data_t *FFTS_RESTRICT LUT = inv ? ffts_constants_inv : ffts_constants;

    V r0, r1, r2, r3, r4, r5, r6, r7;

    data_t *out0 = out + os[0];
    data_t *out1 = out + os[1];

    L_4_2(inv, in + is[0], in + is[1], in + is[2], in + is[3], &r0, &r1, &r2, &r3);
    L_4_4(inv, in + is[6], in + is[7], in + is[4], in + is[5], &r4, &r5, &r6, &r7);

    S_4(r0, r1, r4, r5, out0 + 0, out0 + 4, out0 + 8, out0 + 12);
    K_N(inv, VLD(LUT + 8), VLD(LUT + 12), &r6, &r7, &r2, &r3);
    S_4(r6, r7, r2, r3, out1 + 0, out1 + 4, out1 + 8, out1 + 12);
}

static FFTS_INLINE void LEAF_OO(data_t *const FFTS_RESTRICT out,
                                const ptrdiff_t *FFTS_RESTRICT os,
                                const data_t *FFTS_RESTRICT in,
                                const ptrdiff_t *FFTS_RESTRICT is,
                                int inv)
{
    V r0, r1, r2, r3, r4, r5, r6, r7;

    data_t *out0 = out + os[0];
    data_t *out1 = out + os[1];

    L_4_4(inv, in + is[0], in + is[1], in + is[2], in + is[3], &r0, &r1, &r2, &r3);
    L_4_4(inv, in + is[6], in + is[7], in + is[4], in + is[5], &r4, &r5, &r6, &r7);

    S_4(r0, r1, r4, r5, out0 + 0, out0 + 4, out0 + 8, out0 + 12);
    S_4(r2, r3, r6, r7, out1 + 0, out1 + 4, out1 + 8, out1 + 12);
}

static FFTS_INLINE void X_4(int inv,
                            data_t *FFTS_RESTRICT data,
                            size_t N,
                            const data_t *FFTS_RESTRICT LUT)
{
    size_t i;

    for (i = 0; i < N/8; i++) {
        V r0 = VLD(data);
        V r1 = VLD(data + 2*N/4);
        V r2 = VLD(data + 4*N/4);
        V r3 = VLD(data + 6*N/4);

        K_N(inv, VLD(LUT), VLD(LUT + 4), &r0, &r1, &r2, &r3);

        VST(data        , r0);
        VST(data + 2*N/4, r1);
        VST(data + 4*N/4, r2);
        VST(data + 6*N/4, r3);

        LUT += 8;
        data += 4;
    }
}

static FFTS_INLINE void X_8(int inv,
                            data_t *FFTS_RESTRICT data0,
                            size_t N,
                            const data_t *FFTS_RESTRICT LUT)
{
    data_t *data1 = data0 + 1*N/4;
    data_t *data2 = data0 + 2*N/4;
    data_t *data3 = data0 + 3*N/4;
    data_t *data4 = data0 + 4*N/4;
    data_t *data5 = data0 + 5*N/4;
    data_t *data6 = data0 + 6*N/4;
    data_t *data7 = data0 + 7*N/4;
    size_t i;

    for (i = 0; i < N/16; i++) {
        V r0, r1, r2, r3, r4, r5, r6, r7;

        r0 = VLD(data0);
        r1 = VLD(data1);
        r2 = VLD(data2);
        r3 = VLD(data3);

        K_N(inv, VLD(LUT), VLD(LUT + 4), &r0, &r1, &r2, &r3);
        r4 = VLD(data4);
        r6 = VLD(data6);

        K_N(inv, VLD(LUT + 8), VLD(LUT + 12), &r0, &r2, &r4, &r6);
        r5 = VLD(data5);
        r7 = VLD(data7);

        K_N(inv, VLD(LUT + 16), VLD(LUT + 20), &r1, &r3, &r5, &r7);
        LUT += 24;

        VST(data0, r0);
        data0 += 4;

        VST(data1, r1);
        data1 += 4;

        VST(data2, r2);
        data2 += 4;

        VST(data3, r3);
        data3 += 4;

        VST(data4, r4);
        data4 += 4;

        VST(data5, r5);
        data5 += 4;

        VST(data6, r6);
        data6 += 4;

        VST(data7, r7);
        data7 += 4;
    }
}

static FFTS_INLINE void ffts_static_firstpass_odd(float *const FFTS_RESTRICT out,
        const float *FFTS_RESTRICT in,
        const ffts_plan_t *FFTS_RESTRICT p,
        int inv)
{
    size_t i, i0 = p->i0, i1 = p->i1;
    const ptrdiff_t *is = (const ptrdiff_t*) p->is;
    const ptrdiff_t *os = (const ptrdiff_t*) p->offsets;

    for (i = i0; i > 0; --i) {
        LEAF_EE(out, os, in, is, inv);
        in += 4;
        os += 2;
    }

    for (i = i1; i > 0; --i) {
        LEAF_OO(out, os, in, is, inv);
        in += 4;
        os += 2;
    }

    LEAF_OE(out, os, in, is, inv);
    in += 4;
    os += 2;

    for (i = i1; i > 0; --i) {
        LEAF_EE2(out, os, in, is, inv);
        in += 4;
        os += 2;
    }
}

static FFTS_INLINE void ffts_static_firstpass_even(float *FFTS_RESTRICT out,
        const float *FFTS_RESTRICT in,
        const ffts_plan_t *FFTS_RESTRICT p,
        int inv)
{
    size_t i, i0 = p->i0, i1 = p->i1;
    const ptrdiff_t *is = (const ptrdiff_t*) p->is;
    const ptrdiff_t *os = (const ptrdiff_t*) p->offsets;

    for(i = i0; i > 0; --i) {
        LEAF_EE(out, os, in, is, inv);
        in += 4;
        os += 2;
    }

    LEAF_EO(out, os, in, is, inv);
    in += 4;
    os += 2;

    for (i = i1; i > 0; --i) {
        LEAF_OO(out, os, in, is, inv);
        in += 4;
        os += 2;
    }

    for (i = i1; i > 0; --i) {
        LEAF_EE2(out, os, in, is, inv);
        in += 4;
        os += 2;
    }
}

void ffts_static_rec_f(ffts_plan_t *p, float *data, size_t N)
{
    const float *ws = (float*) p->ws;

    if (N > 128) {
        size_t N1 = N >> 1;
        size_t N2 = N >> 2;
        size_t N3 = N >> 3;

        ffts_static_rec_f(p, data              , N2);
        ffts_static_rec_f(p, data +     N1     , N3);
        ffts_static_rec_f(p, data +     N1 + N2, N3);
        ffts_static_rec_f(p, data + N          , N2);
        ffts_static_rec_f(p, data + N + N1     , N2);

        X_8(0, data, N, ws + (p->ws_is[ffts_ctzl(N) - 4] << 1));
    } else if (N == 128) {
        const float *ws1 = ws + (p->ws_is[1] << 1);

        X_8(0, data +   0, 32, ws1);

        X_4(0, data +  64, 16, ws);
        X_4(0, data +  96, 16, ws);

        X_8(0, data + 128, 32, ws1);
        X_8(0, data + 192, 32, ws1);

        X_8(0, data, N, ws + (p->ws_is[3] << 1));
    } else if (N == 64) {
        X_4(0, data +  0, 16, ws);
        X_4(0, data + 64, 16, ws);
        X_4(0, data + 96, 16, ws);

        X_8(0, data +  0,  N, ws + (p->ws_is[2] << 1));
    } else if (N == 32) {
        X_8(0, data, N, ws + (p->ws_is[1] << 1));
    } else {
        assert(N == 16);
        X_4(0, data, N, ws);
    }
}

void ffts_static_rec_i(ffts_plan_t *p, float *data, size_t N)
{
    float *ws = (float*) p->ws;

    if (N > 128) {
        size_t N1 = N >> 1;
        size_t N2 = N >> 2;
        size_t N3 = N >> 3;

        ffts_static_rec_i(p, data              , N2);
        ffts_static_rec_i(p, data +     N1     , N3);
        ffts_static_rec_i(p, data +     N1 + N2, N3);
        ffts_static_rec_i(p, data + N          , N2);
        ffts_static_rec_i(p, data + N + N1     , N2);

        X_8(1, data, N, ws + (p->ws_is[ffts_ctzl(N) - 4] << 1));
    } else if (N == 128) {
        const float *ws1 = ws + (p->ws_is[1] << 1);

        X_8(1, data +   0, 32, ws1);

        X_4(1, data +  64, 16, ws);
        X_4(1, data +  96, 16, ws);

        X_8(1, data + 128, 32, ws1);
        X_8(1, data + 192, 32, ws1);

        X_8(1, data, N, ws + (p->ws_is[3] << 1));
    } else if (N == 64) {
        X_4(1, data +  0, 16, ws);
        X_4(1, data + 64, 16, ws);
        X_4(1, data + 96, 16, ws);

        X_8(1, data +  0,  N, ws + (p->ws_is[2] << 1));
    } else if (N == 32) {
        X_8(1, data, N, ws + (p->ws_is[1] << 1));
    } else {
        assert(N == 16);
        X_4(1, data, N, ws);
    }
}

void ffts_static_transform_f(ffts_plan_t *p, const void *in, void *out)
{
    if (ffts_ctzl(p->N) & 1) {
        ffts_static_firstpass_odd(out, in, p, 0);
    } else {
        ffts_static_firstpass_even(out, in, p, 0);
    }

    ffts_static_rec_f(p, out, p->N);
}

void ffts_static_transform_i(ffts_plan_t *p, const void *in, void *out)
{
    if (ffts_ctzl(p->N) & 1) {
        ffts_static_firstpass_odd(out, in, p, 1);
    } else {
        ffts_static_firstpass_even(out, in, p, 1);
    }

    ffts_static_rec_i(p, out, p->N);
}