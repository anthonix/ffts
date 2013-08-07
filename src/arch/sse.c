#include "ffts.h"
#include "codegen_sse.h"
void define_transform_size(ffts_plan_t *p, size_t N) {
	if(N < 2048) p->transform_size = 16384;
	else p->transform_size = 16384 + 2*N/8 * __builtin_ctzl(N);
}
void generate_size8_base_case(uint32_t *fp, int sign ) { 
	align_mem16(&fp, 0);
	x_8_addr = fp;
	align_mem16(&fp, 5);
	memcpy(fp, x8_soft, x8_hard - x8_soft);
	fp += (x8_hard - x8_soft);
}
