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
#include "ffts_dd.h"

/* 1/(2*cos(pow(2,-p)*pi)) */
static const FFTS_ALIGN(16) unsigned int half_secant[132] = {
    0x00000000, 0x3fe00000, 0xc9be45de, 0x3be3bd3c,
    0x00000000, 0x3fe00000, 0xc9be45de, 0x3c03bd3c,
    0x00000000, 0x3fe00000, 0xc9be45de, 0x3c23bd3c,
    0x00000000, 0x3fe00000, 0xc9be45de, 0x3c43bd3c,
    0x00000000, 0x3fe00000, 0xc9be45de, 0x3c63bd3c,
    0x00000000, 0x3fe00000, 0xc9be45df, 0x3c83bd3c,
    0x00000001, 0x3fe00000, 0x4df22efd, 0x3c7de9e6,
    0x00000005, 0x3fe00000, 0x906e8725, 0xbc60b0cd,
    0x00000014, 0x3fe00000, 0x906e8357, 0xbc80b0cd,
    0x0000004f, 0x3fe00000, 0x0dce83c9, 0xbc5619b2,
    0x0000013c, 0x3fe00000, 0x0dc6e79a, 0xbc7619b2,
    0x000004ef, 0x3fe00000, 0xe4af1240, 0x3c83cc9b,
    0x000013bd, 0x3fe00000, 0x2d14c08a, 0x3c7e64df,
    0x00004ef5, 0x3fe00000, 0x47a85465, 0xbc59b20b,
    0x00013bd4, 0x3fe00000, 0xab79c897, 0xbc79b203,
    0x0004ef4f, 0x3fe00000, 0x15019a96, 0x3c79386b,
    0x0013bd3d, 0x3fe00000, 0x7d6dbf4b, 0xbc7b16b7,
    0x004ef4f3, 0x3fe00000, 0xf30832e0, 0x3c741ee4,
    0x013bd3cd, 0x3fe00000, 0xd3bcd4bb, 0xbc83f41e,
    0x04ef4f34, 0x3fe00000, 0xdd75aebb, 0xbc82ef06,
    0x13bd3cde, 0x3fe00000, 0xb2b41b3d, 0x3c52d979,
    0x4ef4f46c, 0x3fe00000, 0x4f0fb458, 0xbc851db3,
    0x3bd3e0e7, 0x3fe00001, 0x8a0ce3f0, 0x3c58dbab,
    0xef507722, 0x3fe00004, 0x2a8ec295, 0x3c83e351,
    0xbd5114f9, 0x3fe00013, 0xc4c0d92d, 0x3c8b3ca4,
    0xf637de7d, 0x3fe0004e, 0xb74de729, 0x3c45974e,
    0xe8190891, 0x3fe0013b, 0x26edf4da, 0xbc814c20,
    0x9436640e, 0x3fe004f0, 0xe2b34b50, 0x3c8091ab,
    0x9c61d971, 0x3fe013d1, 0x6ce01b8e, 0x3c7f7df7,
    0xd17cba53, 0x3fe0503e, 0x74ad7633, 0xbc697609,
    0x7bdb3895, 0x3fe1517a, 0x82f9091b, 0xbc8008d1,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000
};

