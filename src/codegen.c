#include "codegen.h"
#include "macros.h"
//#include "mini_macros.h"
#include "cp_sse.h"
#include <libkern/OSCacheControl.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef __ARM_NEON__
	#include "codegen_neon.h"
	#include "neon_float.h"
	#include "neon.h"
#else
	#include "codegen_sse.h"
	#include "sse_float.h"
#endif

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

#ifdef __ARM_NEON__
	typedef uint32_t insns_t;
#else
	typedef uint8_t insns_t;
#endif

void ffts_generate_func_code(ffts_plan_t *p, size_t N, size_t leafN) {
	int count = tree_count(N, leafN, 0) + 1;
	size_t *ps = malloc(count * 2 * sizeof(size_t));
	size_t *pps = ps;

	elaborate_tree(&pps, N, leafN, 0);
	pps[0] = 0;
	pps[1] = 0;

	pps = ps;

	if(N < 8192) p->transform_size = 8192;
	else p->transform_size = N;

	p->transform_base = valloc(p->transform_size);//mmap(NULL, p->transform_size, PROT_WRITE | PROT_READ, MAP_ANON | MAP_SHARED, -1, 0);
	/*
	if(p->transform_base == MAP_FAILED) {
		fprintf(stderr, "MAP FAILED\n");
		exit(1);
	}*/

	insns_t *func = p->transform_base;//valloc(8192);
	insns_t *fp = func;

	fprintf(stderr, "Allocating %d bytes \n", p->transform_size);
	fprintf(stderr, "Base address = %016p\n", func);

	if(!func) { 
		fprintf(stderr, "NOMEM\n");
		exit(1);
	}

	insns_t *x_8_addr = fp;
	memcpy(fp, neon_x8, neon_x8_t - neon_x8);
	fp += (neon_x8_t - neon_x8) / 4;
//uint32_t *x_8_t_addr = fp;
//memcpy(fp, neon_x8_t, neon_end - neon_x8_t);
//fp += (neon_end - neon_x8_t) / 4;
	insns_t *x_4_addr = fp;
	memcpy(fp, neon_x4, neon_x8 - neon_x4);
	fp += (neon_x8 - neon_x4) / 4;

	insns_t *start = fp;

#ifdef __ARM_NEON__
	*fp++ = PUSH_LR();
	
	ADDI(&fp, 3, 1, 0);
	ADDI(&fp, 7, 1, N);
	ADDI(&fp, 5, 1, 2*N);
	ADDI(&fp, 10, 7, 2*N);
	ADDI(&fp, 4, 5, 2*N);
	ADDI(&fp, 8, 10, 2*N);
	ADDI(&fp, 6, 4, 2*N);
	ADDI(&fp, 9, 8, 2*N);
	
  *fp++ = LDRI(12, 0, ((uint32_t)&p->offsets) - ((uint32_t)p)); // load offsets into r12
//  *fp++ = LDRI(1, 0, 4); // load ws into r1
	ADDI(&fp, 1, 0, 0);

	ADDI(&fp, 0, 2, 0), // mov out into r0
#endif

	p->oe_ws = oe_w_data;
	p->ee_ws = ee_w_data;
	p->eo_ws = eo_w_data;


#ifdef __ARM_NEON__
	*fp++ = LDRI(2, 1, ((uint32_t)&p->ee_ws) - ((uint32_t)p)); 
	MOVI(&fp, 11, p->i0);
#else
	
	*fp++ = 0x4c;
	*fp++ = 0x8b;
	*fp++ = 0x07;
	uint32_t lp_cnt = p->i0 * 4;
	MOVI(&fp, RCX, lp_cnt);
	
	//LEA(&fp, R8, RDI, ((uint32_t)&p->offsets) - ((uint32_t)p)); 
#endif
	//fp++;
#ifdef __ARM_NEON__
	memcpy(fp, leaf_ee, leaf_oo - leaf_ee);
	fp += (neon_oo - leaf_ee) / 4;
#else
	int i;
	memcpy(fp, leaf_ee_init, leaf_ee - leaf_ee_init);
	IMM32_NI(fp + 3, READ_IMM32(fp + 3) + ((void *)leaf_ee_init - (void *)fp )); 
	fp += (leaf_ee - leaf_ee_init);

	memcpy(fp, leaf_ee, leaf_oo - leaf_ee);


	uint32_t offsets[8] = {0, N, N/2, 3*N/2, N/4, 5*N/4, 7*N/4, 3*N/4};
	uint32_t offsets_o[8] = {0, N, N/2, 3*N/2, 7*N/4, 3*N/4, N/4, 5*N/4};
	uint32_t offsets_oe[8] = {7*N/4, 3*N/4, N/4, 5*N/4, 0, N, 3*N/2, N/2};
	
	for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_ee_offsets[i], offsets[i]*4); 
	
	fp += (leaf_oo - leaf_ee);
	
	if(__builtin_ctzl(N) & 1){
		if(p->i1) {
			lp_cnt += p->i1 * 4;
  		MOVI(&fp, RCX, lp_cnt);
  		memcpy(fp, leaf_oo, leaf_eo - leaf_oo);
  		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_oo_offsets[i], offsets_o[i]*4); 
  		fp += (leaf_eo - leaf_oo);
		}
		

  	memcpy(fp, leaf_oe, leaf_end - leaf_oe);
  	lp_cnt += 4;
  	for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_oe_offsets[i], offsets_o[i]*4); 
  	fp += (leaf_end - leaf_oe);

	}else{


		memcpy(fp, leaf_eo, leaf_oe - leaf_eo);
		lp_cnt += 4;
		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_eo_offsets[i], offsets[i]*4); 
		fp += (leaf_oe - leaf_eo);

  	if(p->i1) {
			lp_cnt += p->i1 * 4;
  		MOVI(&fp, RCX, lp_cnt);
  		memcpy(fp, leaf_oo, leaf_eo - leaf_oo);
  		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_oo_offsets[i], offsets_o[i]*4); 
  		fp += (leaf_eo - leaf_oo);
  	}

	}
	if(p->i1) {

		lp_cnt += p->i1 * 4;
		MOVI(&fp, RCX, lp_cnt);
		memcpy(fp, leaf_ee, leaf_oo - leaf_ee);
		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_ee_offsets[i], offsets_oe[i]*4); 
		fp += (leaf_oo - leaf_ee);

	}
