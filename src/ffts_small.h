#ifndef FFTS_SMALL_H
#define FFTS_SMALL_H

#include "ffts_internal.h"

void ffts_firstpass_16_f(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_16_b(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_8_f(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_8_b(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_4_f(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_4_b(ffts_plan_t *p, const void *in, void *out);
void ffts_firstpass_2(ffts_plan_t *p, const void *in, void *out);

#endif /* FFTS_SMALL_H */
