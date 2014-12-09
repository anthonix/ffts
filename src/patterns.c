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

#include "patterns.h"

#include <assert.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

static void ffts_permute_addr(int N, int offset, int stride, int *d)
{
    int a[4] = {0,2,1,3};
    int i;

    for (i = 0; i < 4; i++) {
        d[i] = offset + (a[i] << stride);
        if (d[i] < 0) {
            d[i] += N;
        }
    }
}

static void ffts_hardcodedleaf_is_rec(ptrdiff_t **is, int big_N, int N, int poffset, int offset, int stride, int even, int VL)
{
    if (N > 4) {
        ffts_hardcodedleaf_is_rec(is, big_N, N/2, poffset, offset, stride + 1, even, VL);

        if (N/4 >= 4) {
            ffts_hardcodedleaf_is_rec(is, big_N, N/4, poffset + (1 << stride), offset + (N/2), stride + 2, 0, VL);
            ffts_hardcodedleaf_is_rec(is, big_N, N/4, poffset - (1 << stride), offset + (3*N/4), stride + 2, 0, VL);
        } else {
            int temp = poffset + (1 << stride);

            if (temp < 0) {
                temp += big_N;
            }

            temp *= 2;

            if (!(temp % (2 * VL))) {
                int i;

                (*is)[0] = poffset + (1 << stride);
                (*is)[1] = poffset + (1 << stride) + (1 << (stride + 2));
                (*is)[2] = poffset - (1 << stride);
                (*is)[3] = poffset - (1 << stride) + (1 << (stride + 2));

                for (i = 0; i < 4; i++) {
                    if ((*is)[i] < 0) {
                        (*is)[i] += big_N;
                    }
                }

                for (i = 0; i < 4; i++) {
                    (*is)[i] *= 2;
                }

                *is += 4;
            }
        }
    } else if (N == 4) {
        int perm[4];

        ffts_permute_addr(big_N, poffset, stride, perm);

        if (!((2 * perm[0]) % (2 * VL))) {
            int i;

            for (i = 0; i < 4; i++) {
                (*is)[i] = 2 * perm[i];
            }

            *is += 4;
        }
    }
}

ptrdiff_t *ffts_init_is(size_t N, size_t leaf_N, int VL)
{
    int i, i0, i1, i2;
    int stride = (int) (log(N/leaf_N) / log(2));
    ptrdiff_t *is, *pis;

    is = malloc(N / VL * sizeof(*is));
    if (!is) {
        return NULL;
    }

    i0 = N/leaf_N/3 + 1;
    i1 = i2 = N/leaf_N/3;
    if ((N/leaf_N) % 3 > 1) {
        i1++;
    }

    pis = is;
    for (i = 0; i < i0; i++) {
        ffts_hardcodedleaf_is_rec(&pis, N, leaf_N, i, 0, stride, 1, VL);
    }

    for (i = i0; i < i0 + i1; i++) {
        ffts_hardcodedleaf_is_rec(&pis, N, leaf_N / 2, i, 0, stride + 1, 1, VL);
        ffts_hardcodedleaf_is_rec(&pis, N, leaf_N / 2, i - (1 << stride), 0, stride + 1, 1, VL);
    }

    for (i = 0 - i2; i < 0; i++) {
        ffts_hardcodedleaf_is_rec(&pis, N, leaf_N, i, 0, stride, 1, VL);
    }

    return is;
}

static void ffts_elaborate_offsets(ptrdiff_t *offsets, int leafN, int N, int ioffset, int ooffset, int stride, int even)
{
    if ((even && N == leafN) || (!even && N <= leafN)) {
        offsets[2 * (ooffset / leafN) + 0] = ioffset * 2;
        offsets[2 * (ooffset / leafN) + 1] = ooffset;
    } else if (N > 4) {
        ffts_elaborate_offsets(offsets, leafN, N/2, ioffset, ooffset, stride + 1, even);
        ffts_elaborate_offsets(offsets, leafN, N/4, ioffset + (1<<stride), ooffset+N/2, stride + 2, 0);

        if (N/4 >= leafN) {
            ffts_elaborate_offsets(offsets, leafN, N/4, ioffset - (1<<stride), ooffset + 3*N/4, stride + 2, 0);
        }
    }
}

static int ffts_compare_offsets(const void *a, const void *b)
{
    ptrdiff_t diff = ((ptrdiff_t*) a)[0] - ((ptrdiff_t*) b)[0];
    assert(diff > INT_MIN && diff < INT_MAX);
    return (int) diff;
}

ptrdiff_t *ffts_init_offsets(size_t N, size_t leaf_N)
{
    ptrdiff_t *offsets, *tmp;
    size_t i;

    offsets = malloc(N/leaf_N * sizeof(*offsets));
    if (!offsets) {
        return NULL;
    }

    tmp = malloc(2 * N/leaf_N * sizeof(*tmp));
    if (!tmp) {
        free(offsets);
        return NULL;
    }

    ffts_elaborate_offsets(tmp, leaf_N, N, 0, 0, 1, 1);

    for (i = 0; i < 2*N/leaf_N; i += 2) {
        if (tmp[i] < 0) {
            tmp[i] = N + tmp[i];
        }
    }

    qsort(tmp, N/leaf_N, 2 * sizeof(*tmp), ffts_compare_offsets);

    for (i = 0; i < N/leaf_N; i++) {
        offsets[i] = 2 * tmp[2*i + 1];
    }

    free(tmp);
    return offsets;
}