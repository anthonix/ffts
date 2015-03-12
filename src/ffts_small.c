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

#include "ffts_small.h"

#include "ffts_internal.h"
#include "macros.h"

void
ffts_small_2_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    ffts_cpx_32f t0, t1, r0, r1;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[2];
    t1[1] = din[3];

    r0[0] = t0[0] + t1[0];
    r0[1] = t0[1] + t1[1];
    r1[0] = t0[0] - t1[0];
    r1[1] = t0[1] - t1[1];

    dout[0] = r0[0];
    dout[1] = r0[1];
    dout[2] = r1[0];
    dout[3] = r1[1];
}

void
ffts_small_2_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    ffts_cpx_64f t0, t1, r0, r1;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[2];
    t1[1] = din[3];

    r0[0] = t0[0] + t1[0];
    r0[1] = t0[1] + t1[1];
    r1[0] = t0[0] - t1[0];
    r1[1] = t0[1] - t1[1];

    dout[0] = r0[0];
    dout[1] = r0[1];
    dout[2] = r1[0];
    dout[3] = r1[1];
}

void
ffts_small_forward4_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    ffts_cpx_32f t0, t1, t2, t3, t4, t5, t6, t7;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[4];
    t1[1] = din[5];
    t2[0] = din[2];
    t2[1] = din[3];
    t3[0] = din[6];
    t3[1] = din[7];

    t4[0] = t0[0] + t1[0];
    t4[1] = t0[1] + t1[1];
    t5[0] = t0[0] - t1[0];
    t5[1] = t0[1] - t1[1];
    t6[0] = t2[0] + t3[0];
    t6[1] = t2[1] + t3[1];
    t7[0] = t2[0] - t3[0];
    t7[1] = t2[1] - t3[1];

    dout[0] = t4[0] + t6[0];
    dout[1] = t4[1] + t6[1];
    dout[4] = t4[0] - t6[0];
    dout[5] = t4[1] - t6[1];
    dout[2] = t5[0] + t7[1];
    dout[3] = t5[1] - t7[0];
    dout[6] = t5[0] - t7[1];
    dout[7] = t5[1] + t7[0];
}

void
ffts_small_forward4_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    ffts_cpx_64f t0, t1, t2, t3, t4, t5, t6, t7;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[4];
    t1[1] = din[5];
    t2[0] = din[2];
    t2[1] = din[3];
    t3[0] = din[6];
    t3[1] = din[7];

    t4[0] = t0[0] + t1[0];
    t4[1] = t0[1] + t1[1];
    t5[0] = t0[0] - t1[0];
    t5[1] = t0[1] - t1[1];
    t6[0] = t2[0] + t3[0];
    t6[1] = t2[1] + t3[1];
    t7[0] = t2[0] - t3[0];
    t7[1] = t2[1] - t3[1];

    dout[0] = t4[0] + t6[0];
    dout[1] = t4[1] + t6[1];
    dout[4] = t4[0] - t6[0];
    dout[5] = t4[1] - t6[1];
    dout[2] = t5[0] + t7[1];
    dout[3] = t5[1] - t7[0];
    dout[6] = t5[0] - t7[1];
    dout[7] = t5[1] + t7[0];
}

void
ffts_small_backward4_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    ffts_cpx_32f t0, t1, t2, t3, t4, t5, t6, t7;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[4];
    t1[1] = din[5];
    t2[0] = din[2];
    t2[1] = din[3];
    t3[0] = din[6];
    t3[1] = din[7];

    t4[0] = t0[0] + t1[0];
    t4[1] = t0[1] + t1[1];
    t5[0] = t0[0] - t1[0];
    t5[1] = t0[1] - t1[1];
    t6[0] = t2[0] + t3[0];
    t6[1] = t2[1] + t3[1];
    t7[0] = t2[0] - t3[0];
    t7[1] = t2[1] - t3[1];

    dout[0] = t4[0] + t6[0];
    dout[1] = t4[1] + t6[1];
    dout[4] = t4[0] - t6[0];
    dout[5] = t4[1] - t6[1];
    dout[2] = t5[0] - t7[1];
    dout[3] = t5[1] + t7[0];
    dout[6] = t5[0] + t7[1];
    dout[7] = t5[1] - t7[0];
}

