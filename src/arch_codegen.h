#ifndef __ARCH_CODEGEN_H__
#define __ARCH_CODEGEN_H__
#include "ffts.h"
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
	
	void generate_size8_base_case(insns_t *fp, int sign);
	void generate_size4_base_case(insns_t *fp, int sign);
#endif
