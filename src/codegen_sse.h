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

#ifndef FFTS_CODEGEN_SSE_H
#define FFTS_CODEGEN_SSE_H

#include <assert.h>

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
void x8_soft_end();

void sse_constants();
void sse_constants_inv();

//	typedef uint8_t insns_t;

extern const uint32_t sse_leaf_ee_offsets[8];
extern const uint32_t sse_leaf_oo_offsets[8];
extern const uint32_t sse_leaf_eo_offsets[8];
extern const uint32_t sse_leaf_oe_offsets[8];

#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

#define XMM_REG 0x40

#define XMM0  (XMM_REG | 0x0)
#define XMM1  (XMM_REG | 0x1)
#define XMM2  (XMM_REG | 0x2) 
#define XMM3  (XMM_REG | 0x3) 
#define XMM4  (XMM_REG | 0x4)
#define XMM5  (XMM_REG | 0x5)
#define XMM6  (XMM_REG | 0x6)
#define XMM7  (XMM_REG | 0x7)
#define XMM8  (XMM_REG | 0x8)
#define XMM9  (XMM_REG | 0x9)
#define XMM10 (XMM_REG | 0xa)
#define XMM11 (XMM_REG | 0xb)
#define XMM12 (XMM_REG | 0xc)
#define XMM13 (XMM_REG | 0xd)
#define XMM14 (XMM_REG | 0xe)
#define XMM15 (XMM_REG | 0xf)

#define P(x) (*(*p)++ = x)

static void IMM8(uint8_t **p, int32_t imm)
{
    *(*p)++ = (imm & 0xff);
}

static void IMM16(uint8_t **p, int32_t imm)
{
    int i;

    for (i = 0; i < 2; i++) {
        *(*p)++ = (imm & (0xff << (8 * i))) >> (8 * i);
    }
}

static void IMM32(uint8_t **p, int32_t imm)
{
    int i;

    for (i = 0; i < 4; i++) {
        *(*p)++ = (imm & (0xff << (8 * i))) >> (8 * i);
    }
}

static void IMM32_NI(uint8_t *p, int32_t imm)
{
    int i;

    for (i = 0; i < 4; i++) {
        *(p+i) = (imm & (0xff << (8 * i))) >> (8 * i);
    }
}

static int32_t READ_IMM32(uint8_t *p)
{
    int32_t rval = 0;
    int i;

    for (i = 0; i < 4; i++) {
        rval |= *(p+i) << (8 * i);
    }

    return rval;
}

static void MOVI(uint8_t **p, uint8_t dst, uint32_t imm)
{
    if (dst >= 8) {
        *(*p)++ = 0x41;
    }

    *(*p)++ = 0xb8 | (dst & 0x7);
    IMM32(p, imm);
}

static void ADDRMODE(uint8_t **p, uint8_t reg, uint8_t rm, int32_t disp)
{
    if (disp == 0) {
        *(*p)++ = (rm & 7) | ((reg & 7) << 3);
    } else if (disp <= 127 || disp >= -128) {
        *(*p)++ = 0x40 | (rm & 7) | ((reg & 7) << 3);
        IMM8(p, disp);
    } else {
        *(*p)++ = 0x80 | (rm & 7) | ((reg & 7) << 3);
        IMM32(p, disp);
    }
}

