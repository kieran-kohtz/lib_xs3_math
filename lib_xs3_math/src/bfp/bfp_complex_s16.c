

#include "bfp_math.h"

#include "vect/xs3_vect_s32.h"
#include "vect/xs3_vect_s16.h"
#include "../vect/vpu_helper.h"



const extern unsigned rot_table16_rows;
const extern int16_t rot_table16[14][2][16];


headroom_t bfp_complex_s16_headroom(
    bfp_complex_s16_t* a)
{
    a->hr = xs3_vect_complex_s16_headroom(a->real, a->imag, a->length);
    return a->hr;
}


void bfp_complex_s16_shl(
    bfp_complex_s16_t* a,
    const bfp_complex_s16_t* b,
    const left_shift_t shl)
{
#if XS3_BFP_DEBUG_CHECK_LENGTHS
    assert(a->length == b->length);
#endif

    a->length = b->length;
    a->exp = b->exp;

    const headroom_t re_hr = xs3_vect_s16_shl(a->real, b->real, b->length, shl);
    const headroom_t im_hr = xs3_vect_s16_shl(a->imag, b->imag, b->length, shl);
    a->hr = (re_hr <= im_hr)? re_hr : im_hr;
}


void bfp_complex_s16_add(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const bfp_complex_s16_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
    assert(b->length == c->length);
#endif

    right_shift_t b_shr, c_shr;

    xs3_vect_add_sub_calc_params(&a->exp, &b_shr, &c_shr, b->exp, c->exp, 
                             b->hr, c->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    
    a->hr = xs3_vect_complex_s16_add(a->real, a->imag, b->real, b->imag, c->real, c->imag, 
                                     b->length, b_shr, c_shr);
}


void bfp_complex_s16_sub(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const bfp_complex_s16_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
    assert(b->length == c->length);
#endif

    right_shift_t b_shr, c_shr;

    xs3_vect_add_sub_calc_params(&a->exp, &b_shr, &c_shr, b->exp, c->exp, 
                             b->hr, c->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    
    a->hr = xs3_vect_complex_s16_sub(a->real, a->imag, b->real, b->imag, c->real, c->imag,
                                     b->length, b_shr, c_shr);
}


void bfp_complex_s16_real_mul(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const bfp_s16_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
    assert(b->length == c->length);
#endif

    exponent_t a_exp;
    right_shift_t sat;

    xs3_vect_complex_s16_real_mul_calc_params(&a_exp, &sat, b->exp, c->exp, b->hr, c->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    a->exp = a_exp;
    
    const headroom_t re_hr = xs3_vect_s16_mul(a->real, b->real, c->data, b->length, sat);
    const headroom_t im_hr = xs3_vect_s16_mul(a->imag, b->imag, c->data, b->length, sat);
    a->hr = (re_hr <= im_hr)? re_hr : im_hr;
}


void bfp_complex_s16_mul(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const bfp_complex_s16_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
    assert(b->length == c->length);
#endif

    exponent_t a_exp;
    right_shift_t sat;

    xs3_vect_complex_s16_complex_mul_calc_params(&a_exp, &sat, b->exp, c->exp, b->hr, c->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    a->exp = a_exp;
    a->hr = xs3_vect_complex_s16_complex_mul(a->real, a->imag, b->real, b->imag, c->real, c->imag, b->length, sat);
}


void bfp_complex_s16_conj_mul(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const bfp_complex_s16_t* c)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
    assert(b->length == c->length);
#endif

    exponent_t a_exp;
    right_shift_t sat;

    xs3_vect_complex_s16_complex_mul_calc_params(&a_exp, &sat, b->exp, c->exp, b->hr, c->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    a->exp = a_exp;
    a->hr = xs3_vect_complex_s16_conj_mul(a->real, a->imag, b->real, b->imag, c->real, c->imag, b->length, sat);
}


void bfp_complex_s16_real_scale(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b, 
    const int16_t alpha_mant,
    const exponent_t alpha_exp)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
#endif

    right_shift_t sat;
    headroom_t s_hr = HR_S16(alpha_mant);

    xs3_vect_s16_scale_calc_params(&a->exp, &sat, b->exp, alpha_exp, 
                                                b->hr, s_hr, XS3_BFP_ALLOW_SATURATION);
    a->length = b->length;
    a->hr = xs3_vect_complex_s16_real_scale(a->real, a->imag, b->real, b->imag, alpha_mant, b->length, sat);
}


void bfp_complex_s16_scale(
    bfp_complex_s16_t* a, 
    const bfp_complex_s16_t* b,
    const complex_s16_t alpha_mant,
    const exponent_t alpha_exp)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
#endif

    right_shift_t sat;

    headroom_t alpha_hr = HR_C16(alpha_mant);

    xs3_vect_complex_s16_complex_mul_calc_params(&a->exp, &sat, b->exp, alpha_exp, b->hr, alpha_hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;

    a->hr = xs3_vect_complex_s16_scale(a->real, a->imag, b->real, b->imag, 
                                                  alpha_mant.re, alpha_mant.im, 
                                                  b->length, sat);
}


void bfp_complex_s16_squared_mag(
    bfp_s16_t* a, 
    const bfp_complex_s16_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
#endif

    right_shift_t sat;

    xs3_vect_complex_s16_squared_mag_calc_params(&a->exp, &sat, b->exp, b->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;
    a->hr = xs3_vect_complex_s16_squared_mag(a->data, b->real, b->imag, b->length, sat);
}


void bfp_complex_s16_mag(
    bfp_s16_t* a, 
    const bfp_complex_s16_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
#endif

    right_shift_t b_shr;

    xs3_vect_complex_mag_calc_params(&a->exp, &b_shr, b->exp, b->hr, XS3_BFP_ALLOW_SATURATION);

    a->length = b->length;

    a->hr = xs3_vect_complex_s16_mag(a->data, b->real, b->imag, b->length, 
                                     b_shr, (int16_t*) rot_table16, rot_table16_rows);
}


complex_s32_t bfp_complex_s16_sum(
    const bfp_complex_s16_t* b)
{
    return xs3_vect_complex_s16_sum(b->real, b->imag, b->length);
}


void bfp_complex_s16_to_complex_s32(
    bfp_complex_s32_t* a, 
    const bfp_complex_s16_t* b)
{
#if (XS3_BFP_DEBUG_CHECK_LENGTHS)
    assert(b->length == a->length);
#endif

    xs3_vect_complex_s16_to_complex_s32(a->data, b->real, b->imag, b->length);

    a->length = b->length;
    a->exp = b->exp;
    a->hr = b->hr + 16;
}