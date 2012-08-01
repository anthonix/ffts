#include "cp_sse.h"
#include "macros.h"
#include "patterns.h"

__INLINE void 
firstpass_type_1(const float * restrict in, float * restrict out, size_t N, ffts_plan_t * restrict p) {
	size_t i, i0 = p->i0, i1 = p->i1;
	size_t *offsets = (size_t *)p->offsets;
	size_t *is = (size_t *)p->is;
  for(i=i0;i>0;--i) LEAF_EE(&is, in, &offsets, out);
  for(i=i1;i>0;--i) LEAF_OO(&is, in, &offsets, out);
  LEAF_OE(&is, in, &offsets, out);
  for(i=i1;i>0;--i) LEAF_EE(&is, in, &offsets, out);
}

__INLINE void 
firstpass_type_2(const float * restrict in, float * restrict out, size_t N, ffts_plan_t * restrict p) {
	size_t i, i0 = p->i0, i1 = p->i1;
	size_t *offsets = (size_t *)p->offsets;
	size_t *is = (size_t *)p->is;
  for(i=i0;i>0;--i) LEAF_EE(&is, in, &offsets, out);
  LEAF_EO(&is, in, &offsets, out);
  for(i=i1;i>0;--i) LEAF_OO(&is, in, &offsets, out);
  for(i=i1;i>0;--i) LEAF_EE(&is, in, &offsets, out);
}

__INLINE void 
firstpass_64(const float * restrict in, float * restrict out, size_t N, ffts_plan_t * restrict p) {
	size_t *offsets = (size_t *)p->offsets;
	size_t *is = (size_t *)p->is;
  LEAF_EE(&is, in, &offsets, out);
  LEAF_OE(&is, in, &offsets, out);
}

void 
firstpass_32(const data_t * restrict in, data_t * restrict out, size_t N, ffts_plan_t * restrict p) {
  __m128 r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15,r16_17,r18_19,r20_21,r22_23,r24_25,r26_27,r28_29,r30_31;
  float *LUT8 = p->ws[0];
  float *LUT16 = p->ws[1];
  float *LUT32 = p->ws[2];

  L_4_4(in+0,in+32,in+16,in+48,&r0_1,&r2_3,&r16_17,&r18_19);
  L_2_2(in+8,in+40,in+56,in+24,&r4_5,&r6_7,&r20_21,&r22_23);
  K_N(_mm_load_ps(LUT8),_mm_load_ps(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  L_4_2(in+4,in+36,in+20,in+52,&r8_9,&r10_11,&r28_29,&r30_31);
  L_4_4(in+60,in+28,in+12,in+44,&r12_13,&r14_15,&r24_25,&r26_27);
  K_N(_mm_load_ps(LUT16),_mm_load_ps(LUT16+4),&r0_1,&r4_5,&r8_9,&r12_13);
  K_N(_mm_load_ps(LUT16+8),_mm_load_ps(LUT16+12),&r2_3,&r6_7,&r10_11,&r14_15);
  K_N(_mm_load_ps(LUT8),_mm_load_ps(LUT8+4),&r16_17,&r18_19,&r20_21,&r22_23);
  K_N(_mm_load_ps(LUT8),_mm_load_ps(LUT8+4),&r24_25,&r26_27,&r28_29,&r30_31);
  K_N(_mm_load_ps(LUT32),_mm_load_ps(LUT32+4),&r0_1,&r8_9,&r16_17,&r24_25);
  S_4(r0_1,r8_9,r16_17,r24_25,out+0,out+16,out+32,out+48);
  K_N(_mm_load_ps(LUT32+8),_mm_load_ps(LUT32+12),&r2_3,&r10_11,&r18_19,&r26_27);
  S_4(r2_3,r10_11,r18_19,r26_27,out+4,out+20,out+36,out+52);
  K_N(_mm_load_ps(LUT32+16),_mm_load_ps(LUT32+20),&r4_5,&r12_13,&r20_21,&r28_29);
  S_4(r4_5,r12_13,r20_21,r28_29,out+8,out+24,out+40,out+56);
  K_N(_mm_load_ps(LUT32+24),_mm_load_ps(LUT32+28),&r6_7,&r14_15,&r22_23,&r30_31);
  S_4(r6_7,r14_15,r22_23,r30_31,out+12,out+28,out+44,out+60);

}

void 
firstpass_16(const data_t * restrict in, data_t * restrict out, size_t N, ffts_plan_t * restrict p) {
  __m128 r0_1,r2_3,r4_5,r6_7,r8_9,r10_11,r12_13,r14_15;
  float *LUT8 = p->ws[0];
  float *LUT16 = p->ws[1];

  L_4_4(in+0,in+16,in+8,in+24,&r0_1,&r2_3,&r8_9,&r10_11);
  L_2_4(in+4,in+20,in+28,in+12,&r4_5,&r6_7,&r14_15,&r12_13);
  K_N(_mm_load_ps(LUT8),_mm_load_ps(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(_mm_load_ps(LUT16),_mm_load_ps(LUT16+4),&r0_1,&r4_5,&r8_9,&r12_13);
  S_4(r0_1,r4_5,r8_9,r12_13,out+0,out+8,out+16,out+24);
  K_N(_mm_load_ps(LUT16+8),_mm_load_ps(LUT16+12),&r2_3,&r6_7,&r10_11,&r14_15);
  S_4(r2_3,r6_7,r10_11,r14_15,out+4,out+12,out+20,out+28);
}

void 
firstpass_8(const data_t * restrict in, data_t * restrict out, size_t N, ffts_plan_t * restrict p) {
  __m128 r0_1,r2_3,r4_5,r6_7;
  float *LUT8 = p->ws[0];
	L_4_2(in+0,in+8,in+4,in+12,&r0_1,&r2_3,&r4_5,&r6_7);
  K_N(_mm_load_ps(LUT8),_mm_load_ps(LUT8+4),&r0_1,&r2_3,&r4_5,&r6_7);
  S_4(r0_1,r2_3,r4_5,r6_7,out+0,out+4,out+8,out+12);
}
void 
firstpass_4(const data_t * restrict in, data_t * restrict out, size_t N, ffts_plan_t * restrict p) {
  __m128 r0,r1,r2,r3;
  L_4(in+0,in+4,in+2,in+6,&r0,&r1,&r2,&r3);
  S_4(r0,r1,r2,r3,out+0,out+2,out+4,out+6);
}
void 
firstpass_2(const data_t * restrict in, data_t * restrict out, size_t N, ffts_plan_t * restrict p) {
  __m128 r0,r1;
  L_S2(in+0,in+2,&r0,&r1);
  S_2(r0,r1,out+0,out+2);
}

void X_8(data_t * restrict data0, size_t N, const data_t * restrict LUT) {
	data_t *data2 = data0 + 2*N/4;
	data_t *data4 = data0 + 4*N/4;
	data_t *data6 = data0 + 6*N/4;
	data_t *data1 = data0 + 1*N/4;
	data_t *data3 = data0 + 3*N/4;
	data_t *data5 = data0 + 5*N/4;
	data_t *data7 = data0 + 7*N/4;
	size_t k, n4 = N/4;

	for(k=N/8/2;k>0;--k) {
      __m128 r0, r1, r2, r3, r4, r5, r6, r7;
			r0 = _mm_load_ps(data0);
			r1 = _mm_load_ps(data1);
			r2 = _mm_load_ps(data2);
			r3 = _mm_load_ps(data3);
			K_N(_mm_load_ps(LUT), _mm_load_ps(LUT+4), &r0, &r1, &r2, &r3);
			r4 = _mm_load_ps(data4);
			r6 = _mm_load_ps(data6);
			K_N(_mm_load_ps(LUT+8), _mm_load_ps(LUT+12), &r0, &r2, &r4, &r6);
			r5 = _mm_load_ps(data5);
			r7 = _mm_load_ps(data7);
			K_N(_mm_load_ps(LUT+16), _mm_load_ps(LUT+20), &r1, &r3, &r5, &r7);
			LUT += 24;
			_mm_store_ps(data0, r0); data0 += 4;
			_mm_store_ps(data1, r1); data1 += 4;
			_mm_store_ps(data2, r2); data2 += 4;
			_mm_store_ps(data3, r3); data3 += 4;
			_mm_store_ps(data4, r4); data4 += 4;
			_mm_store_ps(data5, r5); data5 += 4;
			_mm_store_ps(data6, r6); data6 += 4;
			_mm_store_ps(data7, r7); data7 += 4;
	}
}
void ffts_execute(ffts_plan_t *p, const void * restrict in, void * restrict out, size_t N) {
	transform_index_t *ps = p->transforms;

	p->firstpass((const float *)in, (float *)out, N, p);
	while(ps[0]) {
		
		if(ps[0] == 32) { 
  		float *LUT = (float *)p->ws[0];
  		float *data = (float *)(out) + ps[1];
  		size_t n = 32;
  		size_t i;
  		for(i=0;i<n/4/2;i++) {
  			__m128 uk = _mm_load_ps(data);
  			__m128 uk2 = _mm_load_ps(data + 2*n/4);
  			__m128 zk_p = _mm_load_ps(data + 4*n/4);
  			__m128 zk_n = _mm_load_ps(data + 6*n/4);

  			K_N(_mm_load_ps(LUT), _mm_load_ps(LUT+4), &uk, &uk2, &zk_p, &zk_n);

  			_mm_store_ps(data, uk); 
  			_mm_store_ps(data + 2*n/4, uk2); 
  			_mm_store_ps(data + 4*n/4, zk_p);
  			_mm_store_ps(data + 6*n/4, zk_n);

  			LUT += 8;
  			data += 4;
  		}


		}else{
  		int index = __builtin_ctzl(ps[0])-5;
  		float *LUT = (float *)p->ws[__builtin_ctzl(ps[0])-5];
  		X_8(((float *)out) + ps[1], ps[0], LUT);
		}

		ps += 2;
	}
	
	
}


ffts_plan_t *ffts_init(size_t N, int sign) {
	ffts_plan_t *p = malloc(sizeof(ffts_plan_t));
	size_t leafN = 16;	
	size_t i;	
		
	if(sign < 0) MULI_SIGN = _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f);
	else         MULI_SIGN = _mm_set_ps(0.0f, -0.0f, 0.0f, -0.0f);
	
	if(N > 32) {
		init_offsets(p, N, leafN);
		init_is(p, N, leafN, 2);
		init_tree(p, N, leafN);
		
		if(N == 64) p->firstpass = &firstpass_64;
		else if(__builtin_ctzl(N) & 1) p->firstpass = &firstpass_type_2;	
		else p->firstpass = &firstpass_type_1;	

		LEAFLUT[0] = _mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941,0.70710678118654757273731092936941);
		LEAFLUT[1] = _mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0.70710678118654746171500846685376,-0.70710678118654746171500846685376);
		LEAFLUT[2] = _mm_set_ps(0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011,0.92387953251128673848313610506011);
		LEAFLUT[3] = _mm_set_ps(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0.38268343236508978177923268049199,-0.38268343236508978177923268049199);
		LEAFLUT[4] = _mm_set_ps(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.38268343236508983729038391174981);
		LEAFLUT[5] = _mm_set_ps(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.92387953251128673848313610506011,-0.92387953251128673848313610506011);
  	
		LEAFLUT[6] = _mm_set_ps(0.70710678118654757273731092936941,0.70710678118654757273731092936941,1,1);
		LEAFLUT[7] = _mm_set_ps(0.70710678118654746171500846685376,-0.70710678118654746171500846685376,0,-0);
		LEAFLUT[8] = _mm_set_ps(0.92387953251128673848313610506011,0.92387953251128673848313610506011,1,1);
		LEAFLUT[9] = _mm_set_ps(0.38268343236508978177923268049199,-0.38268343236508978177923268049199,0,-0);
		LEAFLUT[10] = _mm_set_ps(0.38268343236508983729038391174981,0.38268343236508983729038391174981,0.70710678118654757273731092936941,0.70710678118654757273731092936941);
		LEAFLUT[11] = _mm_set_ps(0.92387953251128673848313610506011,-0.92387953251128673848313610506011,0.70710678118654746171500846685376,-0.70710678118654746171500846685376);

		
		if(sign > 0) {
			LEAFLUT[1] = _mm_xor_ps(LEAFLUT[1], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
			LEAFLUT[3] = _mm_xor_ps(LEAFLUT[3], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
			LEAFLUT[5] = _mm_xor_ps(LEAFLUT[5], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
			LEAFLUT[7] = _mm_xor_ps(LEAFLUT[7], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
			LEAFLUT[9] = _mm_xor_ps(LEAFLUT[9], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
			LEAFLUT[11] = _mm_xor_ps(LEAFLUT[11], _mm_set_ps(-0.0f,-0.0f,-0.0f,-0.0f));
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
		if(N == 2) p->firstpass = &firstpass_2;
		else if(N == 4) p->firstpass = &firstpass_4;
		else if(N == 8) p->firstpass = &firstpass_8;
		else if(N == 16) p->firstpass = &firstpass_16;
		else if(N == 32) p->firstpass = &firstpass_32;

	}

		int hardcoded = 0;

		/*      LUTS           */
		size_t n_luts = __builtin_ctzl(N/leafN);
		if(N <= 32) { n_luts = __builtin_ctzl(N/4); hardcoded = 1; }


		//printf("n_luts = %zu\n", n_luts);
		p->ws = malloc(n_luts * sizeof(data_t *));
		cdata_t *w;

		int n = leafN*2;
		if(hardcoded) n = 8;

		for(i=0;i<n_luts;i++) {
				
			//printf("LUT[%zu] = %d\n", i, n);	
			
			if(!i || hardcoded) {

				w = _mm_malloc(n/4 * 2 * sizeof(cdata_t), 32);

				cdata_t *w0 = _mm_malloc(n/4 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/4;j++) {
					w0[j]	= W(n,j);
				}

				__m128 temp0, temp1, temp2;

				float *fw = (float *)w;
				float *fw0 = (float *)w0;
				for(j=0;j<n/4;j+=2) {
					temp0 = _mm_load_ps(fw0 + j*2);
					__m128 re, im;
					re = _mm_shuffle_ps(temp0, temp0, _MM_SHUFFLE(2, 2, 0, 0));
					im = _mm_shuffle_ps(temp0, temp0, _MM_SHUFFLE(3, 3, 1, 1));
					im = _mm_xor_ps(im, MULI_SIGN);
					_mm_store_ps(fw + j*4  , re);
					_mm_store_ps(fw + j*4+4, im);
				}

	//		  	for(j=0;j<n/2;j++) {
	//		  		printf("%f %f\n", creal(w[j]), cimag(w[j]));

	//		  	}

				_mm_free(w0);
			}else{
				w = _mm_malloc(n/8 * 3 * 2 * sizeof(cdata_t), 32);

				cdata_t *w0 = _mm_malloc(n/8 * sizeof(cdata_t), 32);
				cdata_t *w1 = _mm_malloc(n/8 * sizeof(cdata_t), 32);
				cdata_t *w2 = _mm_malloc(n/8 * sizeof(cdata_t), 32);

				size_t j;
				for(j=0;j<n/8;j++) {
					w0[j]	= W(n,j*2);
					w1[j]	= W(n,j);
					w2[j]	= W(n,j + (n/8));

				}

				__m128 temp0, temp1, temp2, re, im;

				float *fw = (float *)w;
				float *fw0 = (float *)w0;
				float *fw1 = (float *)w1;
				float *fw2 = (float *)w2;
				for(j=0;j<n/8;j+=2) {
					temp0 = _mm_load_ps(fw0 + j*2);
					re = _mm_shuffle_ps(temp0, temp0, _MM_SHUFFLE(2, 2, 0, 0));
					im = _mm_shuffle_ps(temp0, temp0, _MM_SHUFFLE(3, 3, 1, 1));
					im = _mm_xor_ps(im, MULI_SIGN);
					_mm_store_ps(fw + j*2*6  , re);
					_mm_store_ps(fw + j*2*6+4, im);

					temp1 = _mm_load_ps(fw1 + j*2);
					re = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(2, 2, 0, 0));
					im = _mm_shuffle_ps(temp1, temp1, _MM_SHUFFLE(3, 3, 1, 1));
					im = _mm_xor_ps(im, MULI_SIGN);
					_mm_store_ps(fw + j*2*6+8 , re);
					_mm_store_ps(fw + j*2*6+12, im);

					temp2 = _mm_load_ps(fw2 + j*2);
					re = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(2, 2, 0, 0));
					im = _mm_shuffle_ps(temp2, temp2, _MM_SHUFFLE(3, 3, 1, 1));
					im = _mm_xor_ps(im, MULI_SIGN);
					_mm_store_ps(fw + j*2*6+16, re);
					_mm_store_ps(fw + j*2*6+20, im);
				}

				_mm_free(w0);
				_mm_free(w1);
				_mm_free(w2);
			}
			p->ws[i] = w;

			n *= 2;
		}


	
	return p;
}
/*
int main(int argc, char *argv[]) {
	int n = atoi(argv[1]);
	int count = atoi(argv[2]);

	ffts_plan_t *p = ffts_init(n);
	cdata_t *in = _mm_malloc(n * sizeof(cdata_t), 32);
	cdata_t *out = _mm_malloc(n * sizeof(cdata_t), 32);

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
