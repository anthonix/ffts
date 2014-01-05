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
#include "ffts.h"
#include "macros.h"
//#include "mini_macros.h"
#include "patterns.h"
#include "ffts_small.h"

#ifdef DYNAMIC_DISABLED
	#include "ffts_static.h"
#else
	#include "codegen.h"
#endif

#include <errno.h>
  #include <sys/mman.h>
  #include <string.h>
  #include <limits.h>	   /* for PAGESIZE */

#if __APPLE__
  #include <libkern/OSCacheControl.h>
#else
#endif

void ffts_execute(ffts_plan_t *p, const void *  in, void *  out) {
	p->transform(p, (const float *)in, (float *)out);
}

void ffts_free(ffts_plan_t *p) {
	p->destroy(p);
}

void ffts_free_1d(ffts_plan_t *p) {
	
	size_t i;

	if(p->ws) {
		FFTS_FREE(p->ws);
	}
	if(p->is) free(p->is);
	if(p->ws_is) free(p->ws_is);
	if(p->offsets)		free(p->offsets);
	//free(p->transforms);
	if(p->transforms) free(p->transforms);

	if(p->transform_base) {
		if (mprotect(p->transform_base, p->transform_size, PROT_READ | PROT_WRITE)) {
			perror("Couldn't mprotect");
			exit(errno);
		}
		munmap(p->transform_base, p->transform_size);
		//free(p->transform_base);
	}
	free(p);
}

