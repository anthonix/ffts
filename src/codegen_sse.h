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

#endif /* FFTS_CODEGEN_SSE_H */