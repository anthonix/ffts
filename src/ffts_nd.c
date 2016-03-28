/*

This file is part of FFTS -- The Fastest Fourier Transform in the South

Copyright (c) 2016, Jukka Ojanen <jukka.ojanen@kolumbus.fi>
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

#include "ffts_nd.h"
#include "ffts_internal.h"

#ifdef HAVE_NEON
#include "neon.h"
#include <arm_neon.h>
#elif HAVE_SSE2
#include <emmintrin.h>
#endif

#define TSIZE 8

static void
ffts_free_nd(ffts_plan_t *p)
{
    if (p->plans) {
        int i;

        for (i = 0; i < p->rank; i++) {
            ffts_plan_t *plan = p->plans[i];

            if (plan) {
                int k;

                for (k = 0; k < i; k++) {
                    if (p->Ms[i] == p->Ms[k]) {
                        plan = NULL;
                        break;
                    }
                }

                if (plan) {
                    ffts_free(plan);
                }
            }
        }

        free(p->plans);
    }

    if (p->Ns) {
        free(p->Ns);
    }

    if (p->Ms) {
        free(p->Ms);
    }

    if (p->buf) {
        ffts_aligned_free(p->buf);
    }

    free(p);
}

static void
ffts_transpose(uint64_t *in, uint64_t *out, int w, int h)
{
#ifdef HAVE_NEON
#if 0
    neon_transpose(in, out, w, h);
#else
    size_t i, j;

    for (j = 0; j < h; j += 8) {
        for (i = 0; i < w; i += 8) {
            neon_transpose_to_buf(in + j*w + i, out + i*h + j, w);
        }
    }
#endif
#else
#ifdef HAVE_SSE
    uint64_t FFTS_ALIGN(64) tmp[TSIZE*TSIZE];
    int tx, ty;
    /* int x; */
    int y;
    int tw = w / TSIZE;
    int th = h / TSIZE;

    for (ty = 0; ty < th; ty++) {
        for (tx = 0; tx < tw; tx++) {
            uint64_t *ip0 = in + w*TSIZE*ty + tx * TSIZE;
            uint64_t *op0 = tmp; /* out + h*TSIZE*tx + ty*TSIZE; */

            /* copy/transpose to tmp */
            for (y = 0; y < TSIZE; y += 2) {
                /* for (x=0;x<TSIZE;x+=2) {
                   op[x*TSIZE] = ip[x];
                */
                __m128d q0 = _mm_load_pd((double*)(ip0 + 0*w));
                __m128d q1 = _mm_load_pd((double*)(ip0 + 1*w));
                __m128d q2 = _mm_load_pd((double*)(ip0 + 2*w));
                __m128d q3 = _mm_load_pd((double*)(ip0 + 3*w));
                __m128d q4 = _mm_load_pd((double*)(ip0 + 4*w));
                __m128d q5 = _mm_load_pd((double*)(ip0 + 5*w));
                __m128d q6 = _mm_load_pd((double*)(ip0 + 6*w));
                __m128d q7 = _mm_load_pd((double*)(ip0 + 7*w));

                __m128d t0 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(0, 0));
                __m128d t1 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(1, 1));
                __m128d t2 = _mm_shuffle_pd(q2, q3, _MM_SHUFFLE2(0, 0));
                __m128d t3 = _mm_shuffle_pd(q2, q3, _MM_SHUFFLE2(1, 1));
                __m128d t4 = _mm_shuffle_pd(q4, q5, _MM_SHUFFLE2(0, 0));
                __m128d t5 = _mm_shuffle_pd(q4, q5, _MM_SHUFFLE2(1, 1));
                __m128d t6 = _mm_shuffle_pd(q6, q7, _MM_SHUFFLE2(0, 0));
                __m128d t7 = _mm_shuffle_pd(q6, q7, _MM_SHUFFLE2(1, 1));

                ip0 += 2;
                /* _mm_store_pd((double *)(op0 + y*h + x), t0);
                   _mm_store_pd((double *)(op0 + y*h + x + h), t1);
                   */

                _mm_store_pd((double*)(op0 + 0        ), t0);
                _mm_store_pd((double*)(op0 + 0 + TSIZE), t1);
                _mm_store_pd((double*)(op0 + 2        ), t2);
                _mm_store_pd((double*)(op0 + 2 + TSIZE), t3);
                _mm_store_pd((double*)(op0 + 4        ), t4);
                _mm_store_pd((double*)(op0 + 4 + TSIZE), t5);
                _mm_store_pd((double*)(op0 + 6        ), t6);
                _mm_store_pd((double*)(op0 + 6 + TSIZE), t7);
                /* } */

                op0 += 2*TSIZE;
            }

            op0 = out + h*tx*TSIZE + ty*TSIZE;
            ip0 = tmp;
            for (y = 0; y < TSIZE; y += 1) {
                /* memcpy(op0, ip0, TSIZE * sizeof(*ip0)); */

                __m128d q0 = _mm_load_pd((double*)(ip0 + 0));
                __m128d q1 = _mm_load_pd((double*)(ip0 + 2));
                __m128d q2 = _mm_load_pd((double*)(ip0 + 4));
                __m128d q3 = _mm_load_pd((double*)(ip0 + 6));

                _mm_store_pd((double*)(op0 + 0), q0);
                _mm_store_pd((double*)(op0 + 2), q1);
                _mm_store_pd((double*)(op0 + 4), q2);
                _mm_store_pd((double*)(op0 + 6), q3);

                op0 += h;
                ip0 += TSIZE;
            }
        }
    }
    /*
    size_t i,j;
    for(i=0;i<w;i+=2) {
    for(j=0;j<h;j+=2) {
    //		out[i*h + j] = in[j*w + i];
    __m128d q0 = _mm_load_pd((double *)(in + j*w + i));
    __m128d q1 = _mm_load_pd((double *)(in + j*w + i + w));
    __m128d t0 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(0, 0));
    __m128d t1 = _mm_shuffle_pd(q0, q1, _MM_SHUFFLE2(1, 1));
    _mm_store_pd((double *)(out + i*h + j), t0);
    _mm_store_pd((double *)(out + i*h + j + h), t1);
    }
    }
    */
