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
#include "arch_codegen.h"
#include "macros.h"
#include "ffts.h"

#ifdef __APPLE__
	#include <libkern/OSCacheControl.h>
#endif

#include <sys/types.h>
#include <sys/mman.h>

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
			#ifdef __arm__
				if(N <= 32) lut_size += n/4 * 2 * sizeof(cdata_t);
			else lut_size += n/4 * sizeof(cdata_t);
			#else
				lut_size += n/4 * 2 * sizeof(cdata_t);
			#endif
			//	n *= 2;
			} else {
			#ifdef __arm__ 
				lut_size += n/8 * 3 * sizeof(cdata_t);
			#else
				lut_size += n/8 * 3 * 2 * sizeof(cdata_t);
			#endif
			}
			n *= 2;
		}
	return lut_size;
}


void ffts_generate_func_code(ffts_plan_t *p, size_t N, size_t leafN, int sign) {
	int count = tree_count(N, leafN, 0) + 1;
	size_t *ps = malloc(count * 2 * sizeof(size_t));
	size_t *pps = ps;

#ifdef __x86_64__
	if(sign < 0) p->constants = sse_constants;
	else         p->constants = sse_constants_inv;
#endif

	elaborate_tree(&pps, N, leafN, 0);
	pps[0] = 0;
	pps[1] = 0;

	pps = ps;

	define_transform_size(p, N);
#ifdef __arm__ 
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
insns_t *x_8_addr = generate_size8_base_case(&fp, sign);
//fprintf(stderr, "X8 start address = %016p\n", x_8_addr);


#ifdef __arm__
#ifdef HAVE_NEON
	memcpy(fp, neon_x8, neon_x8_t - neon_x8);
	/* 
	 * Changes adds to subtracts and  vice versa to allow the computation 
	 * o the IFFT and FFT
	 */
	if(sign < 0) {
		fp[31] ^= 0x00200000; fp[32] ^= 0x00200000; fp[33] ^= 0x00200000; fp[34] ^= 0x00200000;
		fp[65] ^= 0x00200000; fp[66] ^= 0x00200000; fp[70] ^= 0x00200000; fp[74] ^= 0x00200000;
		fp[97] ^= 0x00200000; fp[98] ^= 0x00200000; fp[102] ^= 0x00200000; fp[104] ^= 0x00200000;
	}
	fp += (neon_x8_t - neon_x8) / 4;
#else
	memcpy(fp, vfp_x8, vfp_end - vfp_x8);
	if(sign > 0) {
		fp[65] ^= 0x00000040; 
		fp[66] ^= 0x00000040; 
		fp[68] ^= 0x00000040; 
		fp[70] ^= 0x00000040; 
		fp[103] ^= 0x00000040; 
		fp[104] ^= 0x00000040; 
		fp[105] ^= 0x00000040; 
		fp[108] ^= 0x00000040; 
		fp[113] ^= 0x00000040; 
		fp[114] ^= 0x00000040; 
		fp[117] ^= 0x00000040; 
		fp[118] ^= 0x00000040; 
	}
	fp += (vfp_end - vfp_x8) / 4;
#endif
#endif
//uint32_t *x_8_t_addr = fp;
//memcpy(fp, neon_x8_t, neon_end - neon_x8_t);
//fp += (neon_end - neon_x8_t) / 4;
	

	insns_t *x_4_addr = generate_size4_base_case(&fp , sign);

	
#ifdef __arm__
	#ifdef have_neon
		memcpy(fp, neon_x4, neon_x8 - neon_x4);
		if(sign < 0) {
			fp[26] ^= 0x00200000; fp[28] ^= 0x00200000; fp[31] ^= 0x00200000; fp[32] ^= 0x00200000;
		}
		fp += (neon_x8 - neon_x4) / 4;
	#else
		memcpy(fp, vfp_x4, vfp_x8 - vfp_x4);
		if(sign > 0) {
			fp[36] ^= 0x00000040; 
			fp[38] ^= 0x00000040; 
			fp[43] ^= 0x00000040; 
			fp[44] ^= 0x00000040;
		}
		fp += (vfp_x8 - vfp_x4) / 4;
	#endif
#endif
	insns_t *start = generate_start_init(&fp, p);

#ifdef __arm__ 
	*fp = PUSH_LR(); fp++;
	*fp = 0xed2d8b10; fp++;

	ADDI(&fp, 3, 1, 0);
	ADDI(&fp, 7, 1, N);
	ADDI(&fp, 5, 1, 2*N);
	ADDI(&fp, 10, 7, 2*N);
	ADDI(&fp, 4, 5, 2*N);
	ADDI(&fp, 8, 10, 2*N);
	ADDI(&fp, 6, 4, 2*N);
	ADDI(&fp, 9, 8, 2*N);
	
  *fp = LDRI(12, 0, ((uint32_t)&p->offsets) - ((uint32_t)p)); fp++; // load offsets into r12
//  *fp++ = LDRI(1, 0, 4); // load ws into r1
	ADDI(&fp, 1, 0, 0);

	ADDI(&fp, 0, 2, 0), // mov out into r0
#endif


#ifdef __arm__
		*fp = LDRI(2, 1, ((uint32_t)&p->ee_ws) - ((uint32_t)p)); fp++; 
  	#ifdef HAVE_NEON
  		MOVI(&fp, 11, p->i0);
  	#else 
  		MOVI(&fp, 11, p->i0);
  	#endif

#endif
	generate_start(&fp, p , x_4_addr, x_8_addr, leafN, N , pps);	
	
#ifdef __arm__
	#ifdef HAVE_NEON
			memcpy(fp, neon_ee, neon_oo - neon_ee);
			if(sign < 0) {
				fp[33] ^= 0x00200000; fp[37] ^= 0x00200000; fp[38] ^= 0x00200000; fp[39] ^= 0x00200000;
				fp[40] ^= 0x00200000; fp[41] ^= 0x00200000; fp[44] ^= 0x00200000; fp[45] ^= 0x00200000;
				fp[46] ^= 0x00200000; fp[47] ^= 0x00200000; fp[48] ^= 0x00200000; fp[57] ^= 0x00200000;
			}
			fp += (neon_oo - neon_ee) / 4;
	#else
				memcpy(fp, vfp_e, vfp_o - vfp_e);
				if(sign > 0) {
					fp[64] ^= 0x00000040; fp[65] ^= 0x00000040; fp[68] ^= 0x00000040; fp[75] ^= 0x00000040;
					fp[76] ^= 0x00000040; fp[79] ^= 0x00000040; fp[80] ^= 0x00000040; fp[83] ^= 0x00000040;
					fp[84] ^= 0x00000040; fp[87] ^= 0x00000040; fp[91] ^= 0x00000040; fp[93] ^= 0x00000040;
				}
				fp += (vfp_o - vfp_e) / 4;
	#endif
#endif
#ifdef __arm__
	#ifdef HAVE_NEON
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
				if(sign < 0) {
					fp[12] ^= 0x00200000; fp[13] ^= 0x00200000; fp[14] ^= 0x00200000; fp[15] ^= 0x00200000;
					fp[27] ^= 0x00200000; fp[29] ^= 0x00200000; fp[30] ^= 0x00200000; fp[31] ^= 0x00200000;
					fp[46] ^= 0x00200000; fp[47] ^= 0x00200000; fp[48] ^= 0x00200000; fp[57] ^= 0x00200000;
				}
				fp += (neon_eo - neon_oo) / 4;
			}
			
			*fp = LDRI(11, 1, ((uint32_t)&p->oe_ws) - ((uint32_t)p)); fp++; 

			memcpy(fp, neon_oe, neon_end - neon_oe);
			if(sign < 0) {
				fp[19] ^= 0x00200000; fp[20] ^= 0x00200000; fp[22] ^= 0x00200000; fp[23] ^= 0x00200000;
				fp[37] ^= 0x00200000; fp[38] ^= 0x00200000; fp[40] ^= 0x00200000; fp[41] ^= 0x00200000;
				fp[64] ^= 0x00200000; fp[65] ^= 0x00200000; fp[66] ^= 0x00200000; fp[67] ^= 0x00200000;
			}
			fp += (neon_end - neon_oe) / 4;

		}else{
			
			*fp = LDRI(11, 1, ((uint32_t)&p->eo_ws) - ((uint32_t)p)); fp++; 

			memcpy(fp, neon_eo, neon_oe - neon_eo);
			if(sign < 0) {
				fp[10] ^= 0x00200000; fp[11] ^= 0x00200000; fp[13] ^= 0x00200000; fp[14] ^= 0x00200000;
				fp[31] ^= 0x00200000; fp[33] ^= 0x00200000; fp[34] ^= 0x00200000; fp[35] ^= 0x00200000;
				fp[59] ^= 0x00200000; fp[60] ^= 0x00200000; fp[61] ^= 0x00200000; fp[62] ^= 0x00200000;
			}
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
				if(sign < 0) {
					fp[12] ^= 0x00200000; fp[13] ^= 0x00200000; fp[14] ^= 0x00200000; fp[15] ^= 0x00200000;
					fp[27] ^= 0x00200000; fp[29] ^= 0x00200000; fp[30] ^= 0x00200000; fp[31] ^= 0x00200000;
					fp[46] ^= 0x00200000; fp[47] ^= 0x00200000; fp[48] ^= 0x00200000; fp[57] ^= 0x00200000;
				}
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

			*fp = LDRI(2, 1, ((uint32_t)&p->ee_ws) - ((uint32_t)p)); fp++; 
		  MOVI(&fp, 11, p->i1);
		memcpy(fp, neon_ee, neon_oo - neon_ee);
		if(sign < 0) {
			fp[33] ^= 0x00200000; fp[37] ^= 0x00200000; fp[38] ^= 0x00200000; fp[39] ^= 0x00200000;
			fp[40] ^= 0x00200000; fp[41] ^= 0x00200000; fp[44] ^= 0x00200000; fp[45] ^= 0x00200000;
			fp[46] ^= 0x00200000; fp[47] ^= 0x00200000; fp[48] ^= 0x00200000; fp[57] ^= 0x00200000;
		}
			fp += (neon_oo - neon_ee) / 4;

		}
	#else
		ADDI(&fp, 2, 7, 0);
		ADDI(&fp, 7, 9, 0);
		ADDI(&fp, 9, 2, 0);

		ADDI(&fp, 2, 8, 0);
		ADDI(&fp, 8, 10, 0);
		ADDI(&fp, 10, 2, 0);
	
			MOVI(&fp, 11, (p->i1>0) ? p->i1 : 1);
  		memcpy(fp, vfp_o, vfp_x4 - vfp_o);
		if(sign > 0) {
			fp[22] ^= 0x00000040; fp[24] ^= 0x00000040; fp[25] ^= 0x00000040; fp[26] ^= 0x00000040;
			fp[62] ^= 0x00000040; fp[64] ^= 0x00000040; fp[65] ^= 0x00000040; fp[66] ^= 0x00000040;
		}
  		fp += (vfp_x4 - vfp_o) / 4;
		
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

		*fp = LDRI(2, 1, ((uint32_t)&p->ee_ws) - ((uint32_t)p)); fp++; 
	  MOVI(&fp, 11, (p->i2>0) ? p->i2 : 1);
  	memcpy(fp, vfp_e, vfp_o - vfp_e);
		if(sign > 0) {
			fp[64] ^= 0x00000040; fp[65] ^= 0x00000040; fp[68] ^= 0x00000040; fp[75] ^= 0x00000040;
			fp[76] ^= 0x00000040; fp[79] ^= 0x00000040; fp[80] ^= 0x00000040; fp[83] ^= 0x00000040;
			fp[84] ^= 0x00000040; fp[87] ^= 0x00000040; fp[91] ^= 0x00000040; fp[93] ^= 0x00000040;
		}
  	fp += (vfp_o - vfp_e) / 4;

	#endif
  *fp = LDRI(2, 1, ((uint32_t)&p->ws) - ((uint32_t)p)); fp++; // load offsets into r12
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
        *fp = BL(fp+2, x_4_addr); fp++;
  	}else if(!pps[2]){
  	  //uint32_t *x_8_t_addr = fp;
#ifdef HAVE_NEON
  		memcpy(fp, neon_x8_t, neon_ee - neon_x8_t);
  		if(sign < 0) {
  			fp[31] ^= 0x00200000; fp[32] ^= 0x00200000; fp[33] ^= 0x00200000; fp[34] ^= 0x00200000;
  			fp[65] ^= 0x00200000; fp[66] ^= 0x00200000; fp[70] ^= 0x00200000; fp[74] ^= 0x00200000;
   			fp[97] ^= 0x00200000; fp[98] ^= 0x00200000; fp[102] ^= 0x00200000; fp[104] ^= 0x00200000;
  		}
  		fp += (neon_ee - neon_x8_t) / 4;
  		//*fp++ = BL(fp+2, x_8_t_addr);

#else
    		*fp = BL(fp+2, x_8_addr); fp++;
#endif
  	}else{
    		*fp = BL(fp+2, x_8_addr); fp++;
  	}

		pAddr = pps[1] * 4;
		pN = pps[0];
		pLUT = p->ws_is[__builtin_ctzl(pps[0]/leafN)-1]*8;//LUT_offset(pps[0], leafN);
//	fprintf(stderr, "LUT offset for %d is %d\n", pN, pLUT); 
		count += 4;
		pps += 2;
	}
	
	*fp++ = 0xecbd8b10;
	*fp++ = POP_LR(); count++;

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
#elif __linux__
#ifdef __GNUC__
	__clear_cache((long)(func), (long)(func) + p->transform_size);
#endif
#endif
//fprintf(stderr, "size of transform %zu = %d\n", N, (fp-func)*4);

	p->transform = (void *) (start);
}
