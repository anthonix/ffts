#ifndef __CODEGEN_NEON_H__
#define __CODEGEN_NEON_H__



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
		uint32_t shamt = (__builtin_ctzl(imm)>15)?15:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;
		
	//	if(imm > 255) fprintf(stderr, "imm>255: %d\n", oimm);
		*(*p)++ = 0xe2400000 | ((src & 0xf) << 16) | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff);
	
		if(imm > 255) ADDI(p, dst, src, (oimm + ((imm & 0xff) << (32-shamt*2))));

	}else{
		uint32_t shamt = (__builtin_ctzl(imm)>15)?15:__builtin_ctzl(imm);
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

uint32_t MOVI(uint32_t **p, uint8_t dst, uint32_t imm) {
	uint32_t oimm = imm;
	
		uint32_t shamt = (__builtin_ctzl(imm)>15)?15:__builtin_ctzl(imm);
		if(shamt & 1) shamt -= 1;
		imm >>= shamt;
		shamt = (32 - shamt)/2;
	*(*p)++ = 0xe3a00000 | ((dst & 0xf) << 12) | ((shamt & 0xf) << 8) | (imm & 0xff) ;
		if(imm > 255) ADDI(p, dst, dst, (oimm - ((imm & 0xff) << (32-shamt*2))));
}

uint32_t PUSH_LR() { return 0xe92d4ff0; } //0xe92d4000; }
uint32_t POP_LR() { return 0xe8bd8ff0; } //0xe8bd8000; }




#endif
