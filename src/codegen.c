/*
 
 This file is part of FFTS -- The Fastest Fourier Transform in the South
  
 Copyright (c) 2012, Anthony M. Blake <amb@anthonix.com>
 Copyright (c) 2012, The University of Waikato 
 
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 	* Redistributions of source code must retain the above copyright
 		notice, this list of conditions and the following disclaimer.
 	* Redistributions in binary form must reproduce the above copyright
 		notice, this list of conditions and the following disclaimer in the
 		documentation and/or other materials provided with the distribution.
 	* Neither the name of the organization nor the
	  names of its contributors may be used to endorse or promote products
 		derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL ANTHONY M. BLAKE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "codegen.h"
#include "macros.h"
//#include "mini_macros.h"
#include "cp_sse.h"

#ifdef __APPLE__
	#include <libkern/OSCacheControl.h>
#endif

#include <sys/types.h>
#include <sys/mman.h>

#ifdef __ARM_NEON__
	#include "codegen_neon.h"
//	#include "neon_float.h"
	#include "neon.h"
#else
	#include "codegen_sse.h"
	#include "sse_float.h"
#endif

#ifdef __ANDROID__
	#include <unistd.h>
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

void ffts_generate_func_code(ffts_plan_t *p, size_t N, size_t leafN) {
	int count = tree_count(N, leafN, 0) + 1;
	size_t *ps = malloc(count * 2 * sizeof(size_t));
	size_t *pps = ps;

#ifdef __x86_64__
	p->constants = sse_constants;
#endif

	elaborate_tree(&pps, N, leafN, 0);
	pps[0] = 0;
	pps[1] = 0;

	pps = ps;

#ifdef __ARM_NEON__
	if(N < 8192) p->transform_size = 8192;
	else p->transform_size = N;
#else
	if(N < 2048) p->transform_size = 16384;
	else p->transform_size = 16384 + 2*N/8 * __builtin_ctzl(N);
#endif

#ifdef __APPLE__
	p->transform_base = mmap(NULL, p->transform_size, PROT_WRITE | PROT_READ, MAP_ANON | MAP_SHARED, -1, 0);
#else
#define MAP_ANONYMOUS 0x20	
	p->transform_base = mmap(NULL, p->transform_size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
#endif

/*
	if(p->transform_base == MAP_FAILED) {
		fprintf(stderr, "MAP FAILED\n");
		exit(1);
	}*/
	insns_t *func = p->transform_base;//valloc(8192);
	insns_t *fp = func;

//fprintf(stderr, "Allocating %d bytes \n", p->transform_size);
//fprintf(stderr, "Base address = %016p\n", func);

	if(!func) { 
		fprintf(stderr, "NOMEM\n");
		exit(1);
	}

	insns_t *x_8_addr = fp;
#ifdef __ARM_NEON__
	memcpy(fp, neon_x8, neon_x8_t - neon_x8);
	fp += (neon_x8_t - neon_x8) / 4;
#else
	align_mem16(&fp, 0);
	x_8_addr = fp;
	align_mem16(&fp, 5);
	memcpy(fp, x8_soft, x8_hard - x8_soft);
	fp += (x8_hard - x8_soft);
//fprintf(stderr, "X8 start address = %016p\n", x_8_addr);
#endif
//uint32_t *x_8_t_addr = fp;
//memcpy(fp, neon_x8_t, neon_end - neon_x8_t);
//fp += (neon_end - neon_x8_t) / 4;
	insns_t *x_4_addr = fp;
#ifdef __ARM_NEON__
	memcpy(fp, neon_x4, neon_x8 - neon_x4);
	fp += (neon_x8 - neon_x4) / 4;
#else
	align_mem16(&fp, 0);
	x_4_addr = fp;
	memcpy(fp, x4, x8_soft - x4);
	fp += (x8_soft - x4);
//fprintf(stderr, "X4 start address = %016p\n", x_4_addr);
#endif
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
	align_mem16(&fp, 0);
	start = fp;
	
	*fp++ = 0x4c;
	*fp++ = 0x8b;
	*fp++ = 0x07;
	uint32_t lp_cnt = p->i0 * 4;
	MOVI(&fp, RCX, lp_cnt);
	
	//LEA(&fp, R8, RDI, ((uint32_t)&p->offsets) - ((uint32_t)p)); 
#endif
	//fp++;
#ifdef __ARM_NEON__
	memcpy(fp, neon_ee, neon_oo - neon_ee);
	fp += (neon_oo - neon_ee) / 4;
#else
//fprintf(stderr, "Body start address = %016p\n", start);
	
	PUSH(&fp, RBP);	
	PUSH(&fp, RBX);
	PUSH(&fp, R10);
	PUSH(&fp, R11);
	PUSH(&fp, R12);
	PUSH(&fp, R13);
	PUSH(&fp, R14);
	PUSH(&fp, R15);
	
	int i;
	memcpy(fp, leaf_ee_init, leaf_ee - leaf_ee_init);
	