/* cos(pow(2,-p)*pi), sin(pow(2,-p)*pi) */
static const FFTS_ALIGN(16) unsigned int cos_sin_pi_table[264] = {
    0x00000000, 0x3ff00000, 0x54442d18, 0x3df921fb,
    0xc9be45de, 0xbbf3bd3c, 0xbb77974f, 0x3a91a390,
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e0921fb,
    0xc9be45de, 0xbc13bd3c, 0x54a14928, 0x3aa19bd0,
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e1921fb,
    0xc9be45de, 0xbc33bd3c, 0xb948108a, 0x3ab17cce,
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e2921fb,
    0xc9be45de, 0xbc53bd3c, 0x4be32e14, 0x3ac100c8,
    0x00000000, 0x3ff00000, 0x54442d18, 0x3e3921fb,
    0xc9be45de, 0xbc73bd3c, 0x2c9f4879, 0x3ace215d,
    0xffffffff, 0x3fefffff, 0x54442d18, 0x3e4921fb,
    0x6c837443, 0x3c888586, 0x0005f376, 0x3acd411f,
    0xfffffffe, 0x3fefffff, 0x54442d18, 0x3e5921fb,
    0x4df22ef1, 0xbc8de9e6, 0x9937209e, 0xbaf7b153,
    0xfffffff6, 0x3fefffff, 0x54442d16, 0x3e6921fb,
    0x906e88aa, 0x3c70b0cd, 0xfe19968a, 0xbb03b7c0,
    0xffffffd9, 0x3fefffff, 0x54442d0e, 0x3e7921fb,
    0xdf22ed26, 0xbc8e9e64, 0x8d1b6ffb, 0xbaee8bb4,
    0xffffff62, 0x3fefffff, 0x54442cef, 0x3e8921fb,
    0x0dd18f0f, 0x3c6619b2, 0x7f2b20fb, 0xbb00e133,
    0xfffffd88, 0x3fefffff, 0x54442c73, 0x3e9921fb,
    0x0dd314b2, 0x3c8619b2, 0x619fdf6e, 0xbb174e98,
    0xfffff621, 0x3fefffff, 0x54442a83, 0x3ea921fb,
    0x3764acf5, 0x3c8866c8, 0xf5b2407f, 0xbb388215,
    0xffffd886, 0x3fefffff, 0x544422c2, 0x3eb921fb,
    0x20e7a944, 0xbc8e64df, 0x7b9b9f23, 0x3b5a0961,
    0xffff6216, 0x3fefffff, 0x544403c1, 0x3ec921fb,
    0x52ee25ea, 0x3c69b20e, 0x4df6a86a, 0xbb5999d9,
    0xfffd8858, 0x3fefffff, 0x544387ba, 0x3ed921fb,
    0xd8910ead, 0x3c89b20f, 0x0809d04d, 0x3b77d9db,
    0xfff62162, 0x3fefffff, 0x544197a1, 0x3ee921fb,
    0x438d3925, 0xbc8937a8, 0xa5d27f7a, 0xbb858b02,
    0xffd88586, 0x3fefffff, 0x5439d73a, 0x3ef921fb,
    0x94b3ddd2, 0x3c8b22e4, 0xf8a3b73d, 0xbb863c7f,
    0xff62161a, 0x3fefffff, 0x541ad59e, 0x3f0921fb,
    0x7ea469b2, 0xbc835c13, 0xb8cee262, 0x3bae9860,
    0xfd885867, 0x3fefffff, 0x539ecf31, 0x3f1921fb,
    0x23a32e63, 0xbc77d556, 0xfcd23a30, 0x3b96b111,
    0xf621619c, 0x3fefffff, 0x51aeb57c, 0x3f2921fb,
    0xbbbd8fe6, 0xbc87507d, 0x4916c435, 0xbbca6e1d,
    0xd8858675, 0x3fefffff, 0x49ee4ea6, 0x3f3921fb,
    0x54748eab, 0xbc879f0e, 0x744a453e, 0x3bde894d,
    0x62161a34, 0x3fefffff, 0x2aecb360, 0x3f4921fb,
    0xb1f9b9c4, 0xbc6136dc, 0x7e566b4c, 0x3be87615,
    0x88586ee6, 0x3feffffd, 0xaee6472e, 0x3f5921fa,
    0xf173ae5b, 0x3c81af64, 0x284a9df8, 0xbbfee52e,
    0x21621d02, 0x3feffff6, 0xbecca4ba, 0x3f6921f8,
    0xebc82813, 0xbc76acfc, 0x7bcab5b2, 0x3c02ba40,
    0x858e8a92, 0x3fefffd8, 0xfe670071, 0x3f7921f0,
    0x1883bcf7, 0x3c8359c7, 0xfe6b7a9b, 0x3bfab967,
    0x169b92db, 0x3fefff62, 0xfcdec784, 0x3f8921d1,
    0xc81fbd0d, 0x3c85dda3, 0xbe836d9d, 0x3c29878e,
    0x6084cd0d, 0x3feffd88, 0xf7a3667e, 0x3f992155,
    0x4556e4cb, 0xbc81354d, 0x091a0130, 0xbbfb1d63,
    0xe3796d7e, 0x3feff621, 0xf10dd814, 0x3fa91f65,
    0x2e24aa15, 0xbc6c57bc, 0x0d569a90, 0xbc2912bd,
    0xa3d12526, 0x3fefd88d, 0xbc29b42c, 0x3fb917a6,
    0x378811c7, 0xbc887df6, 0xd26ed688, 0xbc3e2718,
    0xcff75cb0, 0x3fef6297, 0x3c69a60b, 0x3fc8f8b8,
    0x2a361fd3, 0x3c756217, 0xb9ff8d82, 0xbc626d19,
    0xcf328d46, 0x3fed906b, 0xa6aea963, 0x3fd87de2,
    0x10231ac2, 0x3c7457e6, 0xd3d5a610, 0xbc672ced,
    0x667f3bcd, 0x3fe6a09e, 0x667f3bcd, 0x3fe6a09e,
    0x13b26456, 0xbc8bdd34, 0x13b26456, 0xbc8bdd34,
    0x00000000, 0x00000000, 0x00000000, 0x3ff00000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000
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
#if HAVE_SSE2
int
ffts_generate_cosine_sine_pow2_32f(ffts_cpx_32f *const table, int table_size)
{
    static const __m128d sign_swap = { 0.0, -0.0 };
    const __m128d *FFTS_RESTRICT ct;
    const double *FFTS_RESTRICT hs;
    __m128d FFTS_ALIGN(16) w[32];
    __m128d FFTS_ALIGN(16) h[32];
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
    ct = (const __m128d*)
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[8 * offset]);
    hs = (const double*) &half_secant[4 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i] = ct[2*i];

        /* duplicate the high part */
        h[i] = _mm_set1_pd(hs[2*i]);
    }

    /* generate sine and cosine tables with maximum error less than 0.5 ULP */
    for (i = 1; i < table_size/2; i++) {
        /* calculate trailing zeros in index */
        log_2 = ffts_ctzl(i);

        /* note that storing is not 16 byte aligned */
        _mm_storel_pi((__m64*) &table[i + 0],
            _mm_cvtpd_ps(_mm_or_pd(w[log_2], sign_swap)));
        _mm_storel_pi((__m64*) &table[table_size - i], _mm_cvtpd_ps(
            _mm_or_pd(_mm_shuffle_pd(w[log_2], w[log_2], 1), sign_swap)));

        /* skip and find next trailing zero */
        offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
        w[log_2] = _mm_mul_pd(h[log_2], _mm_add_pd(w[log_2 + 1], w[offset]));
    }