#endif
#ifdef __ARM_NEON__
	if(__builtin_ctzl(N) & 1){
		ADDI(&fp, 2, 7, 0);
		ADDI(&fp, 7, 9, 0);
		ADDI(&fp, 9, 2, 0);

		ADDI(&fp, 2, 8, 0);
		ADDI(&fp, 8, 10, 0);
		ADDI(&fp, 10, 2, 0);
	
		if(p->i1) {
			MOVI(&fp, 11, p->i1);
			memcpy(fp, neon_oo, neon_eo - neon_oo);
			fp += (neon_eo - neon_oo) / 4;
		}
		
		*fp++ = LDRI(11, 1, ((uint32_t)&p->oe_ws) - ((uint32_t)p)); 

		memcpy(fp, neon_oe, neon_end - neon_oe);
		fp += (neon_end - neon_oe) / 4;

	}else{
		
		*fp++ = LDRI(11, 1, ((uint32_t)&p->eo_ws) - ((uint32_t)p)); 

		memcpy(fp, neon_eo, neon_oe - neon_eo);
		fp += (neon_oe - neon_eo) / 4;
		
		ADDI(&fp, 2, 7, 0);
		ADDI(&fp, 7, 9, 0);
		ADDI(&fp, 9, 2, 0);

		ADDI(&fp, 2, 8, 0);
		ADDI(&fp, 8, 10, 0);
		ADDI(&fp, 10, 2, 0);
	
		if(p->i1) {
			MOVI(&fp, 11, p->i1);
			memcpy(fp, neon_oo, neon_eo - neon_oo);
			fp += (neon_eo - neon_oo) / 4;
		}

	}


	if(p->i1) {
		ADDI(&fp, 2, 3, 0);
		ADDI(&fp, 3, 7, 0);
		ADDI(&fp, 7, 2, 0);

		ADDI(&fp, 2, 4, 0);
		ADDI(&fp, 4, 8, 0);
		ADDI(&fp, 8, 2, 0);
		
		ADDI(&fp, 2, 5, 0);
		ADDI(&fp, 5, 9, 0);
		ADDI(&fp, 9, 2, 0);

		ADDI(&fp, 2, 6, 0);
		ADDI(&fp, 6, 10, 0);
		ADDI(&fp, 10, 2, 0);

		ADDI(&fp, 2, 9, 0);
		ADDI(&fp, 9, 10, 0);
		ADDI(&fp, 10, 2, 0);

		*fp++ = LDRI(2, 1, ((uint32_t)&p->ee_ws) - ((uint32_t)p)); 
	  MOVI(&fp, 11, p->i1);
		memcpy(fp, neon_ee, neon_oo - neon_ee);
		fp += (neon_oo - neon_ee) / 4;

	}
	
  *fp++ = LDRI(2, 1, ((uint32_t)&p->ws) - ((uint32_t)p)); // load offsets into r12
	//ADDI(&fp, 2, 1, 0);
	MOVI(&fp, 1, 0);
	
	// args: r0 - out
	//       r1 - N
	//       r2 - ws
