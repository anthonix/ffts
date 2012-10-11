#ifndef __TYPES_H__
#define __TYPES_H__

#define __INLINE static inline __attribute__((always_inline))

#if defined(complex)
	typedef complex float cdata_t;
#else
	typedef float cdata_t[2];
#endif
	typedef float data_t;

#endif


