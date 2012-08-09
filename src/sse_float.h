#ifndef __SSE_FLOAT_H__
#define __SSE_FLOAT_H__

#include <xmmintrin.h>

//#define VL 4

typedef __m128 V;

#define VADD _mm_add_ps
#define VSUB _mm_sub_ps
#define VMUL _mm_mul_ps
//#define VLIT4 _mm_set_ps
#define VXOR _mm_xor_ps
#define VST _mm_store_ps
#define VLD _mm_load_ps

#define VSWAPPAIRS(x) (_mm_shuffle_ps(x,x,_MM_SHUFFLE(2,3,0,1)))

#define VUNPACKHI(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(3,2,3,2)))
#define VUNPACKLO(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(1,0,1,0)))

#define VBLEND(x,y) (_mm_shuffle_ps(x,y,_MM_SHUFFLE(3,2,1,0)))

#define VLIT4 _mm_set_ps

#define VDUPRE(r) (_mm_shuffle_ps(r,r,_MM_SHUFFLE(2,2,0,0)))
#define VDUPIM(r) (_mm_shuffle_ps(r,r,_MM_SHUFFLE(3,3,1,1)))

#define FFTS_MALLOC(d,a) (_mm_malloc(d,a))
#define FFTS_FREE(d) (_mm_free(d))

#endif
