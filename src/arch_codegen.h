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
	/**
	 * TODO: Come up with Better documentation than this
	 * Prints the start function preamble and to move
	 * things onto the stack 
	 *
	 * Returns a pointer to start
	 */
	insns_t* generate_start_init(insns_t **fp, ffts_plan_t * p );

	insns_t * generate_start(insns_t **fp, ffts_plan_t * p, insns_t * x_4_addr, insns_t* x_8_addr, size_t leafN, size_t N, size_t *pps);
#endif
