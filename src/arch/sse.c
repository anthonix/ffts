#include "arch_codegen.h"
#include "ffts.h"
void IMM8(uint8_t **p, int32_t imm) {
		*(*p)++ = (imm & 0xff);
}

void IMM16(uint8_t **p, int32_t imm) {
	int i;
	for(i=0;i<2;i++) {
		*(*p)++ = (imm & (0xff << (i*8))) >> (i*8);
	} 
}
void IMM32(uint8_t **p, int32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(*p)++ = (imm & (0xff << (i*8))) >> (i*8);
	} 
}
void IMM32_NI(uint8_t *p, int32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(p+i) = (imm & (0xff << (i*8))) >> (i*8);
	} 
}

int32_t READ_IMM32(uint8_t *p) {
	int32_t rval = 0;
	int i;
	for(i=0;i<4;i++) {
		rval |= *(p+i) << (i*8);
	} 
	return rval;
}

void MOVI(uint8_t **p, uint8_t dst, uint32_t imm) {
//  if(imm < 65536) *(*p)++ = 0x66; 
  if(dst >= 8) *(*p)++ = 0x41;

  //if(imm < 65536 && imm >= 256) *(*p)++ = 0x66; 

  //if(imm >= 256) 
	*(*p)++ = 0xb8 | (dst & 0x7);
//  else           *(*p)++ = 0xb0 | (dst & 0x7);

 // if(imm < 256) IMM8(p, imm);
//  else 
//if(imm < 65536) IMM16(p, imm);
//else 
	IMM32(p, imm);

//if(dst < 8) {
//	*(*p)++ = 0xb8 + dst; 
//}else{
//	*(*p)++ = 0x49;
//	*(*p)++ = 0xc7;
//	*(*p)++ = 0xc0 | (dst - 8);
//}
//IMM32(p, imm);
}

void ADDRMODE(uint8_t **p, uint8_t reg, uint8_t rm, int32_t disp) {
	if(disp == 0) {
		*(*p)++ = (rm & 7) | ((reg & 7) << 3);
	}else if(disp <= 127 || disp >= -128) {
		*(*p)++ = 0x40 | (rm & 7) | ((reg & 7) << 3);
		IMM8(p, disp);
	}else{
		*(*p)++ = 0x80 | (rm & 7) | ((reg & 7) << 3);
		IMM32(p, disp);
	}
}

void LEA(uint8_t **p, uint8_t dst, uint8_t base, int32_t disp) {

	*(*p)++ = 0x48 | ((base & 0x8) >> 3) | ((dst & 0x8) >> 1);
	*(*p)++ = 0x8d;
	ADDRMODE(p, dst, base, disp);		
}

void RET(uint8_t **p) {
	*(*p)++ = 0xc3;
}

void ADDI(uint8_t **p, uint8_t dst, int32_t imm) {
	
	if(dst >= 8) *(*p)++ = 0x49;
	else         *(*p)++ = 0x48;
	
	if(imm > 127 || imm <= -128) *(*p)++ = 0x81;
	else          *(*p)++ = 0x83;
	
	*(*p)++ = 0xc0 | (dst & 0x7);

	if(imm > 127 || imm <= -128) IMM32(p, imm);
	else          IMM8(p, imm);
}

void CALL(uint8_t **p, uint8_t *func) {
	*(*p)++ = 0xe8;
	IMM32(p, ((void *)func) - (void *)(*p) - 4); 
}

void PUSH(uint8_t **p, uint8_t reg) {
	if(reg >= 8) *(*p)++ = 0x41;
	*(*p)++ = 0x50 | (reg & 7);
}
void POP(uint8_t **p, uint8_t reg) {
	if(reg >= 8) *(*p)++ = 0x41;
	*(*p)++ = 0x58 | (reg & 7);
}


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
	insns_t * x_8_addr = *fp;
	align_mem16(fp, 5);
	memcpy(*fp, x8_soft, x8_hard - x8_soft);
	*fp += (x8_hard - x8_soft);
	return x_8_addr;
}

insns_t* generate_size4_base_case(insns_t **fp, int sign) {
	align_mem16(fp, 0);
	insns_t * x_4_addr = *fp;
	memcpy(*fp, x4, x8_soft - x4);
	*fp += (x8_soft - x4);
	return x_4_addr;

}
insns_t* generate_start_init(insns_t **fp, ffts_plan_t *p  ) {
	
	align_mem16(fp, 0);
	insns_t	* start = *fp;

	/* The following is equivilant to
	 * mov (%rdi), %r8
	 */
	*(*fp)++ = 0x4c;
	*(*fp)++ = 0x8b;
	*(*fp)++ = 0x07;
	uint32_t lp_cnt = p->i0 * 4;
	MOVI(fp, RCX, lp_cnt);
	return start;
}



