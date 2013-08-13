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


#ifndef __CODEGEN_SSE_H__
#define __CODEGEN_SSE_H__

void neon_x4(float *, size_t, float *);
void neon_x8(float *, size_t, float *);
void neon_x8_t(float *, size_t, float *);
void leaf_ee_init();
void leaf_ee();
void leaf_oo();
void leaf_eo();
void leaf_oe();
void leaf_end();
void x_init();
void x4();
void x8_soft();
void x8_hard();

void sse_constants();
void sse_constants_inv();

typedef uint8_t insns_t;

extern const uint32_t sse_leaf_ee_offsets[8];
extern const uint32_t sse_leaf_oo_offsets[8];
extern const uint32_t sse_leaf_eo_offsets[8];
extern const uint32_t sse_leaf_oe_offsets[8];

#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESI 6
#define EDI 7
#define EBP 5

#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSI 6
#define RDI 7
#define RBP 5
#define R8 8 
#define R9 9 
#define R10 10 
#define R11 11 
#define R12 12 
#define R13 13 
#define R14 14 
#define R15 15 

void IMM8(uint8_t **p, int32_t imm) ;

void IMM16(uint8_t **p, int32_t imm) ;
void IMM32(uint8_t **p, int32_t imm);
void IMM32_NI(uint8_t *p, int32_t imm);

int32_t READ_IMM32(uint8_t *p);

void MOVI(uint8_t **p, uint8_t dst, uint32_t imm);

void ADDRMODE(uint8_t **p, uint8_t reg, uint8_t rm, int32_t disp);

void LEA(uint8_t **p, uint8_t dst, uint8_t base, int32_t disp);

void RET(uint8_t **p);

void ADDI(uint8_t **p, uint8_t dst, int32_t imm);

void CALL(uint8_t **p, uint8_t *func);

void PUSH(uint8_t **p, uint8_t reg);
void POP(uint8_t **p, uint8_t reg);

#endif
