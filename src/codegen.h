#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <limits.h>	   /* for PAGESIZE */

typedef struct _ffts_plan_t ffts_plan_t;

typedef void (*transform_func_t)(float *data, size_t N, float *LUT);

void ffts_generate_func_code(ffts_plan_t *, size_t N, size_t leafN); 

//static const __attribute__ ((aligned(16))) float ee_w_data[4] = 
//{0.70710678118654757273731092936941,
// 0.70710678118654746171500846685376, 
// -0.70710678118654757273731092936941,
// -0.70710678118654746171500846685376};

//static const __attribute__ ((aligned(16))) float eo_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
//static const __attribute__ ((aligned(16))) float oe_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
#endif