mid_point:
    table[i][0] =  0.70710677f;
    table[i][1] = -0.70710677f;

exit:
    return 0;
}

int
ffts_generate_cosine_sine_pow2_64f(ffts_cpx_64f *const table, int table_size)
{
    static const __m128d sign_swap = { 0.0, -0.0 };
    const struct ffts_dd2_t *FFTS_RESTRICT ct;
    const double *FFTS_RESTRICT hs;
    struct ffts_dd2_t FFTS_ALIGN(16) w[32];
    struct ffts_dd2_t FFTS_ALIGN(16) h[32];
    struct ffts_dd2_t FFTS_ALIGN(16) sum;
    int i, log_2, offset;

    /* size must be a power of two */
    if (!table || !table_size || (table_size & (table_size - 1))) {
        return -1;
    }

    /* the first */
    table[0][0] =  1.0;
    table[0][1] = -0.0;

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
    ct = (const struct ffts_dd2_t*)
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[8 * offset]);
    hs = (const double*) &half_secant[4 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i] = ct[i];

        /* duplicate the high and low parts */
        h[i].hi = _mm_set1_pd(hs[2*i + 0]);
        h[i].lo = _mm_set1_pd(hs[2*i + 1]);
    }

    /* generate sine and cosine tables with maximum error less than 0.5 ULP */
    for (i = 1; i < table_size/2; i++) {
        /* calculate trailing zeros in index */
        log_2 = ffts_ctzl(i);

        /* result of ffts_dd_mul_dd is normalized */
        _mm_store_pd((double*) &table[i + 0],
            _mm_or_pd(w[log_2].hi, sign_swap));
        _mm_store_pd((double*) &table[table_size - i],
            _mm_or_pd(_mm_shuffle_pd(w[log_2].hi, w[log_2].hi, 1), sign_swap));

        /* skip and find next trailing zero */
        offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
        sum = ffts_dd2_add_dd2_unnormalized(&w[log_2 + 1], &w[offset]);
        w[log_2] = ffts_dd2_mul_dd2(&h[log_2], &sum);
    }

