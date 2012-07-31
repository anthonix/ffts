#ifndef __CP_SSE_H__
#define __CP_SSE_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stddef.h>
#include <xmmintrin.h>
#include <stdint.h>
#include <mach/mach_time.h>

typedef complex float cdata_t;
typedef float data_t;

#define W(N,k) (cexp(-2.0f * M_PI * I * (float)(k) / (float)(N)))

typedef size_t transform_index_t;

struct _ffts_plan_t {
	ptrdiff_t *is;
	ptrdiff_t *offsets;
	void __attribute__ ((aligned(32))) **ws;
	void (*firstpass)(const float * restrict, float * restrict, size_t, struct _ffts_plan_t * restrict);
	size_t i0, i1, i2;
	uint64_t n_bits, leaftime;
	
	transform_index_t *transforms; 
};


typedef struct _ffts_plan_t ffts_plan_t;


typedef struct _split_vec_t {
	__m128 re, im;
} split_vec_t;


#endif
