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

transform_func_t ffts_generate_func_code(ffts_plan_t *, size_t N, size_t leafN); 

#endif