mid_point:
    table[i][0] =  0.707106781186547524;
    table[i][1] = -0.707106781186547524;

exit:
    return 0;
}
#else
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
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[8 * offset]);
    hs = (const double*) &half_secant[4 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i][0] = ct[2*i][0];
        w[i][1] = ct[2*i][1];
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
        w[log_2][0] = hs[2 * log_2] * (w[log_2 + 1][0] + w[offset][0]);
        w[log_2][1] = hs[2 * log_2] * (w[log_2 + 1][1] + w[offset][1]);
    }

mid_point:
    table[i][0] =  0.70710677f;
    table[i][1] = -0.70710677f;

exit:
    return 0;
}

int
ffts_generate_cosine_sine_pow2_64f(ffts_cpx_64f *const table, int table_size)
{
    const struct ffts_dd_t *FFTS_RESTRICT ct;
    const struct ffts_dd_t *FFTS_RESTRICT hs;
    struct ffts_dd_t FFTS_ALIGN(16) w[32][2];
    int i, log_2, offset;

    /* size must be a power of two */
    if (!table || !table_size || (table_size & (table_size - 1))) {
        return -1;
    }

    /* the first */
    table[0][0] =  1.0;
    table[0][1] = -0.0;

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
    ct = (const struct ffts_dd_t*)
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[8 * offset]);
    hs = (const struct ffts_dd_t*) &half_secant[4 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i][0].hi = ct[2*i + 0].hi;
        w[i][0].lo = ct[2*i + 1].hi;
        w[i][1].hi = ct[2*i + 0].lo;
        w[i][1].lo = ct[2*i + 1].lo;
    }

    /* generate sine and cosine tables with maximum error less than 0.5 ULP */
    for (i = 1; i < table_size/2; i++) {
        /* calculate trailing zeros in index */
        log_2 = ffts_ctzl(i);

        /* result of ffts_dd_mul_dd is normalized */
        table[i          + 0][0] =  w[log_2][0].hi;
        table[i          + 0][1] = -w[log_2][1].hi;
        table[table_size - i][0] =  w[log_2][1].hi;
        table[table_size - i][1] = -w[log_2][0].hi;

        /* skip and find next trailing zero */
        offset = (log_2 + 2 + ffts_ctzl(~i >> (log_2 + 2)));
        w[log_2][0] = ffts_dd_mul_dd(hs[log_2],
            ffts_dd_add_dd_unnormalized(w[log_2 + 1][0], w[offset][0]));
        w[log_2][1] = ffts_dd_mul_dd(hs[log_2],
            ffts_dd_add_dd_unnormalized(w[log_2 + 1][1], w[offset][1]));
    }

mid_point:
    table[i][0] =  0.707106781186547524;
    table[i][1] = -0.707106781186547524;

exit:
    return 0;
}
#endif

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
        FFTS_ASSUME_ALIGNED_32(&cos_sin_pi_table[8 * offset]);
    hs = (const double*) &half_secant[4 * offset];

    /* initialize from lookup table */
    for (i = 0; i <= log_2; i++) {
        w[i][0] = ct[2*i][0];
        w[i][1] = ct[2*i][1];
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
            w[log_2][0] = hs[2 * log_2] * (w[log_2 + 1][0] + w[offset][0]);
            w[log_2][1] = hs[2 * log_2] * (w[log_2 + 1][1] + w[offset][1]);
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
            w[log_2][0] = hs[2 * log_2] * (w[log_2 + 1][0] + w[offset][0]);
            w[log_2][1] = hs[2 * log_2] * (w[log_2 + 1][1] + w[offset][1]);
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