//	ADDI(&fp, 3, 1, 0); // put N into r3 for counter

	int32_t pAddr = 0;
	int32_t pN = 0;
	int32_t pLUT = 0;
	count = 2;
	while(pps[0]) {
	
//	fprintf(stderr, "size %zu at %zu - diff %zu\n", pps[0], pps[1]*4, (pps[1]*4) - pAddr);
		if(!pN) {
			MOVI(&fp, 1, pps[0]);
		}else{
  		if((pps[1]*4)-pAddr) ADDI(&fp, 0, 0, (pps[1] * 4)- pAddr);
			if(pps[0] - pN) ADDI(&fp, 1, 1, pps[0] - pN);
		}
		
		if(p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT)
			ADDI(&fp, 2, 2, p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT); 


  	if(pps[0] == 2*leafN) {
      *fp++ = BL(fp+2, x_4_addr);
  	}else if(!pps[2]){
		  //uint32_t *x_8_t_addr = fp;
  		memcpy(fp, neon_x8_t, neon_ee - neon_x8_t);
  		fp += (neon_ee - neon_x8_t) / 4;
  		//*fp++ = BL(fp+2, x_8_t_addr);
  	}else{
  		*fp++ = BL(fp+2, x_8_addr);
  	}

		pAddr = pps[1] * 4;
		pN = pps[0];
		pLUT = p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8;//LUT_offset(pps[0], leafN);
//	fprintf(stderr, "LUT offset for %d is %d\n", pN, pLUT); 
		count += 4;
		pps += 2;
	}
	
	*fp++ = POP_LR(); count++;
#else
	RET(&fp);	


	uint8_t *pp = func;
	int counter = 0;
	do{ 
		printf("%02x ", *pp);
		if(counter++ % 16 == 15) printf("\n");
	} while(++pp < fp);

	printf("\n");


#endif


//	*fp++ = B(14); count++;

//for(int i=0;i<(neon_x8 - neon_x4)/4;i++) 
//	fprintf(stderr, "%08x\n", x_4_addr[i]);
//fprintf(stderr, "\n");
//for(int i=0;i<count;i++) 

	free(ps);
	
	if (mprotect(func, p->transform_size, PROT_READ | PROT_EXEC)) {
		perror("Couldn't mprotect");
		exit(1);
	}

	sys_icache_invalidate(func, p->transform_size);


	fprintf(stderr, "size of transform %zu = %d\n", N, (fp-func)*4);

	p->transform = start;
}