insns_t * generate_start(insns_t **fp, ffts_plan_t * p, insns_t * x_4_addr, insns_t* x_8_addr, size_t leafN ,size_t N, size_t *pps) {

	uint32_t lp_cnt = p->i0 * 4;
//fprintf(stderr, "Body start address = %016p\n", start);
	/*	
	 *	Function Preamble
	 *	Start has its own calling convention.
	 *	It just saves all the registers its going to use 
	 *	on to the stack and pops them off at the end
	 */
	PUSH(fp, RBP);	
	PUSH(fp, RBX);
	PUSH(fp, R10);
	PUSH(fp, R11);
	PUSH(fp, R12);
	PUSH(fp, R13);
	PUSH(fp, R14);
	PUSH(fp, R15);
	int i;


	memcpy(*fp, leaf_ee_init, leaf_ee - leaf_ee_init);
	
	*fp += (leaf_ee - leaf_ee_init);

//fprintf(stderr, "Leaf start address = %016p\n", fp);
	align_mem16(fp, 9);
	memcpy(*fp, leaf_ee, leaf_oo - leaf_ee);


	uint32_t offsets[8] = {0, N, N/2, 3*N/2, N/4, 5*N/4, 7*N/4, 3*N/4};
	uint32_t offsets_o[8] = {0, N, N/2, 3*N/2, 7*N/4, 3*N/4, N/4, 5*N/4};
	uint32_t offsets_oe[8] = {7*N/4, 3*N/4, N/4, 5*N/4, 0, N, 3*N/2, N/2};
	
	for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_ee_offsets[i], offsets[i]*4); 
	
	*fp += (leaf_oo - leaf_ee);
	
	if(__builtin_ctzl(N) & 1){

		if(p->i1) {
			lp_cnt += p->i1 * 4;
			MOVI(fp, RCX, lp_cnt);
			align_mem16(fp, 4);
			memcpy(*fp, leaf_oo, leaf_eo - leaf_oo);
			for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_oo_offsets[i], offsets_o[i]*4); 
			*fp += (leaf_eo - leaf_oo);
		}
		

		memcpy(*fp, leaf_oe, leaf_end - leaf_oe);
		lp_cnt += 4;
		for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_oe_offsets[i], offsets_o[i]*4); 
		*fp += (leaf_end - leaf_oe);

	}else{


		memcpy(*fp, leaf_eo, leaf_oe - leaf_eo);
		lp_cnt += 4;
		for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_eo_offsets[i], offsets[i]*4); 
		*fp += (leaf_oe - leaf_eo);

		if(p->i1) {
			lp_cnt += p->i1 * 4;
			MOVI(fp, RCX, lp_cnt);
			align_mem16(fp, 4);
			memcpy(*fp, leaf_oo, leaf_eo - leaf_oo);
			for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_oo_offsets[i], offsets_o[i]*4); 
			*fp += (leaf_eo - leaf_oo);
		}

	}
	if(p->i1) {
		lp_cnt += p->i1 * 4;
		MOVI(fp, RCX, lp_cnt);
		align_mem16(fp, 9);
		memcpy(*fp, leaf_ee, leaf_oo - leaf_ee);
		for(i=0;i<8;i++) IMM32_NI(*fp + sse_leaf_ee_offsets[i], offsets_oe[i]*4); 
		*fp += (leaf_oo - leaf_ee);

	}
	
//fprintf(stderr, "Body start address = %016p\n", fp);
  //LEA(&fp, R8, RDI, ((uint32_t)&p->ws) - ((uint32_t)p)); 
	memcpy(*fp, x_init, x4 - x_init);
//IMM32_NI(fp + 3, ((int64_t)READ_IMM32(fp + 3)) + ((void *)x_init - (void *)fp )); 
	*fp += (x4 - x_init);

	int32_t pAddr = 0;
	int32_t pN = 0;
	int32_t pLUT = 0;
	int count = 2;
	while(pps[0]) {
	
		if(!pN) {
			MOVI(fp, RCX, pps[0] / 4);
		}else{
  		if((pps[1]*4)-pAddr) ADDI(fp, RDX, (pps[1] * 4)- pAddr);
			if(pps[0] > leafN && pps[0] - pN) {
				
				int diff = __builtin_ctzl(pps[0]) - __builtin_ctzl(pN);
				*(*fp)++ = 0xc1; 
				
				if(diff > 0) {
					*(*fp)++ = 0xe1;
					*(*fp)++ = (diff & 0xff);
				}else{
					*(*fp)++ = 0xe9;
					*(*fp)++ = ((-diff) & 0xff);
				}
			}
		}
		
  		if(p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT)
  			ADDI(fp, R8, p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT); 


		if(pps[0] == 2*leafN) {
			CALL(fp, x_4_addr);
	//  	}else if(!pps[2]){
	//	  //uint32_t *x_8_t_addr = fp;
	//		memcpy(fp, neon_x8_t, neon_ee - neon_x8_t);
	//		fp += (neon_ee - neon_x8_t) / 4;
	//		//*fp++ = BL(fp+2, x_8_t_addr);
		}else{
			CALL(fp, x_8_addr);
		}

		pAddr = pps[1] * 4;
		if(pps[0] > leafN) 
			pN = pps[0];
		pLUT = p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8;//LUT_offset(pps[0], leafN);
//	fprintf(stderr, "LUT offset for %d is %d\n", pN, pLUT); 
		count += 4;
		pps += 2;
	}
	POP(fp, R15);
	POP(fp, R14);
	POP(fp, R13);
	POP(fp, R12);
	POP(fp, R11);
	POP(fp, R10);
	POP(fp, RBX);
	POP(fp, RBP);
	RET(fp);	

}
