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

#include "arch/x64/x64-codegen.h"

#include <assert.h>
#include <string.h>

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

/* forward declarations */
static void IMM8(uint8_t **p, int32_t imm);
static void IMM32(uint8_t **p, int32_t imm);

static void IMM8(uint8_t **p, int32_t imm)
{
    *(*p)++ = (imm & 0xff);
}

static void IMM32(uint8_t **p, int32_t imm)
{
    int i;

    for (i = 0; i < 4; i++) {
        *(*p)++ = (imm & (0xff << (8 * i))) >> (8 * i);
    }
}

static void IMM64(uint8_t **p, int64_t imm)
{
    int i;

    for (i = 0; i < 8; i++) {
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

static FFTS_INLINE void MOVAPS(uint8_t **p, uint8_t reg1, uint8_t reg2, int32_t disp, int is_store)
{
    uint8_t r1 = (reg1 & 7);
    uint8_t r2 = (reg2 & 7);
    uint8_t	r;

    /* REX prefix */
    if ((reg1 & 8) || (reg2 & 8)) {
        *(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
    }

    /* esacape opcode */
    *(*p)++ = 0x0F;

    /* opcode */
    if (is_store) {
        *(*p)++ = 0x29;
    } else {
        *(*p)++ = 0x28;
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

static FFTS_INLINE void MOVAPS2(uint8_t **p, uint8_t reg1, uint8_t reg2)
{
    if (reg1 & XMM_REG) {
        MOVAPS(p, reg2, reg1, 0, 0);
    } else {
        MOVAPS(p, reg1, reg2, 0, 1);
    }
}

static FFTS_INLINE void MOVAPS3(uint8_t **p, uint8_t reg1, int32_t op2, int32_t op3)
{
    if (reg1 & XMM_REG) {
        MOVAPS(p, (uint8_t) op2, reg1, op3, 0);
    } else {
        MOVAPS(p, reg1, (uint8_t) op3, op2, 1);
    }
}

static FFTS_INLINE void MOVDQA(uint8_t **p, uint8_t reg1, uint8_t reg2, int32_t disp, int is_store)
{
    uint8_t r1 = (reg1 & 7);
    uint8_t r2 = (reg2 & 7);
    uint8_t	r;

    /* mandatory prefix */
    *(*p)++ = 0x66;

    /* REX prefix */
    if ((reg1 & 8) || (reg2 & 8)) {
        *(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
    }

    /* esacape opcode */
    *(*p)++ = 0x0F;

    /* opcode */
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

static FFTS_INLINE void MULPS(uint8_t **p, uint8_t reg2, uint8_t reg1)
{
    uint8_t r1 = (reg1 & 7);
    uint8_t r2 = (reg2 & 7);

    /* REX prefix */
    if ((reg1 & 8) || (reg2 & 8)) {
        *(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
    }

    /* esacape opcode */
    *(*p)++ = 0x0F;

    /* opcode */
    *(*p)++ = 0x59;
    *(*p)++ = 0xC0 | r1 | (r2 << 3);
}

static FFTS_INLINE void SHUFPS(uint8_t **p, uint8_t reg2, uint8_t reg1, const int select)
{
    uint8_t r1 = (reg1 & 7);
    uint8_t r2 = (reg2 & 7);
    uint8_t	r;

    /* REX prefix */
    if ((reg1 & 8) || (reg2 & 8)) {
        *(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
    }

    /* esacape opcode */
    *(*p)++ = 0x0F;

    /* opcode */
    *(*p)++ = 0xC6;

    r = r1 | (r2 << 3);

    *(*p)++ = 0xC0 | r;
    *(*p)++ = (select & 0xFF);
}

static FFTS_INLINE void SUBPS(uint8_t **p, uint8_t reg2, uint8_t reg1)
{
    uint8_t r1 = (reg1 & 7);
    uint8_t r2 = (reg2 & 7);

    /* REX prefix */
    if ((reg1 & 8) || (reg2 & 8)) {
        *(*p)++ = 0x40 | ((reg1 & 8) >> 3) | ((reg2 & 8) >> 1);
    }

    /* esacape opcode */
    *(*p)++ = 0x0F;

    /* opcode */
    *(*p)++ = 0x5C;
    *(*p)++ = 0xC0 | r1 | (r2 << 3);
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

static FFTS_INLINE void generate_epilogue(insns_t **fp)
{
#ifdef _M_X64
    /* restore nonvolatile registers */
    MOVDQA3(fp, XMM6,  X64_RSP,   0);
    MOVDQA3(fp, XMM7,  X64_RSP,  16);
    MOVDQA3(fp, XMM8,  X64_RSP,  32);
    MOVDQA3(fp, XMM9,  X64_RSP,  48);
    MOVDQA3(fp, XMM10, X64_RSP,  64);
    MOVDQA3(fp, XMM11, X64_RSP,  80);
    MOVDQA3(fp, XMM12, X64_RSP,  96);
    MOVDQA3(fp, XMM13, X64_RSP, 112);
    MOVDQA3(fp, XMM14, X64_RSP, 128);
    MOVDQA3(fp, XMM15, X64_RSP, 144);

    /* restore stack */
	x64_alu_reg_imm_size(*fp, X86_ADD, X64_RSP, 168, 8);

    /* restore the last 3 registers from the shadow space */
	x64_mov_reg_membase(*fp, X64_RBX, X64_RSP,  8, 8);
	x64_mov_reg_membase(*fp, X64_RSI, X64_RSP, 16, 8);
	x64_mov_reg_membase(*fp, X64_RDI, X64_RSP, 24, 8);
#else	
    x64_pop_reg(*fp, X64_R15);
    x64_pop_reg(*fp, X64_R14);
    x64_pop_reg(*fp, X64_R13);
    x64_pop_reg(*fp, X64_R12);
    x64_pop_reg(*fp, X64_R11);
    x64_pop_reg(*fp, X64_R10);
    x64_pop_reg(*fp, X64_RBX);
    x64_pop_reg(*fp, X64_RBP);
#endif

    x64_ret(*fp);
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
	x64_mov_membase_reg(*fp, X64_RSP,  8, X64_RBX, 8);
	x64_mov_membase_reg(*fp, X64_RSP, 16, X64_RSI, 8);
	x64_mov_membase_reg(*fp, X64_RSP, 24, X64_RDI, 8);

    /* reserve space.. */
	x64_alu_reg_imm_size(*fp, X86_SUB, X64_RSP, 168, 8);

    /* to save XMM6-XMM15 registers */
    MOVDQA3(fp, X64_RSP,   0,  XMM6);
    MOVDQA3(fp, X64_RSP,  16,  XMM7);
    MOVDQA3(fp, X64_RSP,  32,  XMM8);
    MOVDQA3(fp, X64_RSP,  48,  XMM9);
    MOVDQA3(fp, X64_RSP,  64, XMM10);
    MOVDQA3(fp, X64_RSP,  80, XMM11);
    MOVDQA3(fp, X64_RSP,  96, XMM12);
    MOVDQA3(fp, X64_RSP, 112, XMM13);
    MOVDQA3(fp, X64_RSP, 128, XMM14);
    MOVDQA3(fp, X64_RSP, 144, XMM15);
#else
	x64_push_reg(*fp, X64_RBP);
	x64_push_reg(*fp, X64_RBX);
	x64_push_reg(*fp, X64_R10);
	x64_push_reg(*fp, X64_R11);
	x64_push_reg(*fp, X64_R12);
	x64_push_reg(*fp, X64_R13);
	x64_push_reg(*fp, X64_R14);
	x64_push_reg(*fp, X64_R15);
#endif

    return start;
}

static FFTS_INLINE void generate_transform_init(insns_t **fp)
{
#ifdef _M_X64
    /* generate function */
    MOVAPS2(fp, XMM3, X64_RSI);

    /* set "pointer" to twiddle factors */
	x64_mov_reg_membase(*fp, X64_RDI, X64_RCX, 0x20, 8);
#else
    size_t len;

    /* copy function */
    assert((char*) x4 > (char*) x_init);
    len = (char*) x4 - (char*) x_init;
    memcpy(*fp, x_init, len);
    *fp += len;
#endif
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
#ifdef _M_X64
    insns_t *x8_soft_loop;
#else
    size_t len;
#endif

    /* align call destination */
    ffts_align_mem16(fp, 0);
    x_8_addr = *fp;

    /* align loop/jump destination */
#ifdef _M_X64
    ffts_align_mem16(fp, 6);
#else
    ffts_align_mem16(fp, 5);
#endif

#ifdef _M_X64
    /* input */
	x64_mov_reg_reg(*fp, X64_RAX, X64_RDI, 8);

    /* output */
	x64_mov_reg_reg(*fp, X64_RCX, X64_R8, 8);

    /* lea rdx, [r8 + rbx] */
    /* loop stop (output + output_stride) */
    *(*fp)++ = 0x49;
    *(*fp)++ = 0x8D;
    *(*fp)++ = 0x14;
    *(*fp)++ = 0x18;

    /* lea rsi, [rbx + rbx*2] */
    /* 3 * output_stride */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x8D;
    *(*fp)++ = 0x34;
    *(*fp)++ = 0x5B;

    /* lea r10, [rbx + rbx*4] */
    /* 5 * output_stride */
    *(*fp)++ = 0x4C;
    *(*fp)++ = 0x8D;
    *(*fp)++ = 0x14;
    *(*fp)++ = 0x9B;

    /* lea r11, [rsi + rbx*4] */
    /* 7 * output_stride */
    *(*fp)++ = 0x4C;
    *(*fp)++ = 0x8D;
    *(*fp)++ = 0x1C;
    *(*fp)++ = 0x9E;

    x8_soft_loop = *fp;
    assert(!(((uintptr_t) x8_soft_loop) & 0xF));

    /* movaps xmm9, [rax] */
    /* input + 0 * input_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x08;

    /* movaps xmm6, [rcx + rbx*2] */
    /* output + 2 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x34;
    *(*fp)++ = 0x59;

    /* movaps xmm11, xmm9 */
    *(*fp)++ = 0x45;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xD9;

    /* movaps xmm7, [rcx + rsi] */
    /* output + 3 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x3C;
    *(*fp)++ = 0x31;

    /* movaps xmm8, [rax + 0x10] */
    /* input + 1 * input_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x40;
    *(*fp)++ = 0x10;

    MULPS(fp, XMM11, XMM6);
    MULPS(fp, XMM9, XMM7);
    SHUFPS(fp, XMM6, XMM6, 0xB1);
    MULPS(fp, XMM6, XMM8);
    SHUFPS(fp, XMM7, XMM7, 0xB1);
    SUBPS(fp, XMM11, XMM6);
    MULPS(fp, XMM8, XMM7);

    /* movaps xmm10, xmm11 */
    *(*fp)++ = 0x45;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xD3;

	x64_sse_addps_reg_reg(*fp, X64_XMM9, X64_XMM8);

    /* movaps xmm15, [rax + 0x20]  */
    /* input + 2 * input_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x78;
    *(*fp)++ = 0x20;

	x64_sse_addps_reg_reg(*fp, X64_XMM10, X64_XMM9);
    SUBPS(fp, XMM11, XMM9);

    /* movaps xmm5, [rcx] */
    /* output + 0 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x29;

    /* movaps xmm6,xmm15 */
    *(*fp)++ = 0x41;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xF7;

    /* movaps xmm12, [rcx + rbx*4]  */
    /* output + 4 * output_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x99;

    /* movaps xmm2, xmm5 */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xD5;

    /* movaps xmm13, [rcx + rsi*2] */
    /* output + 6 * output_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x2C;
    *(*fp)++ = 0x71;

	x64_sse_xorps_reg_reg(*fp, X64_XMM11, X64_XMM3);

    /* movaps xmm14, [rax + 0x30] */
    /* input + 3 * input_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x70;
    *(*fp)++ = 0x30;

    SUBPS(fp, XMM2, XMM10);
    MULPS(fp, XMM6, XMM12);
	x64_sse_addps_reg_reg(*fp, X64_XMM5, X64_XMM10);

    MULPS(fp, XMM15, XMM13);

    /* movaps xmm10, [rax + 0x40] */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x50;
    *(*fp)++ = 0x40;

    /* movaps xmm0, xmm5 */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xC5;

    SHUFPS(fp, XMM12, XMM12, 0xB1);
    SHUFPS(fp, XMM13, XMM13, 0xB1);
    MULPS(fp, XMM12, XMM14);
    MULPS(fp, XMM14, XMM13);
    SUBPS(fp, XMM6, XMM12);
	x64_sse_addps_reg_reg(*fp, X64_XMM15, X64_XMM14);

    /* movaps xmm7, [rcx + r10] */
    *(*fp)++ = 0x42;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x3C;
    *(*fp)++ = 0x11;

    /* movaps xmm13, xmm10 */
    *(*fp)++ = 0x45;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xEA;

    /* movaps xmm8, [rcx + r11] */
    *(*fp)++ = 0x46;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x04;
    *(*fp)++ = 0x19;

    /* movaps xmm12, xmm6 */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xE6;

    /* movaps xmm9, [rax + 0x50] */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x50;

    /* input + 6 * input_stride */
	x64_alu_reg_imm_size(*fp, X86_ADD, X64_RAX, 0x60, 8);

    MULPS(fp, XMM13, XMM7);
    SUBPS(fp, XMM6, XMM15);
	x64_sse_addps_reg_reg(*fp, X64_XMM12, X64_XMM15);
    MULPS(fp, XMM10, XMM8);
    SUBPS(fp, XMM0, XMM12);
	x64_sse_addps_reg_reg(*fp, X64_XMM5, X64_XMM12);
    SHUFPS(fp, XMM7, XMM7, 0xB1);
	x64_sse_xorps_reg_reg(*fp, X64_XMM6, X64_XMM3);

    SHUFPS(fp, XMM8, XMM8, 0xB1);

    /* movaps xmm12, xmm2 */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xE2;

    MULPS(fp, XMM7, XMM9);
    MULPS(fp, XMM9, XMM8);
    SUBPS(fp, XMM13, XMM7);
	x64_sse_addps_reg_reg(*fp, X64_XMM10, X64_XMM9);

    /* movaps xmm4, [rcx + rbx] */
    /* output + 1 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x19;

    SHUFPS(fp, XMM11, XMM11, 0xB1);

    /* movaps xmm1, xmm4 */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xCC;

    SHUFPS(fp, XMM6, XMM6, 0xB1);
	x64_sse_addps_reg_reg(*fp, X64_XMM1, X64_XMM11);
    SUBPS(fp, XMM4, XMM11);
	x64_sse_addps_reg_reg(*fp, X64_XMM12, X64_XMM6);
    SUBPS(fp, XMM2, XMM6);

    /* movaps xmm11, xmm13 */
    *(*fp)++ = 0x45;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xDD;

    /* movaps xmm14, xmm4 */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xF4;

    /* movaps xmm6, xmm1 */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x28;
    *(*fp)++ = 0xF1;

    SUBPS(fp, XMM13, XMM10);
	x64_sse_addps_reg_reg(*fp, X64_XMM11, X64_XMM10);
	x64_sse_xorps_reg_reg(*fp, X64_XMM13, X64_XMM3);
	x64_sse_addps_reg_reg(*fp, X64_XMM4, X64_XMM11);
    SUBPS(fp, XMM14, XMM11);
    SHUFPS(fp, XMM13, XMM13, 0xB1);

    /* movaps [rcx], xmm5 */
    /* output + 0 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x29;

    /* movaps [rcx + rbx], xmm4 */
    /* output + 1 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x19;

    /* movaps [rcx + rbx*2], xmm2 */
    /* output + 2 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x14;
    *(*fp)++ = 0x59;

    SUBPS(fp, XMM1, XMM13);
	x64_sse_addps_reg_reg(*fp, X64_XMM6, X64_XMM13);

    /* movaps [rcx + rsi], xmm1 */
    /* output + 3 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x0C;
    *(*fp)++ = 0x31;

    /* movaps [rcx + rbx*4], xmm0 */
    /* output + 4 * output_stride */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x04;
    *(*fp)++ = 0x99;

    /* movaps [rcx + r10], xmm14 */
    /* output + 5 * output_stride */
    *(*fp)++ = 0x46;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x34;
    *(*fp)++ = 0x11;

    /* movaps [rcx + rsi*2], xmm12 */
    /* output + 6 * output_stride */
    *(*fp)++ = 0x44;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x24;
    *(*fp)++ = 0x71;

    /* movaps [rcx + r11], xmm6 */
    /* output + 7 * output_stride */
    *(*fp)++ = 0x42;
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x29;
    *(*fp)++ = 0x34;
    *(*fp)++ = 0x19;

    /* add rcx, 0x10 */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x83;
    *(*fp)++ = 0xC1;
    *(*fp)++ = 0x10;

    /* cmp rcx, rdx */
    *(*fp)++ = 0x48;
    *(*fp)++ = 0x39;
    *(*fp)++ = 0xD1;

    /* jne [x8_soft_loop] */
    *(*fp)++ = 0x0F;
    *(*fp)++ = 0x85;
    *(*fp)++ = 0x9E;
    *(*fp)++ = 0xFE;
    *(*fp)++ = 0xFF;
    *(*fp)++ = 0xFF;

    /* ret */
    x64_ret(*fp);
#else
    /* copy function */
    assert((char*) x8_soft_end >= (char*) x8_soft);
    len = (char*) x8_soft_end - (char*) x8_soft;
    memcpy(*fp, x8_soft, len);
    *fp += len;
#endif

    return x_8_addr;
}

#endif /* FFTS_CODEGEN_SSE_H */