#endif
#endif
}

static void
ffts_execute_nd(ffts_plan_t *p, const void *in, void *out)
{
    uint64_t *din = (uint64_t*) in;
    uint64_t *buf = p->buf;
    uint64_t *dout = (uint64_t*) out;

    ffts_plan_t *plan;
    int i;
    size_t j;

    plan = p->plans[0];
    for (j = 0; j < p->Ns[0]; j++) {
        plan->transform(plan, din + (j * p->Ms[0]), buf + (j * p->Ms[0]));
    }

    ffts_transpose(buf, dout, p->Ms[0], p->Ns[0]);

    for (i = 1; i < p->rank; i++) {
        plan = p->plans[i];

        for (j = 0; j < p->Ns[i]; j++) {
            plan->transform(plan, dout + (j * p->Ms[i]), buf + (j * p->Ms[i]));
        }

        ffts_transpose(buf, dout, p->Ms[i], p->Ns[i]);
    }
}

FFTS_API ffts_plan_t*
ffts_init_nd(int rank, size_t *Ns, int sign)
{
    ffts_plan_t *p;
    size_t vol;
    int i;

    p = calloc(1, sizeof(*p));
    if (!p) {
        return NULL;
    }

    p->transform = &ffts_execute_nd;
    p->destroy   = &ffts_free_nd;
    p->rank      = rank;

    p->Ms = malloc(rank * sizeof(*p->Ms));
    if (!p->Ms) {
        goto cleanup;
    }

    p->Ns = malloc(rank * sizeof(*p->Ns));
    if (!p->Ns) {
        goto cleanup;
    }

    vol = p->Ns[0] = Ns[0];
    for (i = 1; i < rank; i++) {
        p->Ns[i] = Ns[i];
        vol *= Ns[i];
    }

    p->buf = ffts_aligned_malloc(2 * vol * sizeof(float));
    if (!p->buf) {
        goto cleanup;
    }

    p->plans = calloc(rank, sizeof(*p->plans));
    if (!p->plans) {
        goto cleanup;
    }

    for (i = 0; i < rank; i++) {
        int k;

        p->Ms[i] = vol / p->Ns[i];

        for (k = 0; k < i; k++) {
            if (p->Ms[k] == p->Ms[i]) {
                p->plans[i] = p->plans[k];
                break;
            }
        }

        if (!p->plans[i]) {
            p->plans[i] = ffts_init_1d(p->Ms[i], sign);
            if (!p->plans) {
                goto cleanup;
            }
        }
    }

    return p;

cleanup:
    ffts_free_nd(p);
    return NULL;
}

FFTS_API ffts_plan_t*
ffts_init_2d(size_t N1, size_t N2, int sign)
{
    size_t Ns[2];

    Ns[0] = N1;
    Ns[1] = N2;
    return ffts_init_nd(2, Ns, sign);
}
