#ifndef __ARCH_CODEGEN_H__
#define __ARCH_CODEGEN_H__
#include "ffts.h"
#include "codegen.h"
#ifdef HAVE_NEON
	#include "codegen_arm.h"
	#include "neon.h"
#elif HAVE_VFP
	#include "codegen_arm.h"
	#include "vfp.h"
#else
	#include "codegen_sse.h"
	#include "macros-sse.h"
#endif
	void define_transform_size(ffts_plan_t * p, size_t N );
	
	/**
	 * Will add a size 8 base transform for a given sign
	 * 
	 * Returns a pointer the start of the function
	 */
	insns_t* generate_size8_base_case(insns_t **fp, int sign);
	/**
	 * Add a size 4 base transform for a given sign
	 * 
	 * Returns a pointer the start of the function
	 */
	insns_t* generate_size4_base_case(insns_t **fp, int sign);
#endif
