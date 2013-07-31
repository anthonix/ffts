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

#include "patterns.h"

void permute_addr(int N, int offset, int stride, int *d) {
    int i, a[4] = {0,2,1,3};
    for(i=0;i<4;i++) {
        d[i] = offset + (a[i] << stride);
        if(d[i] < 0) d[i] += N;
    }
}

void ffts_hardcodedleaf_is_rec(ptrdiff_t **is, int bigN, int N, int poffset, int offset, int stride, int even, int VL) {
 
	if(N > 4) {  
    ffts_hardcodedleaf_is_rec(is, bigN, N/2, poffset, offset, stride + 1, even, VL);  
		if(N/4 >= 4) ffts_hardcodedleaf_is_rec(is, bigN, N/4, poffset+(1<<stride),offset+(N/2), stride + 2, 0, VL);
		if(N/4 >= 4) ffts_hardcodedleaf_is_rec(is, bigN, N/4, poffset-(1<<stride),offset+(3*N/4), stride + 2, 0, VL);
		else {
			int temp = poffset+(1<<stride);
			if(temp < 0) temp += bigN;
			temp *= 2;

			if(!(temp % (VL*2))) { 
				(*is)[0] = poffset+(1<<stride);
				(*is)[1] = poffset+(1<<stride)+(1<<(stride+2));
				(*is)[2] = poffset-(1<<stride);
				(*is)[3] = poffset-(1<<stride)+(1<<(stride+2));
				int i;
				for(i=0;i<4;i++) if((*is)[i] < 0) (*is)[i] += bigN;
				for(i=0;i<4;i++) (*is)[i] *= 2; 
				*is += 4;
			}
		}
  }else if(N == 4) {
		int perm[4];
		permute_addr(bigN, poffset, stride, perm);
		if(!((perm[0]*2) % (VL*2))) { 
			int i;
			for(i=0;i<4;i++) {
				(*is)[i] = perm[i] * 2;
				}
			*is += 4;
		}
	}
}

void ffts_init_is(ffts_plan_t *p, int N, int leafN, int VL) {
	int i, i0 = N/leafN/3+1, i1=N/leafN/3, i2 = N/leafN/3;
	int stride = log(N/leafN)/log(2);
	
	p->is = malloc(N/VL * sizeof(ptrdiff_t));
	
	ptrdiff_t *is = p->is;

	if((N/leafN) % 3 > 1) i1++;

	for(i=0;i<i0;i++) ffts_hardcodedleaf_is_rec(&is, N, leafN, i, 0, stride, 1, VL);
	for(i=i0;i<i0+i1;i++) {
		ffts_hardcodedleaf_is_rec(&is, N, leafN/2, i, 0, stride+1, 1, VL);
		ffts_hardcodedleaf_is_rec(&is, N, leafN/2, i-(1<<stride), 0, stride+1, 1, VL);
	}
	for(i=0-i2;i<0;i++) ffts_hardcodedleaf_is_rec(&is, N, leafN, i, 0, stride, 1, VL);


//for(i=0;i<N/VL;i++) {
//	printf("%td ", p->is[i]);
//	if(i % 16 == 15) printf("\n");
//}

	p->i0 = i0; p->i1 = i1;
}
/**
 *
 *
 */
void ffts_elaborate_offsets(ptrdiff_t *offsets, int leafN, int N, int ioffset, int ooffset, int stride, int even) {
  if((even && N == leafN) || (!even && N <= leafN)) {
		offsets[2*(ooffset/leafN)]   = ioffset*2;
		offsets[2*(ooffset/leafN)+1] = ooffset;
	}else if(N > 4) {
		ffts_elaborate_offsets(offsets, leafN, N/2, ioffset, ooffset, stride+1, even);
		ffts_elaborate_offsets(offsets, leafN, N/4, ioffset+(1<<stride), ooffset+N/2, stride+2, 0);
  	if(N/4 >= leafN) 
		ffts_elaborate_offsets(offsets, leafN, N/4, ioffset-(1<<stride), ooffset+3*N/4, stride+2, 0);
	}

}

int compare_offsets(const void *a, const void *b) {
	return ((ptrdiff_t *)a)[0] - ((ptrdiff_t *)b)[0];
}

uint32_t reverse_bits(uint32_t a, int n) {
	uint32_t x = 0;

	int i;
	for(i=0;i<n;i++) {
		if(a & (1 << i)) x |= 1 << (n-i-1);
	}
	return x;
}


void ffts_init_offsets(ffts_plan_t *p, int N, int leafN) {

	ptrdiff_t *offsets = malloc(2 * N/leafN * sizeof(ptrdiff_t));

	ffts_elaborate_offsets(offsets, leafN, N, 0, 0, 1, 1);

	size_t i;
	for(i=0;i<2*N/leafN;i+=2) {
		if(offsets[i] < 0) offsets[i] = N + offsets[i];
	}
	
	qsort(offsets, N/leafN, 2 * sizeof(ptrdiff_t), compare_offsets); 
	//elaborate_is(p, N, 0, 0, 1);
	p->offsets = malloc(N/leafN * sizeof(ptrdiff_t));
	for(i=0;i<N/leafN;i++) {
		p->offsets[i] = offsets[i*2+1]*2;
	}
//for(i=0;i<N/leafN;i++) {
//	printf("%4d %4d\n", p->offsets[i], reverse_bits(p->offsets[i], __builtin_ctzl(2*N)));
//}
	free(offsets);
}

/*
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

void elaborate_tree(transform_index_t **p, int N, int leafN, int offset) {
	
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

void ffts_init_tree(ffts_plan_t *p, int N, int leafN) {

	int count = tree_count(N, leafN, 0) + 1;
	transform_index_t *ps = p->transforms = malloc(count * 2 * sizeof(transform_index_t));

//printf("count = %d\n", count);

	elaborate_tree(&ps, N, leafN, 0);
	#ifdef __ARM_NEON__
	ps -= 2;
	#endif
	ps[0] = 0;
	ps[1] = 0;
//int i;
//for(i=0;i<count;i++) {
//	fprintf(stderr, "%lu %lu - %d\n", p->transforms[i*2], p->transforms[i*2+1],
//		__builtin_ctzl(p->transforms[i*2]) - 5);
//}

}
*/
