#include "codegen.h"
#include "macros.h"
#include "neon_float.h"
#include "neon.h"

int tree_count(int N, int leafN, int offset) {
	
	if(N <= leafN) return 0;
	int count = 0;	
	count += tree_count(N/4, leafN, offset);
	count += tree_count(N/8, leafN, offset + N/4);
	count += tree_count(N/8, leafN, offset + N/4 + N/8);
	count += tree_count(N/4, leafN, offset + N/2);
	count += tree_count(N/4, leafN, offset + 3*N/4);

	return 1 + count;
}

void elaborate_tree(size_t **p, int N, int leafN, int offset) {
	
	if(N <= leafN) return;
	elaborate_tree(p, N/4, leafN, offset);
	elaborate_tree(p, N/8, leafN, offset + N/4);
	elaborate_tree(p, N/8, leafN, offset + N/4 + N/8);
	elaborate_tree(p, N/4, leafN, offset + N/2);
	elaborate_tree(p, N/4, leafN, offset + 3*N/4);

	(*p)[0] = N;
	(*p)[1] = offset*2;

	(*p)+=2;
}

void 
ffts_x_8(float *data, size_t N, float *LUT) {
	X_8_SPLIT(data, N, LUT);
}
void 
ffts_x_8_t(float *data, size_t N, float *LUT) {
	X_8_SPLIT_T(data, N, LUT);
}
void 
ffts_x_4(float *data, size_t N, float *LUT) {
	//fprintf(stderr, "X_4 %zu\n", N);
	X_4_SPLIT(data, N, LUT);
}

void 
dummy(float *data, size_t N, float *LUT) {
}

uint32_t BL(void *pos, void *target) {
	return 0xeb000000 | (((target - pos) / 4) & 0xffffff);
}

uint32_t B(uint8_t r) {
	return 0xe12fff10 | r;
}

uint32_t MOV(uint8_t dst, uint8_t src) {
		return 0xe1a00000 | (src & 0xf) | ((dst & 0xf) << 12);
}

uint32_t ADDI(uint8_t dst, uint8_t src, int32_t imm) {
	if(imm < 0) {
		imm = -imm;
		uint32_t shamt = (__builtin_ctzl(imm)>15)?15:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;
		return 0xe2400000 | ((src & 0xf) << 16) | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff);

	}
	uint32_t shamt = (__builtin_ctzl(imm)>15)?15:__builtin_ctzl(imm);
	fprintf(stderr, "pre ADDI shamt:%d imm:%d\n", shamt, imm);
	if(shamt & 1) shamt -= 1;
	fprintf(stderr, "ADDI shamt:%d imm:%d\n", shamt, imm);
	imm >>= shamt;
	shamt = (32 - shamt)/2;
	
	
	return 0xe2800000 | ((src & 0xf) << 16) | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff);
}

uint32_t MOVI(uint8_t dst, uint16_t val) {
	return 0xe3a00000 | ((dst & 0xf) << 12) | (val & 0xffff) ;
}

uint32_t PUSH_LR() { return 0xe92d4000; }
uint32_t POP_LR() { return 0xe8bd8000; }

uint32_t LUT_offset(size_t N, size_t leafN) {
		int i;
		size_t p_lut_size = 0;
		size_t lut_size = 0;
		int hardcoded = 0;
		size_t n_luts = __builtin_ctzl(N/leafN);
		int n = leafN*2;
		//if(N <= 32) { n_luts = __builtin_ctzl(N/4); hardcoded = 1; }
		
		for(i=0;i<n_luts-1;i++) {
			p_lut_size = lut_size;
			if(!i || hardcoded) {
			#ifdef __ARM_NEON__
				if(N <= 32) lut_size += n/4 * 2 * sizeof(cdata_t);
			else lut_size += n/4 * sizeof(cdata_t);
			#else
				lut_size += n/4 * 2 * sizeof(cdata_t);
			#endif
			//	n *= 2;
			} else {
			#ifdef __ARM_NEON__
				lut_size += n/8 * 3 * sizeof(cdata_t);
			#else
				lut_size += n/8 * 3 * 2 * sizeof(cdata_t);
			#endif
			}
			n *= 2;
		}
	return lut_size;
}

transform_func_t ffts_generate_func_code(ffts_plan_t *p, size_t N, size_t leafN) {
	int count = tree_count(N, leafN, 0) + 1;
	size_t *ps = malloc(count * 2 * sizeof(size_t));
	size_t *pps = ps;

	elaborate_tree(&pps, N, leafN, 0);
	pps[0] = 0;
	pps[1] = 0;

	pps = ps;

	uint32_t *func = valloc(8192);
	uint32_t *fp = func;

	//p->transform_base = func;

	uint32_t *x_8_addr = fp;
	memcpy(fp, neon_x8, neon_x8_t - neon_x8);
	fp += (neon_x8_t - neon_x8) / 4;
	uint32_t *x_8_t_addr = fp;
	memcpy(fp, neon_x8_t, neon_end - neon_x8_t);
	fp += (neon_end - neon_x8_t) / 4;
	uint32_t *x_4_addr = fp;
	memcpy(fp, neon_x4, neon_x8 - neon_x4);
	fp += (neon_x8 - neon_x4) / 4;

	uint32_t *start = fp;

	fprintf(stderr, "X_4: %08x START: %08x\n", x_4_addr, start);
	fprintf(stderr, "X_8: %08x\n", x_8_addr, start);
	fprintf(stderr, "X_8_T: %08x\n", x_8_t_addr, start);
	fprintf(stderr, "LUT: %08x\n", p->ws);
	*fp++ = PUSH_LR();
//	*fp++ = MOV(2, 1);
//	*fp++ = BL(fp+2, start);

	int32_t pAddr = 0;
	int32_t pN = 0;
	int32_t pLUT = 0;
	count = 2;
	while(pps[0]) {
	
		fprintf(stderr, "size %zu at %zu - diff %zu\n", pps[0], pps[1]*4, (pps[1]*4) - pAddr);
		if(!pN) {
			*fp++ = MOVI(1, pps[0]);
		}else{
  		*fp++ = ADDI(0, 0, (pps[1] * 4)- pAddr);
  		*fp++ = ADDI(1, 1, pps[0] - pN);
		}
		//*fp++ = ADDI(2, 2, LUT_offset(pps[0], leafN) - pLUT); 
		*fp++ = ADDI(2, 2, p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT); 


  	if(pps[0] == 2*leafN) {
      *fp++ = BL(fp+2, x_4_addr);
  	}else if(!pps[2]){
  		*fp++ = BL(fp+2, x_8_t_addr);
  	}else{
  		*fp++ = BL(fp+2, x_8_addr);
  	}

		pAddr = pps[1] * 4;
		pN = pps[0];
		pLUT = p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8;//LUT_offset(pps[0], leafN);
		fprintf(stderr, "LUT offset for %d is %d\n", pN, pLUT); 
		count += 4;
		pps += 2;
	}
	
	*fp++ = POP_LR(); count++;
//	*fp++ = B(14); count++;

	for(int i=0;i<(neon_x8 - neon_x4)/4;i++) 
		fprintf(stderr, "%08x\n", x_4_addr[i]);
	fprintf(stderr, "\n");
	for(int i=0;i<count;i++) 
		fprintf(stderr, "%08x\n", start[i]);

	free(ps);
	
	if (mprotect(func, 8192, PROT_READ | PROT_EXEC)) {
		perror("Couldn't mprotect");
		return NULL;
	}


	return (transform_func_t)start;
}
