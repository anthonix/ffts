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

#ifndef __CODEGEN_ARM_H__
#define __CODEGEN_ARM_H__



uint32_t BL(void *pos, void *target) {
	return 0xeb000000 | (((target - pos) / 4) & 0xffffff);
}

uint32_t B(uint8_t r) {
	return 0xe12fff10 | r;
}

uint32_t MOV(uint8_t dst, uint8_t src) {
		return 0xe1a00000 | (src & 0xf) | ((dst & 0xf) << 12);
}

void ADDI(uint32_t **p, uint8_t dst, uint8_t src, int32_t imm) {
	int32_t oimm = imm;
	if(imm < 0) {
		imm = -imm;
		uint32_t shamt = (__builtin_ctzl(imm)>23)?23:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;
		
	//	if(imm > 255) fprintf(stderr, "imm>255: %d\n", oimm);
		*(*p)++ = 0xe2400000 | ((src & 0xf) << 16) | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff);
	
		if(imm > 255) ADDI(p, dst, src, (oimm + ((imm & 0xff) << (32-shamt*2))));

	}else{
		uint32_t shamt = (__builtin_ctzl(imm)>23)?23:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;

//		if(imm > 255) fprintf(stderr, "imm>255: %d\n", oimm);

		*(*p)++ = 0xe2800000 | ((src & 0xf) << 16) | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff);
		
		if(imm > 255) ADDI(p, dst, src, (oimm - ((imm & 0xff) << (32-shamt*2))));
	}
}

uint32_t LDRI(uint8_t dst, uint8_t base, uint32_t offset) {
	return 0xe5900000 | ((dst & 0xf) << 12)
	                  | ((base & 0xf) << 16) | (offset & 0xfff) ;
}

void MOVI(uint32_t **p, uint8_t dst, uint32_t imm) {
	uint32_t oimm = imm;
	
		uint32_t shamt = (__builtin_ctzl(imm)>23)?23:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;
	*(*p)++ = 0xe3a00000 | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff) ;
		if(imm > 255) ADDI(p, dst, dst, (oimm - ((imm & 0xff) << (32-shamt*2))));
}

uint32_t PUSH_LR() { return 0xe92d4ff0; } //0xe92d4000; }
uint32_t POP_LR() { return 0xe8bd8ff0; } //0xe8bd8000; }




#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
