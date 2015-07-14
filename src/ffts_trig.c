/*

This file is part of FFTS -- The Fastest Fourier Transform in the South

Copyright (c) 2015, Jukka Ojanen <jukka.ojanen@kolumbus.fi>

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

#include "ffts_trig.h"

/* 1/(2*cos(pow(2,-p)*pi)) */
static const FFTS_ALIGN(16) unsigned int half_secant[66] = {
    0x00000000, 0x3fe00000, 0x00000000, 0x3fe00000, 0x00000000, 0x3fe00000,
    0x00000000, 0x3fe00000, 0x00000000, 0x3fe00000, 0x00000000, 0x3fe00000,
    0x00000001, 0x3fe00000, 0x00000005, 0x3fe00000, 0x00000014, 0x3fe00000,
    0x0000004f, 0x3fe00000, 0x0000013c, 0x3fe00000, 0x000004ef, 0x3fe00000,
    0x000013bd, 0x3fe00000, 0x00004ef5, 0x3fe00000, 0x00013bd4, 0x3fe00000,
    0x0004ef4f, 0x3fe00000, 0x0013bd3d, 0x3fe00000, 0x004ef4f3, 0x3fe00000,
    0x013bd3cd, 0x3fe00000, 0x04ef4f34, 0x3fe00000, 0x13bd3cde, 0x3fe00000,
    0x4ef4f46c, 0x3fe00000, 0x3bd3e0e7, 0x3fe00001, 0xef507722, 0x3fe00004,
    0xbd5114f9, 0x3fe00013, 0xf637de7d, 0x3fe0004e, 0xe8190891, 0x3fe0013b,
    0x9436640e, 0x3fe004f0, 0x9c61d971, 0x3fe013d1, 0xd17cba53, 0x3fe0503e,
    0x7bdb3895, 0x3fe1517a, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

/* cos(pow(2,-p)*pi), sin(pow(2,-p)*pi) */
static const FFTS_ALIGN(16) unsigned int cos_sin_pi_table[132] = {
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e0921fb, 0x00000000, 0x3ff00000,
    0x54442d18, 0x3e0921fb, 0x00000000, 0x3ff00000, 0x54442d18, 0x3e1921fb,
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e2921fb, 0x00000000, 0x3ff00000,
    0x54442d18, 0x3e3921fb, 0xffffffff, 0x3fefffff, 0x54442d18, 0x3e4921fb,
    0xfffffffe, 0x3fefffff, 0x54442d18, 0x3e5921fb, 0xfffffff6, 0x3fefffff,
    0x54442d16, 0x3e6921fb, 0xffffffd9, 0x3fefffff, 0x54442d0e, 0x3e7921fb,
    0xffffff62, 0x3fefffff, 0x54442cef, 0x3e8921fb, 0xfffffd88, 0x3fefffff,
    0x54442c73, 0x3e9921fb, 0xfffff621, 0x3fefffff, 0x54442a83, 0x3ea921fb,
    0xffffd886, 0x3fefffff, 0x544422c2, 0x3eb921fb, 0xffff6216, 0x3fefffff,
    0x544403c1, 0x3ec921fb, 0xfffd8858, 0x3fefffff, 0x544387ba, 0x3ed921fb,
    0xfff62162, 0x3fefffff, 0x544197a1, 0x3ee921fb, 0xffd88586, 0x3fefffff,
    0x5439d73a, 0x3ef921fb, 0xff62161a, 0x3fefffff, 0x541ad59e, 0x3f0921fb,
    0xfd885867, 0x3fefffff, 0x539ecf31, 0x3f1921fb, 0xf621619c, 0x3fefffff,
    0x51aeb57c, 0x3f2921fb, 0xd8858675, 0x3fefffff, 0x49ee4ea6, 0x3f3921fb,
    0x62161a34, 0x3fefffff, 0x2aecb360, 0x3f4921fb, 0x88586ee6, 0x3feffffd,
    0xaee6472e, 0x3f5921fa, 0x21621d02, 0x3feffff6, 0xbecca4ba, 0x3f6921f8,
    0x858e8a92, 0x3fefffd8, 0xfe670071, 0x3f7921f0, 0x169b92db, 0x3fefff62,
    0xfcdec784, 0x3f8921d1, 0x6084cd0d, 0x3feffd88, 0xf7a3667e, 0x3f992155,
    0xe3796d7e, 0x3feff621, 0xf10dd814, 0x3fa91f65, 0xa3d12526, 0x3fefd88d,
    0xbc29b42c, 0x3fb917a6, 0xcff75cb0, 0x3fef6297, 0x3c69a60b, 0x3fc8f8b8,
    0xcf328d46, 0x3fed906b, 0xa6aea963, 0x3fd87de2, 0x667f3bcd, 0x3fe6a09e,
    0x667f3bcd, 0x3fe6a09e, 0x00000000, 0x00000000, 0x00000000, 0x3ff00000
};

int
ffts_generate_cosine_sine_32f(ffts_cpx_32f *const table, int table_size)
{
    double alpha, beta;
    double c[2], s[2];
    double x, z;
    int i;

    if (!table || !table_size) {
        return -1;
    }

    /* the first */
    table[0][0] =  1.0f;
    table[0][1] = -0.0f;

    if (FFTS_UNLIKELY(table_size == 1)) {
        goto exit;
    }

    if (FFTS_UNLIKELY(table_size == 2)) {
        /* skip over */
        i = 1;
        goto mid_point;
    }

    /* polynomial approximations calculated using Sollya */
    x = 1.0 / table_size;
    z = x * x;

    /* alpha = 2 * sin(M_PI_4 / m) * sin(M_PI_4 / m) */
    alpha = x * (1.1107207345394952717884501203293686870741139540138 +
        z * (-0.114191397993514079911985272577099412137126013186879 +
        z * 3.52164670852685621720746817665316575239342815885835e-3));
    alpha = alpha * alpha;

    /* beta = sin(M_PI_2 / m) */
    beta = x * (1.57079632679489455959753740899031981825828552246094 +
        z * (-0.64596409735041482313988581154262647032737731933593 +
        z * 7.9690915468332887416913479228242067620158195495605e-2));

    /* cos(0) = 1.0, sin(0) = 0.0 */
    c[0] = 1.0;
    s[0] = 0.0;

    /* generate sine and cosine tables with maximum error less than 1 ULP */
    for (i = 1; i < (table_size + 1)/2; i++) {
        c[1] = c[0] - ((alpha * c[0]) + (beta * s[0]));
        s[1] = s[0] - ((alpha * s[0]) - (beta * c[0]));

        table[i          + 0][0] = (float)  c[1];
        table[i          + 0][1] = (float) -s[1];
        table[table_size - i][0] = (float)  s[1];
        table[table_size - i][1] = (float) -c[1];

        c[0] = c[1];
        s[0] = s[1];
    }

    if (FFTS_UNLIKELY(table_size & 1)) {
        goto exit;
    }

mid_point:
    table[i][0] =  0.70710677f;
    table[i][1] = -0.70710677f;

exit:
    return 0;
}

/* Oscar Buneman's method for generating a sequence of sines and cosines.
*  Expired US Patent 4,878,187 A
*
*  D. Potts, G. Steidl, M. Tasche, Numerical stability of fast
*  trigonometric transforms — a worst case study,
*  J. Concrete Appl. Math. 1 (2003) 1–36
*
*  O. Buneman, Stable on–line creation of sines and cosines of
*  successive angles, Proc. IEEE 75, 1434 – 1435 (1987).
*/
int
ffts_generate_cosine_sine_pow2_32f(ffts_cpx_32f *const table, int table_size)
{
    const ffts_cpx_64f *FFTS_RESTRICT ct;
    const double *FFTS_RESTRICT hs;
    ffts_cpx_64f FFTS_ALIGN(16) w[32];
    int i, log_2, offset;

    /* size must be a power of two */
    if (!table || !table_size || (table_size & (table_size - 1))) {
        return -1;
    }

    /* the first */
    table[0][0] =  1.0f;
    table[0][1] = -0.0f;

    if (FFTS_UNLIKELY(table_size == 1)) {
        goto exit;
    }

    if (FFTS_UNLIKELY(table_size == 2)) {
        /* skip over */
        i = 1;
        goto mid_point;
    }

    /* calculate table offset */
    FFTS_ASSUME(table_size/2 > 1);
    log_2 = ffts_ctzl(table_size);
    FFTS_ASSUME(log_2 > 1);
    offset = 32 - log_2;
    ct = (const ffts_cpx_64f*)
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[4 * offset]);
    hs = (const double*) &half_secant[2 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i][0] = ct[i][0];
        w[i][1] = ct[i][1];
    }

    /* generate sine and cosine tables with maximum error less than 0.5 ULP */
    for (i = 1; i < table_size/2; i++) {
        /* calculate trailing zeros in index */
        log_2 = ffts_ctzl(i);

        table[i          + 0][0] = (float)  w[log_2][0];
        table[i          + 0][1] = (float) -w[log_2][1];
        table[table_size - i][0] = (float)  w[log_2][1];
        table[table_size - i][1] = (float) -w[log_2][0];

        /* skip and find next trailing zero */
        offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
        w[log_2][0] = hs[log_2] * (w[log_2 + 1][0] + w[offset][0]);
        w[log_2][1] = hs[log_2] * (w[log_2 + 1][1] + w[offset][1]);
    }