//fprintf(stderr, "Leaf ee init address = %016p\n", leaf_ee_init);
//fprintf(stderr, "Constants address = %016p\n", sse_constants);
//fprintf(stderr, "Constants address = %016p\n", p->constants);
	
//int32_t val = READ_IMM32(fp + 3);
//fprintf(stderr, "diff = 0x%x\n", ((uint32_t)&p->constants) - ((uint32_t)p));

//int64_t v2 = val + (int64_t)((void *)leaf_ee_init - (void *)fp );
//fprintf(stderr, "IMM = 0x%llx\n", v2);

//IMM32_NI(fp + 3, ((int64_t) READ_IMM32(fp + 3)) + ((void *)leaf_ee_init - (void *)fp )); 
	fp += (leaf_ee - leaf_ee_init);

//fprintf(stderr, "Leaf start address = %016p\n", fp);
	align_mem16(&fp, 9);
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
			align_mem16(&fp, 4);
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
			align_mem16(&fp, 4);
  		memcpy(fp, leaf_oo, leaf_eo - leaf_oo);
  		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_oo_offsets[i], offsets_o[i]*4); 
  		fp += (leaf_eo - leaf_oo);
  	}

	}
	if(p->i1) {

		lp_cnt += p->i1 * 4;
		MOVI(&fp, RCX, lp_cnt);
		align_mem16(&fp, 9);
		memcpy(fp, leaf_ee, leaf_oo - leaf_ee);
		for(i=0;i<8;i++) IMM32_NI(fp + sse_leaf_ee_offsets[i], offsets_oe[i]*4); 
		fp += (leaf_oo - leaf_ee);

	}
	
//fprintf(stderr, "Body start address = %016p\n", fp);
  //LEA(&fp, R8, RDI, ((uint32_t)&p->ws) - ((uint32_t)p)); 
	memcpy(fp, x_init, x4 - x_init);
//IMM32_NI(fp + 3, ((int64_t)READ_IMM32(fp + 3)) + ((void *)x_init - (void *)fp )); 
	fp += (x4 - x_init);

	int32_t pAddr = 0;
	int32_t pN = 0;
	int32_t pLUT = 0;
	count = 2;
	while(pps[0]) {
	
		if(!pN) {
			MOVI(&fp, RCX, pps[0] / 4);
		}else{
  		if((pps[1]*4)-pAddr) ADDI(&fp, RDX, (pps[1] * 4)- pAddr);
			if(pps[0] > leafN && pps[0] - pN) {
				
				int diff = __builtin_ctzl(pps[0]) - __builtin_ctzl(pN);
				*fp++ = 0xc1; 
				
				if(diff > 0) {
					*fp++ = 0xe1;
					*fp++ = (diff & 0xff);
				}else{
					*fp++ = 0xe9;
					*fp++ = ((-diff) & 0xff);
				}
			}
		}
		
  		if(p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT)
  			ADDI(&fp, R8, p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8 - pLUT); 


  	if(pps[0] == 2*leafN) {
      CALL(&fp, x_4_addr);
//  	}else if(!pps[2]){
//	  //uint32_t *x_8_t_addr = fp;
//		memcpy(fp, neon_x8_t, neon_ee - neon_x8_t);
//		fp += (neon_ee - neon_x8_t) / 4;
//		//*fp++ = BL(fp+2, x_8_t_addr);
  	}else{
    		CALL(&fp, x_8_addr);
  	}

		pAddr = pps[1] * 4;
		if(pps[0] > leafN) 
			pN = pps[0];
		pLUT = p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8;//LUT_offset(pps[0], leafN);
//	fprintf(stderr, "LUT offset for %d is %d\n", pN, pLUT); 
		count += 4;
		pps += 2;
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
	POP(&fp, R15);
	POP(&fp, R14);
	POP(&fp, R13);
	POP(&fp, R12);
	POP(&fp, R11);
	POP(&fp, R10);
	POP(&fp, RBX);
	POP(&fp, RBP);
	RET(&fp);	


//uint8_t *pp = func;
//int counter = 0;
//do{ 
//	printf("%02x ", *pp);
//	if(counter++ % 16 == 15) printf("\n");
//} while(++pp < fp);

//printf("\n");


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
#ifdef __APPLE__
	sys_icache_invalidate(func, p->transform_size);
#elif __ANDROID__
	cacheflush((long)(func), (long)(func) + p->transform_size, 0);
#endif

//fprintf(stderr, "size of transform %zu = %d\n", N, (fp-func)*4);

	p->transform = start;
}
