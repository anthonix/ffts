#ifndef __CODEGEN_SSE_H__
#define __CODEGEN_SSE_H__

static const __attribute__ ((aligned(16))) float ee_w_data[4] = {0.70710678118654757273731092936941,0.70710678118654746171500846685376,
    -0.70710678118654757273731092936941,-0.70710678118654746171500846685376};
static const __attribute__ ((aligned(16))) data_t oe_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
static const __attribute__ ((aligned(16))) data_t eo_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
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

void IMM8(uint8_t **p, uint32_t imm) {
		*(*p)++ = (imm & 0xff);
}

void IMM32(uint8_t **p, uint32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(*p)++ = (imm & (0xff << (i*8))) >> (i*8);
	} 
}
void IMM32_NI(uint8_t *p, uint32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(p+i) = (imm & (0xff << (i*8))) >> (i*8);
	} 
}

uint32_t READ_IMM32(uint8_t *p) {
	uint32_t rval = 0;
	int i;
	for(i=0;i<4;i++) {
		rval |= *(p+i) << (i*8);
	} 
	return rval;
}

void MOVI(uint8_t **p, uint8_t dst, uint32_t imm) {
	if(dst < 8) {
		*(*p)++ = 0xb8 + dst; 
	}else{
		*(*p)++ = 0x49;
		*(*p)++ = 0xc7;
		*(*p)++ = 0xc0 | (dst - 8);
	}
	IMM32(p, imm);
}

void ADDRMODE(uint8_t **p, uint8_t reg, uint8_t rm, int32_t disp) {
	if(disp == 0) {
		*(*p)++ = (rm & 7) | ((reg & 7) << 3);
	}else if(disp <= 127 || disp >= -128) {
		*(*p)++ = 0x40 | (rm & 7) | ((reg & 7) << 3);
		IMM8(p, disp);
	}else{
		*(*p)++ = 0x80 | (rm & 7) | ((reg & 7) << 3);
		IMM32(p, disp);
	}
}

void LEA(uint8_t **p, uint8_t dst, uint8_t base, int32_t disp) {

	*(*p)++ = 0x48 | ((base & 0x8) >> 3) | ((dst & 0x8) >> 1);
	*(*p)++ = 0x8d;
	ADDRMODE(p, dst, base, disp);		
}

void RET(uint8_t **p) {
	*(*p)++ = 0xc3;
}

void ADDI(uint8_t **p, uint8_t dst, int32_t imm) {
	if(dst >= 8) *(*p)++ = 0x49;
	else         *(*p)++ = 0x48;
	
	if(imm > 127 || imm <= -128) *(*p)++ = 0x81;
	else          *(*p)++ = 0x83;
	
	*(*p)++ = 0xc0 | (dst & 0x7);

	if(imm > 127 || imm <= -128) IMM32(p, imm);
	else          IMM8(p, imm);
}

void CALL(uint8_t **p, insns_t *func) {
	*(*p)++ = 0xe8;
	IMM32(p, ((void *)func) - (void *)(*p) - 4); 
}

void PUSH(uint8_t **p, uint8_t reg) {
	if(reg >= 8) *(*p)++ = 0x41;
	*(*p)++ = 0x50 | (reg & 7);
}
void POP(uint8_t **p, uint8_t reg) {
	if(reg >= 8) *(*p)++ = 0x41;
	*(*p)++ = 0x58 | (reg & 7);
}

#endif
