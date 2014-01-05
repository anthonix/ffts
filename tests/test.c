/*
 
 This file is part of SFFT.
  
 Copyright (c) 2012, Anthony M. Blake
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

#include <stdio.h>
#include <math.h>

#ifdef __ARM_NEON__
#endif
#ifdef HAVE_SSE
	#include <xmmintrin.h>
#endif 

#include "../include/ffts.h"


#define PI 3.1415926535897932384626433832795028841971693993751058209

float impulse_error(int N, int sign, float *data) {
#ifdef __ANDROID__
	double delta_sum = 0.0f;
	double sum = 0.0f;
#else
	long double delta_sum = 0.0f;
	long double sum = 0.0f;
#endif 

	int i;
	for(i=0;i<N;i++) {
#ifdef __ANDROID__
		double re, im;
		if(sign < 0) {
			re = cos(2 * PI * (double)i / (double)N); 
			im = -sin(2 * PI * (double)i / (double)N); 
		}else{
			re = cos(2 * PI * (double)i / (double)N); 
			im = sin(2 * PI * (double)i / (double)N); 
		}
#else
		long double re, im;
		if(sign < 0) {
			re = cosl(2 * PI * (long double)i / (long double)N); 
			im = -sinl(2 * PI * (long double)i / (long double)N); 
		}else{
			re = cosl(2 * PI * (long double)i / (long double)N); 
			im = sinl(2 * PI * (long double)i / (long double)N); 
		}
#endif
		sum += re * re + im * im;

		re = re - data[2*i];
		im = im - data[2*i+1];
		
		delta_sum += re * re + im * im;

	}
#ifdef __ANDROID__
	return sqrt(delta_sum) / sqrt(sum);
#else
	return sqrtl(delta_sum) / sqrtl(sum);
#endif
}

int 
test_transform(int n, int sign) {

#ifdef HAVE_SSE 
	float __attribute__ ((aligned(32))) *input = _mm_malloc(2 * n * sizeof(float), 32);
  float __attribute__ ((aligned(32))) *output = _mm_malloc(2 * n * sizeof(float), 32);
#else
	float __attribute__ ((aligned(32))) *input = valloc(2 * n * sizeof(float));
  float __attribute__ ((aligned(32))) *output = valloc(2 * n * sizeof(float));
#endif
	int i;	
	for(i=0;i<n;i++) {
		input[2*i]   = 0.0f;
		input[2*i+1] = 0.0f;
	}

	input[2] = 1.0f;

	ffts_plan_t *p = ffts_init_1d(i, sign);
	if(p) {
		ffts_execute(p, input, output);
		printf(" %3d  | %9d | %10E\n", sign, n, impulse_error(n, sign, output));
  	ffts_free(p);
	}else{
		printf("Plan unsupported\n");
		return 0;
	}

	return 1;
}

int
main(int argc, char *argv[]) {
	
	if(argc == 3) {
		// test specific transform with test pattern and display output
		int n = atoi(argv[1]);
		int sign = atoi(argv[2]);

#ifdef HAVE_SSE
		float __attribute__ ((aligned(32))) *input = _mm_malloc(2 * n * sizeof(float), 32);
		float __attribute__ ((aligned(32))) *output = _mm_malloc(2 * n * sizeof(float), 32);
#else
		float __attribute__ ((aligned(32))) *input = valloc(2 * n * sizeof(float));
		float __attribute__ ((aligned(32))) *output = valloc(2 * n * sizeof(float));
#endif
		int i;	
		for(i=0;i<n;i++) {
			input[2*i]   = i;
			input[2*i+1] = 0.0f;
		}

	//	input[2] = 1.0f;

		ffts_plan_t *p = ffts_init_1d(i, sign);
		if(p) {
			ffts_execute(p, input, output);
			for(i=0;i<n;i++) printf("%d %d %f %f\n", i, sign, output[2*i], output[2*i+1]);
			ffts_free(p);
		}else{
			printf("Plan unsupported\n");
			return 0;
		}

#ifdef HAVE_NEON 
		_mm_free(input);
		_mm_free(output);
#else
		free(input);
		free(output);
#endif

	}else{
		// test various sizes and display error
		printf(" Sign |      Size |     L2 Error\n");
		printf("------+-----------+-------------\n");
		int n;
		for(n=1;n<=18;n++) {
			test_transform(pow(2,n), -1);
		}
		for(n=1;n<=18;n++) {
			test_transform(pow(2,n), 1);
		}
	}
  return 0;
}
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
