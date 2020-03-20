

#include <stdio.h>

#include "unity.h"
#include "test_cases.h"

int main(int argc, char** argv)
{
    int retval = 0;

    

    /* **** Basic **** */

    retval = test_CLS_funcs();
    retval = test_HR_funcs();


    /* **** Low-level API **** */

    retval = test_xs3_set_vect();
    retval = test_xs3_headroom_vect();
    retval = test_xs3_shl_vect();
    retval = test_xs3_add_vect();
    retval = test_xs3_sub_vect();
    retval = test_xs3_mul_vect();
    retval = test_xs3_scalar_mul_vect();
    retval = test_xs3_rect_vect();
    retval = test_xs3_abs_vect();
    retval = test_xs3_clip_vect();

    retval = test_xs3_sum();
    retval = test_xs3_dot();
    
    retval = test_xs3_s32_to_s16();
    retval = test_xs3_s16_to_s32();


    /* **** High-level API **** */

    retval = test_bfp_init_vect();
    retval = test_bfp_set_vect();
    retval = test_bfp_headroom_vect();
    retval = test_bfp_ldexp_vect();
    retval = test_bfp_add_vect();
    retval = test_bfp_sub_vect();
    retval = test_bfp_abs_vect();
    retval = test_bfp_rect_vect();

    retval = test_bfp_sum();

    retval = test_bfp_s32_to_s16();
    retval = test_bfp_s16_to_s32();

    return retval;
}