void
ffts_small_backward4_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    ffts_cpx_64f t0, t1, t2, t3, t4, t5, t6, t7;

    /* unreferenced parameter */
    (void) p;

    t0[0] = din[0];
    t0[1] = din[1];
    t1[0] = din[4];
    t1[1] = din[5];
    t2[0] = din[2];
    t2[1] = din[3];
    t3[0] = din[6];
    t3[1] = din[7];

    t4[0] = t0[0] + t1[0];
    t4[1] = t0[1] + t1[1];
    t5[0] = t0[0] - t1[0];
    t5[1] = t0[1] - t1[1];
    t6[0] = t2[0] + t3[0];
    t6[1] = t2[1] + t3[1];
    t7[0] = t2[0] - t3[0];
    t7[1] = t2[1] - t3[1];

    dout[0] = t4[0] + t6[0];
    dout[1] = t4[1] + t6[1];
    dout[4] = t4[0] - t6[0];
    dout[5] = t4[1] - t6[1];
    dout[2] = t5[0] - t7[1];
    dout[3] = t5[1] + t7[0];
    dout[6] = t5[0] + t7[1];
    dout[7] = t5[1] - t7[0];
}

void
ffts_small_forward8_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    V4SF r0_1, r2_3, r4_5, r6_7;
    float *LUT8 = (float*) p->ws + p->ws_is[0];

    V4SF_L_4_2(0, din, din+8, din+4, din+12, &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_K_N(0, V4SF_LD(LUT8), V4SF_LD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_S_4(r0_1, r2_3, r4_5, r6_7, dout+0, dout+4, dout+8, dout+12);
}

void
ffts_small_forward8_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    V4SF r0_1, r2_3, r4_5, r6_7;
    double *LUT8 = (double*) p->ws + p->ws_is[0];

#if MACROS_READY
    L_4_2(0, din, din+8, din+4, din+12, &r0_1, &r2_3, &r4_5, &r6_7);
    K_N(0, VLD(LUT8), VLD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    S_4(r0_1, r2_3, r4_5, r6_7, dout+0, dout+4, dout+8, dout+12);
#endif
}

void
ffts_small_backward8_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    V4SF r0_1, r2_3, r4_5, r6_7;
    float *LUT8 = (float*) p->ws + p->ws_is[0];

    V4SF_L_4_2(1, din, din+8, din+4, din+12, &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_K_N(1, V4SF_LD(LUT8), V4SF_LD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_S_4(r0_1, r2_3, r4_5, r6_7, dout+0, dout+4, dout+8, dout+12);
}

void
ffts_small_backward8_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    V4SF r0_1, r2_3, r4_5, r6_7;
    double *LUT8 = (double*) p->ws + p->ws_is[0];

#if MACROS_READY
    L_4_2(1, din, din+8, din+4, din+12, &r0_1, &r2_3, &r4_5, &r6_7);
    K_N(1, VLD(LUT8), VLD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    S_4(r0_1, r2_3, r4_5, r6_7, dout+0, dout+4, dout+8, dout+12);
#endif
}

void
ffts_small_forward16_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    float *LUT8 = (float*) p->ws;
    V4SF r0_1, r2_3, r4_5, r6_7, r8_9, r10_11, r12_13, r14_15;

    V4SF_L_4_4(0, din+0, din+16, din+8, din+24, &r0_1, &r2_3, &r8_9, &r10_11);
    V4SF_L_2_4(0, din+4, din+20, din+28, din+12, &r4_5, &r6_7, &r14_15, &r12_13);
    V4SF_K_N(0, V4SF_LD(LUT8), V4SF_LD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_K_N(0, V4SF_LD(LUT8+8), V4SF_LD(LUT8+12), &r0_1, &r4_5, &r8_9, &r12_13);
    V4SF_S_4(r0_1, r4_5, r8_9, r12_13, dout+0, dout+8, dout+16, dout+24);
    V4SF_K_N(0, V4SF_LD(LUT8+16), V4SF_LD(LUT8+20), &r2_3, &r6_7, &r10_11, &r14_15);
    V4SF_S_4(r2_3, r6_7, r10_11, r14_15, dout+4, dout+12, dout+20, dout+28);
}

void
ffts_small_forward16_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    double *LUT8 = (double*) p->ws;
    V4SF r0_1, r2_3, r4_5, r6_7, r8_9, r10_11, r12_13, r14_15;

#ifdef MACROS_READY
    L_4_4(0, din+0, din+16, din+8, din+24, &r0_1, &r2_3, &r8_9, &r10_11);
    L_2_4(0, din+4, din+20, din+28, din+12, &r4_5, &r6_7, &r14_15, &r12_13);
    K_N(0, VLD(LUT8), VLD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    K_N(0, VLD(LUT8+8), VLD(LUT8+12), &r0_1, &r4_5, &r8_9, &r12_13);
    S_4(r0_1, r4_5, r8_9, r12_13, dout+0, dout+8, dout+16, dout+24);
    K_N(0, VLD(LUT8+16), VLD(LUT8+20), &r2_3, &r6_7, &r10_11, &r14_15);
    S_4(r2_3, r6_7, r10_11, r14_15, dout+4, dout+12, dout+20, dout+28);
#endif
}

void
ffts_small_backward16_32f(ffts_plan_t *p, const void *in, void *out)
{
    const float *din = (const float*) in;
    float *dout = (float*) out;
    float *LUT8 = (float*) p->ws;
    V4SF r0_1, r2_3, r4_5, r6_7, r8_9, r10_11, r12_13, r14_15;

    V4SF_L_4_4(1, din+0, din+16, din+8, din+24, &r0_1, &r2_3, &r8_9, &r10_11);
    V4SF_L_2_4(1, din+4, din+20, din+28, din+12, &r4_5, &r6_7, &r14_15, &r12_13);
    V4SF_K_N(1, V4SF_LD(LUT8+ 0), V4SF_LD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    V4SF_K_N(1, V4SF_LD(LUT8+ 8), V4SF_LD(LUT8+12), &r0_1, &r4_5, &r8_9, &r12_13);
    V4SF_S_4(r0_1, r4_5, r8_9, r12_13, dout+0, dout+8, dout+16, dout+24);
    V4SF_K_N(1, V4SF_LD(LUT8+16), V4SF_LD(LUT8+20), &r2_3, &r6_7, &r10_11, &r14_15);
    V4SF_S_4(r2_3, r6_7, r10_11, r14_15, dout+4, dout+12, dout+20, dout+28);
}

void
ffts_small_backward16_64f(ffts_plan_t *p, const void *in, void *out)
{
    const double *din = (const double*) in;
    double *dout = (double*) out;
    double *LUT8 = (double*) p->ws;
    V4SF r0_1, r2_3, r4_5, r6_7, r8_9, r10_11, r12_13, r14_15;

#ifdef MACROS_READY
    L_4_4(1, din+0, din+16, din+8, din+24, &r0_1, &r2_3, &r8_9, &r10_11);
    L_2_4(1, din+4, din+20, din+28, din+12, &r4_5, &r6_7, &r14_15, &r12_13);
    K_N(1, VLD(LUT8), VLD(LUT8+4), &r0_1, &r2_3, &r4_5, &r6_7);
    K_N(1, VLD(LUT8+8), VLD(LUT8+12),&r0_1, &r4_5, &r8_9, &r12_13);
    S_4(r0_1, r4_5, r8_9, r12_13, dout+0, dout+8, dout+16, dout+24);
    K_N(1, VLD(LUT8+16), VLD(LUT8+20), &r2_3, &r6_7, &r10_11, &r14_15);
    S_4(r2_3, r6_7, r10_11, r14_15, dout+4, dout+12, dout+20, dout+28);
#endif
}