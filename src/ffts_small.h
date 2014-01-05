#ifndef __FFTS_SMALL_H__
#define __FFTS_SMALL_H__


void firstpass_16_f(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_16_b(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_8_f(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_8_b(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_4_f(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_4_b(ffts_plan_t *  p, const void *  in, void *  out);
void firstpass_2(ffts_plan_t *  p, const void *  in, void *  out);

#endif
// vim: set autoindent noexpandtab tabstop=3 shiftwidth=3:
