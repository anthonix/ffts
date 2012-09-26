#include "cp_sse.h"
#include "macros.h"
//#include "mini_macros.h"
#include "patterns.h"
  #include <libkern/OSCacheControl.h>
  #include <errno.h>
  #include <sys/mman.h>
  #include <string.h>
  #include <limits.h>	   /* for PAGESIZE */


void ffts_execute(ffts_plan_t *p, const void * restrict in, void * restrict out) {
	transform_index_t *ps = p->transforms;
	//p->firstpass((const float *)in, (float *)out, p);
	p->transform(p, (const float *)in, (float *)out);
	//if(p->transform) p->transform(out, p->N, p->ws);
}

void ffts_free(ffts_plan_t *p) {
	
	size_t i;

	if(p->ws) {
		FFTS_FREE(p->ws);
	}
	if(p->is) free(p->is);
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

ffts_plan_t *ffts_init(size_t N, int sign) {
	ffts_plan_t *p = malloc(sizeof(ffts_plan_t));
	size_t leafN = 8;	
	size_t i;	
		
	if(sign < 0) MULI_SIGN = VLIT4(-0.0f, 0.0f, -0.0f, 0.0f);
	else         MULI_SIGN = VLIT4(0.0f, -0.0f, 0.0f, -0.0f);
	
	if(sign < 0) SCALAR_MULI_SIGN = -0.0f*I; 
	else         SCALAR_MULI_SIGN = -0.0f; 

	p->transform = NULL;
	p->transform_base = NULL;
	p->transforms = NULL;
	p->is = NULL;
	p->ws = NULL;
	p->offsets = NULL;


	if(N >= 32) {
		ffts_init_offsets(p, N, leafN);
		ffts_init_is(p, N, leafN, 2);
	//	ffts_init_tree(p, N, leafN);
		
	//	if(N == 64) p->firstpass = &firstpass_64;

		LEAFLUT[0] = VLIT4(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941);
		LEAFLUT[1] = VLIT4(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376);
		LEAFLUT[2] = VLIT4(0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011);
		LEAFLUT[3] = VLIT4(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0.38268343236508978177923268049199,-0.38268343236508978177923268049199);
		LEAFLUT[4] = VLIT4(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981);
		LEAFLUT[5] = VLIT4(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.92387953251128673848313610506011,-0.92387953251128673848313610506011);
  	
		LEAFLUT[6] = VLIT4(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1);
		LEAFLUT[7] = VLIT4(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0);
		LEAFLUT[8] = VLIT4(0.92387953251128673848313610506011,0.92387953251128673848313610506011,1,1);
		LEAFLUT[9] = VLIT4(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0,-0);
		LEAFLUT[10] = VLIT4(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.70710678118654757273731092936941,0.70710678118654757273731092936941);
		LEAFLUT[11] = VLIT4(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.70710678118654746171500846685376,-0.70710678118654746171500846685376);

		
		if(sign > 0) {
			V neg = VLIT4(-0.0f, -0.0f, -0.0f, -0.0f);
			LEAFLUT[1] = VXOR(LEAFLUT[1], neg);
			LEAFLUT[3] = VXOR(LEAFLUT[3], neg);
			LEAFLUT[5] = VXOR(LEAFLUT[5], neg);
			LEAFLUT[7] = VXOR(LEAFLUT[7], neg);
			LEAFLUT[9] = VXOR(LEAFLUT[9], neg);
			LEAFLUT[11] = VXOR(LEAFLUT[11], neg);
		}
		
		p->i0 = N/leafN/3+1;
		p->i1 = N/leafN/3;
		if((N/leafN) % 3 > 1) p->i1++;
		p->i0/=2;
		p->i1/=2;

	}else{
		p->transforms = malloc(2 * sizeof(transform_index_t));
		p->transforms[0] = 0;
		p->transforms[1] = 1;
  	if(N == 2) p->transform = &firstpass_2;
  	else if(N == 4 && sign == -1) p->transform = &firstpass_4_f;
  	else if(N == 4 && sign == 1) p->transform = &firstpass_4_b;
  	else if(N == 8) p->transform = &firstpass_8;
  	else if(N == 16) p->transform = &firstpass_16;
  	else if(N == 32) p->transform = &firstpass_32;

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
			#ifdef __ARM_NEON__
				if(N <= 32) lut_size += n/4 * 2 * sizeof(cdata_t);
				else lut_size += n/4 * sizeof(cdata_t);
			#else
				lut_size += n/4 * 2 * sizeof(cdata_t);
			#endif
				n *= 2;
			} else {
			#ifdef __ARM_NEON__
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
		
		
		for(i=0;i<n_luts;i++) {
			p->ws_is[i] = w - (cdata_t *)p->ws;	
			//fprintf(stderr, "LUT[%zu] = %d @ %08x - %zu\n", i, n, w, p->ws_is[i]);	
			
			if(!i || hardcoded) {
				cdata_t *w0 = FFTS_MALLOC(n/4 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/4;j++) {
					w0[j]	= W(n,j);
				}


				float *fw0 = (float *)w0;
				#ifdef __ARM_NEON__
				if(N <= 32) {
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
				}else{
					//w = FFTS_MALLOC(n/4 * sizeof(cdata_t), 32);
					float *fw = (float *)w;
					VS temp0, temp1, temp2;
					for(j=0;j<n/4;j+=4) {
						temp0 = VLD2(fw0 + j*2);
						STORESPR(fw + j*2, temp0);
					}
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
	//		  	for(j=0;j<n/2;j++) {
	//		  		printf("%f %f\n", creal(w[j]), cimag(w[j]));

	//		  	}

				FFTS_FREE(w0);
			}else{

				cdata_t *w0 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);
				cdata_t *w1 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);
				cdata_t *w2 = FFTS_MALLOC(n/8 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/8;j++) {
					w0[j]	= W(n,j*2);
					w1[j]	= W(n,j);
					w2[j]	= W(n,j + (n/8));

				}

				float *fw0 = (float *)w0;
				float *fw1 = (float *)w1;
				float *fw2 = (float *)w2;
				#ifdef __ARM_NEON__
				//w = FFTS_MALLOC(n/8 * 3 * sizeof(cdata_t), 32);
				float *fw = (float *)w;
				VS temp0, temp1, temp2;
				for(j=0;j<n/8;j+=4) {
					temp0 = VLD2(fw0 + j*2);
					STORESPR(fw + j*2*3,      temp0);
					//VST(fw + j*2*3,      temp0.val[0]);
					//VST(fw + j*2*3 + 4,  temp0.val[1]);
					temp1 = VLD2(fw1 + j*2);
					STORESPR(fw + j*2*3 + 8,  temp1);
					//VST(fw + j*2*3 + 8,  temp1.val[0]);
					//VST(fw + j*2*3 + 12, temp1.val[1]);
					temp2 = VLD2(fw2 + j*2);
					STORESPR(fw + j*2*3 + 16, temp2);
					//VST(fw + j*2*3 + 16, temp2.val[0]);
					//VST(fw + j*2*3 + 20, temp2.val[1]);
				}
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
//for(i=0;i<lut_size*2;i+=8) {
//	fprintf(stderr, "%08x %f %f %f %f - %f %f %f %f\n", 
//		tmp,
//		tmp[0], tmp[1], tmp[2], tmp[3], 
//		tmp[4], tmp[5], tmp[6], tmp[7]);
//	tmp += 8;
//}

	p->N = N;
	p->lastlut = w;
	p->n_luts = n_luts;
	if(N>=32)  ffts_generate_func_code(p, N, leafN);
#ifdef __x86_64__
	float *temp_consts = (float *)p->constants;
	if(sign > 0) {
		temp_consts[0] = -0.0f;
		temp_consts[1] = 0.0f;
		temp_consts[2] = -0.0f;
		temp_consts[3] = 0.0f;
	
		float temp;
  	temp = temp_consts[9];
  	temp_consts[9] = temp_consts[8];
  	temp_consts[8] = temp;
  	temp = temp_consts[11];
  	temp_consts[11] = temp_consts[10];
  	temp_consts[10] = temp;

		temp = temp_consts[18];
		temp_consts[18] = temp_consts[19];
		temp_consts[19] = temp;
	}
#endif
//	fprintf(stderr, "sizeof(size_t) == %lu\n", sizeof(size_t));

	return p;
}
/*
int main(int argc, char *argv[]) {
	int n = atoi(argv[1]);
	int count = atoi(argv[2]);

	ffts_plan_t *p = ffts_init(n);
	cdata_t *in = FFTS_MALLOC(n * sizeof(cdata_t), 32);
	cdata_t *out = FFTS_MALLOC(n * sizeof(cdata_t), 32);

	size_t i;
	for(i=0;i<n;i++) in[i] = i;
	
	p->leaftime = 0;

	if(count>1){
		for(i=0;i<count;i++) ffts_execute(p, (const float *)in, (float *)out, n);
	}
	p->leaftime = 0;

	uint64_t start = mach_absolute_time();
	for(i=0;i<count;i++) ffts_execute(p, (const float *)in, (float *)out, n);
	uint64_t elapsed = mach_absolute_time() - start;
	
	static double conversion = 0.0;

	if( conversion == 0.0 )
	{
		mach_timebase_info_data_t info;
		kern_return_t err = mach_timebase_info( &info );

		//Convert the timebase into seconds
		if( err == 0  )
			conversion = 1e-9 * (double) info.numer / (double) info.denom;
	}

	double tt = conversion * (double) elapsed;
	tt /= (double) count;	
	double lt = conversion * (double) (p->leaftime);
//	lt /= (double) count;	
	for(i=0;i<n;i++) printf("%f %f\n", creal(out[i]), cimag(out[i]));

	double ctgs = 5.0f * (double)n * (log(n)/log(2)) / (tt * 1000000000.0f);

	printf("Time: %f seconds, CTGs: %f Leaftime: %f \n", tt, ctgs, lt);

	return 0;
}*/
