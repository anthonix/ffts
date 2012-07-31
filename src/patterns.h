#ifndef __PATTERNS_H__
#define __PATTERNS_H__

#include "cp_sse.h"

void init_is(ffts_plan_t *p, int N, int leafN, int VL); 
void init_offsets(ffts_plan_t *p, int N, int leafN); 
void init_tree(ffts_plan_t *p, int N, int leafN); 

#endif
