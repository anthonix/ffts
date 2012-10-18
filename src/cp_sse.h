#ifndef __CP_SSE_H__
#define __CP_SSE_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
//#include <stdalign.h>

//#include "codegen.h"
#include "types.h"

//#define W(N,k) (cexp(-2.0f * M_PI * I * (float)(k) / (float)(N)))
#define PI 3.1415926535897932384626433832795

inline float W_re(float N, float k) {
	return cos(-2.0f * PI * k / N);
}

inline float W_im(float N, float k) {
	return sin(-2.0f * PI * k / N);
}


typedef size_t transform_index_t;

//typedef void (*transform_func_t)(float *data, size_t N, float *LUT);
typedef void (*transform_func_t)(float *data, size_t N, float *LUT);

struct _ffts_plan_t {
	ptrdiff_t *offsets;
	void __attribute__ ((aligned(32))) *ws;
	void __attribute__ ((aligned(32))) *oe_ws, *eo_ws, *ee_ws;
	ptrdiff_t *is;
	size_t *ws_is;
	size_t i0, i1, n_luts;
	size_t N;
	void *lastlut;
	transform_index_t *transforms; 
	//transform_func_t transform;
	void (*transform)(struct _ffts_plan_t * restrict, const float * restrict, float * restrict);
	void *transform_base;
	size_t transform_size;
	void *constants;
};

typedef struct _ffts_plan_t ffts_plan_t;

#endif
