#include "arch_codegen.h"

#define P(x) (*(*p)++ = x)
void insert_nops(uint8_t **p, uint32_t count) {
	switch(count) {
		case 0: break;
		case 2: P(0x66); 
		case 1: P(0x90); break;
		case 3: P(0x0F); P(0x1F); P(0x00); break;	
		case 4: P(0x0F); P(0x1F); P(0x40); P(0x00); break;	
		case 5: P(0x0F); P(0x1F); P(0x44); P(0x00); P(0x00); break;	
		case 6: P(0x66); P(0x0F); P(0x1F); P(0x44); P(0x00); P(0x00); break;	
		case 7: P(0x0F); P(0x1F); P(0x80); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		case 8: P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		case 9: P(0x66); P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		default:
			P(0x66); P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); 
			insert_nops(p, count-9);
			break;	
	}
}
void align_mem16(uint8_t **p, uint32_t offset) {
	int r = (16 - (offset & 0xf)) - ((uint32_t)(*p) & 0xf);
	r = (16 + r) & 0xf;
	insert_nops(p, r);	
}

void define_transform_size(ffts_plan_t *p, size_t N) {
	if(N < 2048) p->transform_size = 16384;
	else p->transform_size = 16384 + 2*N/8 * __builtin_ctzl(N);
}
insns_t* generate_size8_base_case(insns_t **fp, int sign ) { 
	align_mem16(fp, 0);
	insns_t * x_8_addr = fp;
	align_mem16(fp, 5);
	memcpy(*fp, x8_soft, x8_hard - x8_soft);
	*fp += (x8_hard - x8_soft);
	return x_8_addr;
}

insns_t* generate_size4_base_case(insns_t **fp, int sign) {
	align_mem16(fp, 0);
	insns_t * x_4_addr = fp;
	memcpy(*fp, x4, x8_soft - x4);
	*fp += (x8_soft - x4);
	return x_4_addr;

}