static FFTS_INLINE void MOVDQA(uint8_t **p, uint8_t reg1, uint8_t reg2, int32_t disp, int is_store)
{
	uint8_t r1 = (reg1 & 7);
	uint8_t r2 = (reg2 & 7);
	uint8_t	r;

	*(*p)++ = 0x66;

	if ((reg1 & 8) || (reg2 & 8)) {
		*(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
	}

	*(*p)++ = 0x0F;

	if (is_store) {
		*(*p)++ = 0x7F;
	} else {
		*(*p)++ = 0x6F;
	}

	r = r1 | (r2 << 3);

 	if ((reg1 & XMM_REG) && (reg2 & XMM_REG)) {
		assert(disp == 0);
		*(*p)++ = 0xC0 | r;
	} else {
		assert((reg1 & XMM_REG) || (reg2 & XMM_REG));

		if (disp == 0 && r1 != 5) {
			*(*p)++ = r;

			if (r1 == 4) {
				*(*p)++ = 0x24;
			}
		} else {
			if (disp <= 127 && disp >= -128) {
				*(*p)++ = 0x40 | r;

				if (r1 == 4) {
					*(*p)++ = 0x24;
				}

				IMM8(p, disp);
			} else {
				*(*p)++ = 0x80 | r;

				if (r1 == 4) {
					*(*p)++ = 0x24;
				}

				IMM32(p, disp);
			}
		}
	}
}

static FFTS_INLINE void MOVDQA2(uint8_t **p, uint8_t reg1, uint8_t reg2)
{
	if (reg1 & XMM_REG) {
		MOVDQA(p, reg2, reg1, 0, 0);
	} else {
		MOVDQA(p, reg1, reg2, 0, 1);
	}
}

static FFTS_INLINE void MOVDQA3(uint8_t **p, uint8_t reg1, int32_t op2, int32_t op3)
{
	if (reg1 & XMM_REG) {
		MOVDQA(p, (uint8_t) op2, reg1, op3, 0);
	} else {
		MOVDQA(p, reg1, (uint8_t) op3, op2, 1);
	}
}

static void LEA(uint8_t **p, uint8_t dst, uint8_t base, int32_t disp)
{
    *(*p)++ = 0x48 | ((base & 0x8) >> 3) | ((dst & 0x8) >> 1);
    *(*p)++ = 0x8d;
    ADDRMODE(p, dst, base, disp);
}

static void RET(uint8_t **p)
{
    *(*p)++ = 0xc3;
}

static void ADDI(uint8_t **p, uint8_t dst, int32_t imm)
{
    if (dst >= 8) {
        *(*p)++ = 0x49;
    } else {
        *(*p)++ = 0x48;
    }

    if (imm > 127 || imm <= -128) {
        *(*p)++ = 0x81;
    } else {
        *(*p)++ = 0x83;
    }

    *(*p)++ = 0xc0 | (dst & 0x7);

    if (imm > 127 || imm <= -128) {
        IMM32(p, imm);
    } else {
        IMM8(p, imm);
    }
}

static void SUBI(uint8_t **p, uint8_t dst, int32_t imm)
{
    if (dst >= 8) {
        *(*p)++ = 0x49;
    } else {
        *(*p)++ = 0x48;
    }

    if (imm > 127 || imm <= -128) {
        *(*p)++ = 0x81;
    } else {
        *(*p)++ = 0x83;
    }

    *(*p)++ = 0xe8 | (dst & 0x7);

    if (imm > 127 || imm <= -128) {
        IMM32(p, imm);
    } else {
        IMM8(p, imm);
    }
}

static void CALL(uint8_t **p, uint8_t *func)
{
    *(*p)++ = 0xe8;
    IMM32(p, func - *p - 4);
}

static void PUSH(uint8_t **p, uint8_t reg)
{
    if (reg >= 8) {
        *(*p)++ = 0x41;
    }

    *(*p)++ = 0x50 | (reg & 7);
}

static void POP(uint8_t **p, uint8_t reg)
{
    if (reg >= 8) {
        *(*p)++ = 0x41;
    }

    *(*p)++ = 0x58 | (reg & 7);
}

static void SHIFT(uint8_t **p, uint8_t reg, int shift)
{
    if (reg >= 8) {
        *(*p)++ = 0x49;
    }


    *(*p)++ = 0xc1;
    if (shift > 0) {
        *(*p)++ = 0xe0 | (reg & 7);
        *(*p)++ = (shift & 0xff);
    } else {
        *(*p)++ = 0xe8 | (reg & 7);
        *(*p)++ = ((-shift) & 0xff);
    }
}

static FFTS_INLINE void ffts_insert_nops(uint8_t **p, uint32_t count)
{
    if (count >= 9) {
        P(0x66);
        P(0x0F);
        P(0x1F);
        P(0x84);
        P(0x00);
        P(0x00);
        P(0x00);
        P(0x00);
        P(0x00);

        if (count > 9) {
            ffts_insert_nops(p, count - 9);
        }
    } else {
        switch(count) {
        case 0:
            break;
        case 2:
            P(0x66);
        /* fall through */
        case 1:
            P(0x90);
            break;
        case 3:
            P(0x0F);
            P(0x1F);
            P(0x00);
            break;
        case 4:
            P(0x0F);
            P(0x1F);
            P(0x40);
            P(0x00);
            break;
        case 5:
            P(0x0F);
            P(0x1F);
            P(0x44);
            P(0x00);
            P(0x00);
            break;
        case 6:
            P(0x66);
            P(0x0F);
            P(0x1F);
            P(0x44);
            P(0x00);
            P(0x00);
            break;
        case 7:
            P(0x0F);
            P(0x1F);
            P(0x80);
            P(0x00);
            P(0x00);
            P(0x00);
            P(0x00);
            break;
        case 8:
        default:
            P(0x0F);
            P(0x1F);
            P(0x84);
            P(0x00);
            P(0x00);
            P(0x00);
            P(0x00);
            P(0x00);
            break;
        }
    }
}

static FFTS_INLINE void ffts_align_mem16(uint8_t **p, uint32_t offset)
{
    int r = (16 - (offset & 0xf)) - ((uintptr_t)(*p) & 0xf);
    r = (16 + r) & 0xf;
    ffts_insert_nops(p, r);
}

static FFTS_INLINE insns_t* generate_size4_base_case(insns_t **fp, int sign)
{
	insns_t *x_4_addr;
	size_t len;

	/* align call destination */
	ffts_align_mem16(fp, 0);
	x_4_addr = *fp;

	/* copy function */
    assert((char*) x8_soft > (char*) x4);
    len = (char*) x8_soft - (char*) x4;
	memcpy(*fp, x4, len);
    *fp += len;

	return x_4_addr;
}

static FFTS_INLINE insns_t* generate_size8_base_case(insns_t **fp, int sign)
{
	insns_t *x_8_addr;
	size_t len;

	/* align call destination */
	ffts_align_mem16(fp, 0);
	x_8_addr = *fp;

    /* align loop/jump destination */
#ifdef _M_X64
    ffts_align_mem16(fp, 6);
#else
    ffts_align_mem16(fp, 5);
#endif

	/* copy function */
    assert((char*) x8_soft_end > (char*) x8_soft);
    len = (char*) x8_soft_end - (char*) x8_soft;
	memcpy(*fp, x8_soft, len);
	*fp += len;

	return x_8_addr;
}

static FFTS_INLINE insns_t* generate_prologue(insns_t **fp, ffts_plan_t *p)
{
	insns_t	*start;

	/* align call destination */
	ffts_align_mem16(fp, 0);
	start = *fp;

    /* save nonvolatile registers */
#ifdef _M_X64
    /* use the shadow space to save first 3 registers */

    /* mov [rsp + 8], rbx */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x89;
    *(*fp)++ = 0x5C;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x08;

    /* mov [rsp + 16], rsi */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x89;
    *(*fp)++ = 0x74;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x10;

    /* mov [rsp + 24], rdi */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x89;
    *(*fp)++ = 0x7C;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x18;

    /* reserve space to save XMM6-XMM15 registers */
	SUBI(fp, RSP, 168);

	MOVDQA3(fp, RSP,   0,  XMM6);
	MOVDQA3(fp, RSP,  16,  XMM7);
	MOVDQA3(fp, RSP,  32,  XMM8);
	MOVDQA3(fp, RSP,  48,  XMM9);
	MOVDQA3(fp, RSP,  64, XMM10);
	MOVDQA3(fp, RSP,  80, XMM11);
	MOVDQA3(fp, RSP,  96, XMM12);
	MOVDQA3(fp, RSP, 112, XMM13);
	MOVDQA3(fp, RSP, 128, XMM14);
	MOVDQA3(fp, RSP, 144, XMM15);
#else
    PUSH(fp, RBP);
    PUSH(fp, RBX);
    PUSH(fp, R10);
    PUSH(fp, R11);
    PUSH(fp, R12);
    PUSH(fp, R13);
    PUSH(fp, R14);
    PUSH(fp, R15);
#endif

	return start;
}

static FFTS_INLINE void generate_epilogue(insns_t **fp)
{
#ifdef _M_X64
    /* restore nonvolatile registers */
	MOVDQA3(fp, XMM6,  RSP,   0);
	MOVDQA3(fp, XMM7,  RSP,  16);
	MOVDQA3(fp, XMM8,  RSP,  32);
	MOVDQA3(fp, XMM9,  RSP,  48);
	MOVDQA3(fp, XMM10, RSP,  64);
	MOVDQA3(fp, XMM11, RSP,  80);
	MOVDQA3(fp, XMM12, RSP,  96);
	MOVDQA3(fp, XMM13, RSP, 112);
	MOVDQA3(fp, XMM14, RSP, 128);
	MOVDQA3(fp, XMM15, RSP, 144);

	/* restore stack */
	ADDI(fp, RSP, 168);

    /* restore the last 3 registers from the shadow space */

	/* mov rbx, [rsp + 8] */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x8B;
    *(*fp)++ = 0x5C;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x08;

    /* mov rsi, [rsp + 16] */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x8B;
    *(*fp)++ = 0x74;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x10;

    /* mov rdi, [rsp + 24] */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x8B;
    *(*fp)++ = 0x7C;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x18;
#else
    POP(fp, R15);
    POP(fp, R14);
    POP(fp, R13);
    POP(fp, R12);
    POP(fp, R11);
    POP(fp, R10);
    POP(fp, RBX);
    POP(fp, RBP);
#endif

    RET(fp);
}

#endif /* FFTS_CODEGEN_SSE_H */