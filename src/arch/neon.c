#include "ffts.h"
#include "codegen.h"
#include "arch_codegen.h"
#include "macros.h"
#include <sys/types.h>
#include <sys/mman.h>
#include "arch_codegen.h"
	#include "neon.h"
#include "vfp.h"
void define_transform_size(ffts_plan_t *p, size_t N) {
	if(N < 8192) p->transform_size = 8192;
	else p->transform_size = N;
}

void generate_size8_base_case(uint32_t *fp, int sign ) { 
	memcpy(fp, neon_x8, neon_x8_t - neon_x8);
	/* 
	 * Changes adds to subtracts and  vice versa to allow the computation 
	 * of both the IFFT and FFT
	 */
	if(sign < 0) {
		fp[31] ^= 0x00200000; fp[32] ^= 0x00200000; fp[33] ^= 0x00200000; fp[34] ^= 0x00200000;
		fp[65] ^= 0x00200000; fp[66] ^= 0x00200000; fp[70] ^= 0x00200000; fp[74] ^= 0x00200000;
		fp[97] ^= 0x00200000; fp[98] ^= 0x00200000; fp[102] ^= 0x00200000; fp[104] ^= 0x00200000;
	}
	fp += (neon_x8_t - neon_x8) / 4;
}
