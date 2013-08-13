#include "arch_codegen.h"
#include "ffts.h"
void IMM8(uint8_t **p, int32_t imm) {
		*(*p)++ = (imm & 0xff);
}

void IMM16(uint8_t **p, int32_t imm) {
	int i;
	for(i=0;i<2;i++) {
		*(*p)++ = (imm & (0xff << (i*8))) >> (i*8);
	} 
}
void IMM32(uint8_t **p, int32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(*p)++ = (imm & (0xff << (i*8))) >> (i*8);
	} 
}
void IMM32_NI(uint8_t *p, int32_t imm) {
	int i;
	for(i=0;i<4;i++) {
		*(p+i) = (imm & (0xff << (i*8))) >> (i*8);
	} 
}

int32_t READ_IMM32(uint8_t *p) {
	int32_t rval = 0;
	int i;
	for(i=0;i<4;i++) {
		rval |= *(p+i) << (i*8);
	} 
	return rval;
}

void MOVI(uint8_t **p, uint8_t dst, uint32_t imm) {
//  if(imm < 65536) *(*p)++ = 0x66; 
  if(dst >= 8) *(*p)++ = 0x41;

  //if(imm < 65536 && imm >= 256) *(*p)++ = 0x66; 

  //if(imm >= 256) 
	*(*p)++ = 0xb8 | (dst & 0x7);
//  else           *(*p)++ = 0xb0 | (dst & 0x7);

 // if(imm < 256) IMM8(p, imm);
//  else 
//if(imm < 65536) IMM16(p, imm);
//else 
	IMM32(p, imm);

//if(dst < 8) {
//	*(*p)++ = 0xb8 + dst; 
//}else{
//	*(*p)++ = 0x49;
//	*(*p)++ = 0xc7;
//	*(*p)++ = 0xc0 | (dst - 8);
//}
//IMM32(p, imm);
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

void CALL(uint8_t **p, uint8_t *func) {
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


#define P(x) (*(*p)++ = x)
void insert_nops(uint8_t **p, uint32_t count) {
	switch(count) {
		case 0: break;
		case 2: P(0x66); 
		case 1: P(0x90); break;
		case 3: P(0x0F); P(0x1F); P(0x00); break;	
		case 4: P(0x0F); P(0x1F); P(0x40); P(0x00); break;	
		case 5: P(0x0F); P(0x1F); P(0x44); P(0x00); P(0x00); break;	
		case 6: P(0x66); P(0x0F); P(0x1F); P(0x44); P(0x00); P(0x00); break;	
		case 7: P(0x0F); P(0x1F); P(0x80); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		case 8: P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		case 9: P(0x66); P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); break;	
		default:
			P(0x66); P(0x0F); P(0x1F); P(0x84); P(0x00); P(0x00); P(0x00); P(0x00); P(0x00); 
			insert_nops(p, count-9);
			break;	
	}
}
void align_mem16(uint8_t **p, uint32_t offset) {
	int r = (16 - (offset & 0xf)) - ((uint32_t)(*p) & 0xf);
	r = (16 + r) & 0xf;
	insert_nops(p, r);	
}

void define_transform_size(ffts_plan_t *p, size_t N) {
	if(N < 2048) p->transform_size = 16384;
	else p->transform_size = 16384 + 2*N/8 * __builtin_ctzl(N);
}
insns_t* generate_size8_base_case(insns_t **fp, int sign ) { 
	align_mem16(fp, 0);
	insns_t * x_8_addr = *fp;
	align_mem16(fp, 5);
	memcpy(*fp, x8_soft, x8_hard - x8_soft);
	*fp += (x8_hard - x8_soft);
	return x_8_addr;
}

insns_t* generate_size4_base_case(insns_t **fp, int sign) {
	align_mem16(fp, 0);
	insns_t * x_4_addr = *fp;
	memcpy(*fp, x4, x8_soft - x4);
	*fp += (x8_soft - x4);
	return x_4_addr;

}
