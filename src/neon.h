#ifndef __NEON_H__
#define __NEON_H__

static const __attribute__ ((aligned(16))) float ee_w_data[4] = {0.70710678118654757273731092936941,0.70710678118654746171500846685376,
    -0.70710678118654757273731092936941,-0.70710678118654746171500846685376};
static const __attribute__ ((aligned(16))) float oe_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
static const __attribute__ ((aligned(16))) float eo_w_data[4] = {1.0f,0.70710678118654757273731092936941f, 0.0f,-0.70710678118654746171500846685376};
void neon_x4(float *, size_t, float *);
void neon_x8(float *, size_t, float *);
void neon_x8_t(float *, size_t, float *);
void neon_ee();
void neon_oo();
void neon_eo();
void neon_oe();
void neon_end();

#endif