mid_point:
    table[i][0] =  0.70710677f;
    table[i][1] = -0.70710677f;

exit:
    return 0;
}

int
ffts_generate_table_1d_real_32f(struct _ffts_plan_t *const p,
                                int sign,
                                int invert)
{
    const ffts_cpx_64f *FFTS_RESTRICT ct;
    const double *FFTS_RESTRICT hs;
    ffts_cpx_64f FFTS_ALIGN(16) w[32];
    int i, log_2, offset, N;
    float *A, *B;

    if (!p) {
        return -1;
    }

    A = (float*) FFTS_ASSUME_ALIGNED_32(p->A);
    B = (float*) FFTS_ASSUME_ALIGNED_32(p->B);
    N = (int) p->N;

    /* the first */
    if (sign < 0) {
        A[0] =  0.5f;
        A[1] = -0.5f;
        B[0] =  invert ? -0.5f : 0.5f;
        B[1] =  0.5f;
    } else {
        /* peel of the first */
        A[0] = 1.0f;
        A[1] = invert ? 1.0f : -1.0f;
        B[0] = 1.0f;
        B[1] = 1.0f;
    }

    if (FFTS_UNLIKELY(N == 4)) {
        i = 1;
        goto last;
    }

    /* calculate table offset */
    FFTS_ASSUME(N / 4 > 1);
    log_2 = ffts_ctzl(N);
    FFTS_ASSUME(log_2 > 2);
    offset = 34 - log_2;
    ct = (const ffts_cpx_64f*)
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[4 * offset]);
    hs = (const double*) &half_secant[2 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i][0] = ct[i][0];
        w[i][1] = ct[i][1];
    }

    /* generate sine and cosine tables with maximum error less than 0.5 ULP */
    if (sign < 0) {
        for (i = 1; i < N/4; i++) {
            float t0, t1, t2; 

            /* calculate trailing zeros in index */
            log_2 = ffts_ctzl(i);

            t0 = (float) (0.5 * (1.0 - w[log_2][1]));
            t1 = (float) (0.5 * w[log_2][0]);
            t2 = (float) (0.5 * (1.0 + w[log_2][1]));

            A[    2 * i + 0] =  t0;
            A[N - 2 * i + 0] =  t0;
            A[    2 * i + 1] = -t1;
            A[N - 2 * i + 1] =  t1;

            B[    2 * i + 0] =  invert ? -t2 : t2;
            B[N - 2 * i + 0] =  invert ? -t2 : t2;
            B[    2 * i + 1] =  t1;
            B[N - 2 * i + 1] = -t1;

            /* skip and find next trailing zero */
            offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
            w[log_2][0] = hs[log_2] * (w[log_2 + 1][0] + w[offset][0]);
            w[log_2][1] = hs[log_2] * (w[log_2 + 1][1] + w[offset][1]);
        }
    } else {
        for (i = 1; i < N/4; i++) {
            float t0, t1, t2; 

            /* calculate trailing zeros in index */
            log_2 = ffts_ctzl(i);

            t0 = (float) (1.0 - w[log_2][1]);
            t1 = (float) w[log_2][0];
            t2 = (float) (1.0 + w[log_2][1]);

            A[    2 * i + 0] = t0;
            A[N - 2 * i + 0] = t0;
            A[    2 * i + 1] = invert ?  t1 : -t1;
            A[N - 2 * i + 1] = invert ? -t1 :  t1;

            B[    2 * i + 0] =  t2;
            B[N - 2 * i + 0] =  t2;
            B[    2 * i + 1] =  t1;
            B[N - 2 * i + 1] = -t1;

            /* skip and find next trailing zero */
            offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
            w[log_2][0] = hs[log_2] * (w[log_2 + 1][0] + w[offset][0]);
            w[log_2][1] = hs[log_2] * (w[log_2 + 1][1] + w[offset][1]);
        }
    }

last:
    if (sign < 0) {
        A[2 * i + 0] = 0.0f;
        A[2 * i + 1] = 0.0f;
        B[2 * i + 0] = invert ? -1.0f : 1.0f;
        B[2 * i + 1] = 0.0f;
    } else {
        A[2 * i + 0] = 0.0f;
        A[2 * i + 1] = 0.0f;
        B[2 * i + 0] = 2.0f;
        B[2 * i + 1] = 0.0f;
    }

    return 0;
}