ffts_plan_t *ffts_init_1d(size_t N, int sign) {
	ffts_plan_t *p = malloc(sizeof(ffts_plan_t));
	size_t leafN = 8;	
	size_t i;	

#ifdef __arm__
//#ifdef HAVE_NEON
	V MULI_SIGN;
	
	if(sign < 0) MULI_SIGN = VLIT4(-0.0f, 0.0f, -0.0f, 0.0f);
	else         MULI_SIGN = VLIT4(0.0f, -0.0f, 0.0f, -0.0f);
//#endif 
#else
	V MULI_SIGN;
	
	if(sign < 0) MULI_SIGN = VLIT4(-0.0f, 0.0f, -0.0f, 0.0f);
	else         MULI_SIGN = VLIT4(0.0f, -0.0f, 0.0f, -0.0f);
#endif

	p->transform = NULL;
	p->transform_base = NULL;
	p->transforms = NULL;
	p->is = NULL;
	p->ws_is = NULL;
	p->ws = NULL;
	p->offsets = NULL;
	p->destroy = ffts_free_1d;

	if(N >= 32) {
		ffts_init_offsets(p, N, leafN);
#ifdef __arm__
#ifdef HAVE_NEON
		ffts_init_is(p, N, leafN, 1);
#else
		ffts_init_is(p, N, leafN, 1);
#endif
#else
		ffts_init_is(p, N, leafN, 1);
#endif
		
		p->i0 = N/leafN/3+1;
		p->i1 = N/leafN/3;
		if((N/leafN) % 3 > 1) p->i1++;
		p->i2 = N/leafN/3;
		
		#ifdef __arm__	
		#ifdef HAVE_NEON
		p->i0/=2;
		p->i1/=2;
		#endif
		#else
		p->i0/=2;
		p->i1/=2;
		#endif

	}else{
		p->transforms = malloc(2 * sizeof(transform_index_t));
		p->transforms[0] = 0;
		p->transforms[1] = 1;
		if(N == 2) p->transform = &firstpass_2;
		else if(N == 4 && sign == -1) p->transform = &firstpass_4_f;
		else if(N == 4 && sign == 1) p->transform = &firstpass_4_b;
		else if(N == 8 && sign == -1) p->transform = &firstpass_8_f;
		else if(N == 8 && sign == 1) p->transform = &firstpass_8_b;
		else if(N == 16 && sign == -1) p->transform = &firstpass_16_f;
		else if(N == 16 && sign == 1) p->transform = &firstpass_16_b;

		p->is = NULL;
		p->offsets = NULL;
	}

		int hardcoded = 0;

		/*      LUTS           */
		size_t n_luts = __builtin_ctzl(N/leafN);
		if(N < 32) { n_luts = __builtin_ctzl(N/4); hardcoded = 1; }

		if(n_luts >= 32) n_luts = 0;

//		fprintf(stderr, "n_luts = %zu\n", n_luts);
		
		cdata_t *w;

		int n = leafN*2;
		if(hardcoded) n = 8;
		
		size_t lut_size = 0;

		for(i=0;i<n_luts;i++) {
			if(!i || hardcoded) {
			#ifdef __arm__ 
				if(N <= 32) lut_size += n/4 * 2 * sizeof(cdata_t);
				else lut_size += n/4 * sizeof(cdata_t);
			#else
				lut_size += n/4 * 2 * sizeof(cdata_t);
			#endif
				n *= 2;
			} else {
			#ifdef __arm__
				lut_size += n/8 * 3 * sizeof(cdata_t);
			#else
				lut_size += n/8 * 3 * 2 * sizeof(cdata_t);
			#endif
			}
			n *= 2;
		}
		
//		lut_size *= 16;
		
	//	fprintf(stderr, "lut size = %zu\n", lut_size);
		if(n_luts) {
			p->ws = FFTS_MALLOC(lut_size,32);
			p->ws_is = malloc(n_luts * sizeof(size_t));
		}else{
			p->ws = NULL;
			p->ws_is = NULL;
		}
		w = p->ws;

		n = leafN*2;
		if(hardcoded) n = 8;
		
		#ifdef HAVE_NEON
			V neg = (sign < 0) ? VLIT4(0.0f, 0.0f, 0.0f, 0.0f) : VLIT4(-0.0f, -0.0f, -0.0f, -0.0f);
		#endif
		
		for(i=0;i<n_luts;i++) {
			p->ws_is[i] = w - (cdata_t *)p->ws;	
			//fprintf(stderr, "LUT[%zu] = %d @ %08x - %zu\n", i, n, w, p->ws_is[i]);	
			
			if(!i || hardcoded) {
				cdata_t *w0 = FFTS_MALLOC(n/4 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/4;j++) {
					w0[j][0]	= W_re(n,j);
					w0[j][1]	= W_im(n,j);
				}


				float *fw0 = (float *)w0;
				#ifdef __arm__
					if(N < 32) {
						//w = FFTS_MALLOC(n/4 * 2 * sizeof(cdata_t), 32);
						float *fw = (float *)w;
						V temp0, temp1, temp2;
						for(j=0;j<n/4;j+=2) {
						//	#ifdef HAVE_NEON
							temp0 = VLD(fw0 + j*2);
							V re, im;
							re = VDUPRE(temp0);
							im = VDUPIM(temp0);
							#ifdef HAVE_NEON 
								im = VXOR(im, MULI_SIGN);
								//im = IMULI(sign>0, im);
							#else
								im = MULI(sign>0, im);
							#endif
							VST(fw + j*4  , re);
							VST(fw + j*4+4, im);
					//		#endif
						}
						w += n/4 * 2;
					}else{
						//w = FFTS_MALLOC(n/4 * sizeof(cdata_t), 32);
						float *fw = (float *)w;
						#ifdef HAVE_NEON
							VS temp0, temp1, temp2;
							for(j=0;j<n/4;j+=4) {
								temp0 = VLD2(fw0 + j*2);
								temp0.val[1] = VXOR(temp0.val[1], neg);
								STORESPR(fw + j*2, temp0);
							}
						#else
							for(j=0;j<n/4;j+=1) {
								fw[j*2] = fw0[j*2];
								fw[j*2+1] = (sign < 0) ? fw0[j*2+1] : -fw0[j*2+1];
							}
						#endif
						w += n/4;
					}
				#else
					//w = FFTS_MALLOC(n/4 * 2 * sizeof(cdata_t), 32);
					float *fw = (float *)w;
					V temp0, temp1, temp2;
					for(j=0;j<n/4;j+=2) {
						temp0 = VLD(fw0 + j*2);
						V re, im;
						re = VDUPRE(temp0);
						im = VDUPIM(temp0);
						im = VXOR(im, MULI_SIGN);
						VST(fw + j*4  , re);
						VST(fw + j*4+4, im);
					}
					w += n/4 * 2;
				#endif

				FFTS_FREE(w0);
			}else{

				cdata_t *w0 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);
				cdata_t *w1 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);
				cdata_t *w2 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/8;j++) {
					w0[j][0]	= W_re(n,j*2);
					w0[j][1]	= W_im(n,j*2);
					w1[j][0]	= W_re(n,j);
					w1[j][1]	= W_im(n,j);
					w2[j][0]	= W_re(n,j + (n/8));
					w2[j][1]	= W_im(n,j + (n/8));

				}

				float *fw0 = (float *)w0;
				float *fw1 = (float *)w1;
				float *fw2 = (float *)w2;
				#ifdef __arm__
					//w = FFTS_MALLOC(n/8 * 3 * sizeof(cdata_t), 32);
					float *fw = (float *)w;
					#ifdef HAVE_NEON	
						VS temp0, temp1, temp2;
						for(j=0;j<n/8;j+=4) {
							temp0 = VLD2(fw0 + j*2);
							temp0.val[1] = VXOR(temp0.val[1], neg);
							STORESPR(fw + j*2*3,      temp0);
							temp1 = VLD2(fw1 + j*2);
							temp1.val[1] = VXOR(temp1.val[1], neg);
							STORESPR(fw + j*2*3 + 8,  temp1);
							temp2 = VLD2(fw2 + j*2);
							temp2.val[1] = VXOR(temp2.val[1], neg);
							STORESPR(fw + j*2*3 + 16, temp2);
						}
					#else
						for(j=0;j<n/8;j+=1) {
								fw[j*6] = fw0[j*2];
								fw[j*6+1] = (sign < 0) ? fw0[j*2+1] : -fw0[j*2+1];
								fw[j*6+2] = fw1[j*2+0];
								fw[j*6+3] = (sign < 0) ? fw1[j*2+1] : -fw1[j*2+1];
								fw[j*6+4] = fw2[j*2+0];
								fw[j*6+5] = (sign < 0) ? fw2[j*2+1] : -fw2[j*2+1];
						}
					#endif
					w += n/8 * 3;
				#else
					//w = FFTS_MALLOC(n/8 * 3 * 2 * sizeof(cdata_t), 32);
					float *fw = (float *)w;
					V temp0, temp1, temp2, re, im;
					for(j=0;j<n/8;j+=2) {
						temp0 = VLD(fw0 + j*2);
						re = VDUPRE(temp0);
						im = VDUPIM(temp0);
						im = VXOR(im, MULI_SIGN);
						VST(fw + j*2*6  , re);
						VST(fw + j*2*6+4, im);

						temp1 = VLD(fw1 + j*2);
						re = VDUPRE(temp1);
						im = VDUPIM(temp1);
						im = VXOR(im, MULI_SIGN);
						VST(fw + j*2*6+8 , re);
						VST(fw + j*2*6+12, im);

						temp2 = VLD(fw2 + j*2);
						re = VDUPRE(temp2);
						im = VDUPIM(temp2);
						im = VXOR(im, MULI_SIGN);
						VST(fw + j*2*6+16, re);
						VST(fw + j*2*6+20, im);
					}
					w += n/8 * 3 * 2;
				#endif

				FFTS_FREE(w0);
				FFTS_FREE(w1);
				FFTS_FREE(w2);
			}
			///p->ws[i] = w;

			n *= 2;
		}

	float *tmp = (float *)p->ws;

	if(sign < 0) {
		p->oe_ws = (void *)(&w_data[4]);
		p->ee_ws = (void *)(w_data);
		p->eo_ws = (void *)(&w_data[4]);
	}else{
		p->oe_ws = (void *)(w_data + 12);
		p->ee_ws = (void *)(w_data + 8);
		p->eo_ws = (void *)(w_data + 12);
	}

	p->N = N;
	p->lastlut = w;
	p->n_luts = n_luts;
#ifdef DYNAMIC_DISABLED
	if(sign < 0) { 
		if(N >= 32) p->transform = ffts_static_transform_f; 
	}else{
		if(N >= 32) p->transform = ffts_static_transform_i; 
	}

#else
	if(N>=32)  ffts_generate_func_code(p, N, leafN, sign);
#endif

	return p;
}

// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
