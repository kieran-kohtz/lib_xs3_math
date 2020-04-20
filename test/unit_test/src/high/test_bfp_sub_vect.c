
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xs3_math.h"

#include "../tst_common.h"

#include "unity.h"

#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


#define REPS        IF_QUICK_TEST(10, 100)
#define MAX_LEN     2  //Smaller lengths mean larger variance w.r.t. individual element headroom


static unsigned seed = 666;
static char msg_buff[200];

#define TEST_ASSERT_EQUAL_MSG(EXPECTED, ACTUAL, EXTRA, LINE_NUM)   do{          \
    if((EXPECTED)!=(ACTUAL)) {                                                  \
      sprintf(msg_buff, "%s (test vector @ line %u)", (EXTRA), (LINE_NUM));     \
      TEST_ASSERT_EQUAL_MESSAGE((EXPECTED), (ACTUAL), msg_buff);                \
    }} while(0)




static void test_bfp_sub_vect_calc_params()
{
    PRINTF("%s...\n", __func__);


    typedef struct {

        struct {    headroom_t hr;  exponent_t exp;     } b;
        struct {    headroom_t hr;  exponent_t exp;     } c;
        struct {    exponent_t exp; right_shift_t b_shr; right_shift_t c_shr;     } expected;

        unsigned line;
    } test_case_t;


    test_case_t casses[] = {
        //    b{   hr,  exp },      c{   hr,  exp },   expected{   exp,  b_shr, c_shr }   
        {      {    0,    0 },       {    0,    0 },           {     1,      1,    1  },       __LINE__ },
        {      {    1,    5 },       {    1,    5 },           {     5,      0,    0  },       __LINE__ },
        {      {    1,    0 },       {    0,    0 },           {     1,      1,    1  },       __LINE__ },
        {      {    0,   10 },       {    1,   10 },           {    11,      1,    1  },       __LINE__ },
        {      {   15,   -4 },       {   15,   -4 },           {   -18,    -14,  -14  },       __LINE__ },
        {      {    8,  -14 },       {    4,   -4 },           {    -7,      7,   -3  },       __LINE__ },
        {      {    8,  -14 },       {    4,   -4 },           {    -7,      7,   -3  },       __LINE__ },

    };

    const unsigned N_cases = sizeof(casses)/sizeof(test_case_t);

    const unsigned start_case = 0;

    for(int v = start_case; v < N_cases; v++){
        PRINTF("\ttest vector %d..\n", v);
        
        test_case_t* casse = &casses[v];

        exponent_t a_exp;
        right_shift_t b_shr, c_shr;


        int exp_mods[] = {0, 44, -534};

        for( int m = 0; m < sizeof(exp_mods)/sizeof(int); m++){

            PRINTF("\t\t\texp_delta = %d\n", exp_mods[m]);
    

            for(int sbc = 0; sbc <= 1; sbc++){

                PRINTF("\t\t\t\tswap b&c: %d\n", sbc);

                if(sbc){
                    bfp_sub_vect_calc_params(&a_exp, &c_shr, &b_shr, 
                                casse->c.exp + exp_mods[m], casse->b.exp + exp_mods[m], 
                                casse->c.hr,  casse->b.hr);
                } else {
                    bfp_sub_vect_calc_params(&a_exp, &b_shr, &c_shr, 
                                casse->b.exp + exp_mods[m], casse->c.exp + exp_mods[m], 
                                casse->b.hr,  casse->c.hr);
                }

                exponent_t expected_exp = casse->expected.exp + exp_mods[m];

                TEST_ASSERT_EQUAL_MSG(expected_exp,   a_exp,  "a_exp is wrong.", casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.b_shr, b_shr,  "b_shr is wrong.", casse->line);
                TEST_ASSERT_EQUAL_MSG(casse->expected.c_shr, c_shr,  "c_shr is wrong.", casse->line);
            }
        }

    }
}









static void test_bfp_sub_vect_s16_0()
{
    PRINTF("%s...\n", __func__);

    int16_t dataA[MAX_LEN];
    int16_t dataB[MAX_LEN];
    int16_t dataC[MAX_LEN];
    bfp_s16_t A, B, C;

    bfp_init_vect_s16(&A, dataA, 0, MAX_LEN, 0);
    bfp_init_vect_s16(&B, dataB, 0, MAX_LEN, 0);
    bfp_init_vect_s16(&C, dataC, 0, MAX_LEN, 0);

    typedef struct {

        struct {    int16_t value;  exponent_t exp;     } b;
        struct {    int16_t value;  exponent_t exp;     } c;
        struct {    int16_t value;  exponent_t exp;     } expected;

        unsigned line;
    } test_case_t;



    test_case_t casses[] = {
        //    b{   value,  exp },      c{   value,  exp },   expected{   value,  exp }
        {      { -0x0100,    0 },       {  0x0100,    0 },           { -0x4000,  -5  },        __LINE__ },
        {      {  0x00FF,    0 },       { -0x00FF,    0 },           {  0x7F80,  -6  },        __LINE__ },
        {      {  0x0001,    0 },       { -0x0000,    0 },           {  0x2000,  -13 },        __LINE__ },
        {      {  0x0002,    0 },       { -0x0000,    0 },           {  0x2000,  -12 },        __LINE__ },
        {      {  0x0000,    0 },       { -0x1111,    0 },           {  0x2222,  -1  },        __LINE__ },
        {      {  0x0001,    0 },       { -0x0001,    0 },           {  0x4000,  -13 },        __LINE__ },

        {      { -0x0100,    1 },       {  0x0100,    1 },           { -0x4000,  -4  },        __LINE__ },
        {      {  0x00FF,    1 },       { -0x00FF,    1 },           {  0x7F80,  -5  },        __LINE__ },
        {      {  0x0001,    1 },       { -0x0000,    1 },           {  0x2000,  -12 },        __LINE__ },
        {      {  0x0002,    1 },       { -0x0000,    1 },           {  0x2000,  -11 },        __LINE__ },
        {      {  0x0000,    1 },       { -0x1111,    1 },           {  0x2222,   0  },        __LINE__ },
        {      {  0x0001,    1 },       { -0x0001,    1 },           {  0x4000,  -12 },        __LINE__ },
        
        {      {  0x0001,    1 },       { -0x0001,    0 },           {  0x3000,  -12 },        __LINE__ },
        {      {  0x0001,    0 },       { -0x0001,    1 },           {  0x6000,  -13 },        __LINE__ },
        {      {  0x0002,    0 },       { -0x0001,    0 },           {  0x3000,  -12 },        __LINE__ },
        {      {  0x0002,    0 },       { -0x0010,   -4 },           {  0x3000,  -12 },        __LINE__ },

    };

    const unsigned N_cases = sizeof(casses)/sizeof(test_case_t);

    const unsigned start_case = 0;

    for(int v = start_case; v < N_cases; v++){
        PRINTF("\ttest vector %d..\n", v);
        
        test_case_t* casse = &casses[v];

        bfp_set_vect_s16(&B, casse->b.value, casse->b.exp);
        bfp_set_vect_s16(&C, casse->c.value, casse->c.exp);

        bfp_sub_vect_s16(&A, &B, &C);

        headroom_t exp_hr = HR_S16(casse->expected.value);

        TEST_ASSERT_EQUAL_MSG(casse->expected.exp, A.exp, "A.exp is wrong.", casse->line);
        TEST_ASSERT_EQUAL_MSG(exp_hr, A.hr, "A.hr is wrong.", casse->line);
        TEST_ASSERT_EQUAL_MSG(B.length, A.length, "A.length is wrong.", casse->line);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL_MSG(casse->expected.value, A.data[i], "A.data[] is wrong.", casse->line);
        }
    }
}




static void test_bfp_sub_vect_s16_1()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8788764;

    int16_t dataA[MAX_LEN];
    int16_t dataB[MAX_LEN];
    int16_t dataC[MAX_LEN];
    int16_t expA[MAX_LEN];
    bfp_s16_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s16(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s16(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s16(Bf, &B);
        test_double_from_s16(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] - Cf[i];
        }

        bfp_sub_vect_s16(&A, &B, &C);

        test_s16_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            // PRINTF("! %08X\t%08X\n", (unsigned) expA[i], (unsigned) A.data[i]);
            TEST_ASSERT_INT16_WITHIN(1, expA[i], A.data[i]);
        }
    }
}
static void test_bfp_sub_vect_s32_0()
{
    PRINTF("%s...\n", __func__);

    int32_t dataA[MAX_LEN];
    int32_t dataB[MAX_LEN];
    int32_t dataC[MAX_LEN];
    bfp_s32_t A, B, C;

    bfp_init_vect_s32(&A, dataA, 0, MAX_LEN, 0);
    bfp_init_vect_s32(&B, dataB, 0, MAX_LEN, 0);
    bfp_init_vect_s32(&C, dataC, 0, MAX_LEN, 0);

    typedef struct {

        struct {    int32_t value;  exponent_t exp;     } b;
        struct {    int32_t value;  exponent_t exp;     } c;
        struct {    int32_t value;  exponent_t exp;     } expected;

        unsigned line;
    } test_case_t;



    test_case_t casses[] = {
        //    b{     value,  exp },      c{     value,  exp },   expected{       value,  exp }
        {      { -0x010000,    0 },       {  0x010000,    0 },           { -0x40000000,  -13 },        __LINE__ },
        {      {  0x00FF00,    0 },       { -0x00FF00,    0 },           {  0x7F800000,  -14 },        __LINE__ },
        {      {  0x000100,    0 },       { -0x000000,    0 },           {  0x20000000,  -21 },        __LINE__ },
        {      {  0x000200,    0 },       { -0x000000,    0 },           {  0x20000000,  -20 },        __LINE__ },
        {      {  0x000000,    0 },       { -0x111100,    0 },           {  0x22220000,  -9  },        __LINE__ },
        {      {  0x000100,    0 },       { -0x000100,    0 },           {  0x40000000,  -21 },        __LINE__ },

        {      { -0x010000,    1 },       {  0x010000,    1 },           { -0x40000000,  -12 },        __LINE__ },
        {      {  0x00FF00,    1 },       { -0x00FF00,    1 },           {  0x7F800000,  -13 },        __LINE__ },
        {      {  0x000100,    1 },       { -0x000000,    1 },           {  0x20000000,  -20 },        __LINE__ },
        {      {  0x000200,    1 },       { -0x000000,    1 },           {  0x20000000,  -19 },        __LINE__ },
        {      {  0x000000,    1 },       { -0x111100,    1 },           {  0x22220000,  -8  },        __LINE__ },
        {      {  0x000100,    1 },       { -0x000100,    1 },           {  0x40000000,  -20 },        __LINE__ },
        
        {      {  0x000100,    1 },       { -0x000100,    0 },           {  0x30000000,  -20 },        __LINE__ },
        {      {  0x000100,    0 },       { -0x000100,    1 },           {  0x60000000,  -21 },        __LINE__ },
        {      {  0x000200,    0 },       { -0x000100,    0 },           {  0x30000000,  -20 },        __LINE__ },
        {      {  0x000200,    0 },       { -0x001000,   -4 },           {  0x30000000,  -20 },        __LINE__ },

    };

    const unsigned N_cases = sizeof(casses)/sizeof(test_case_t);

    const unsigned start_case = 0;

    for(int v = start_case; v < N_cases; v++){
        PRINTF("\ttest vector %d..\n", v);
        
        test_case_t* casse = &casses[v];

        bfp_set_vect_s32(&B, casse->b.value, casse->b.exp);
        bfp_set_vect_s32(&C, casse->c.value, casse->c.exp);

        bfp_sub_vect_s32(&A, &B, &C);

        headroom_t exp_hr = HR_S32(casse->expected.value);

        TEST_ASSERT_EQUAL_MSG(casse->expected.exp, A.exp, "A.exp is wrong.", casse->line);
        TEST_ASSERT_EQUAL_MSG(exp_hr, A.hr, "A.hr is wrong.", casse->line);
        TEST_ASSERT_EQUAL_MSG(B.length, A.length, "A.length is wrong.", casse->line);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_EQUAL_MSG(casse->expected.value, A.data[i], "A.data[] is wrong.", casse->line);
        }
    }
}




static void test_bfp_sub_vect_s32_1()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8788764;

    int32_t dataA[MAX_LEN];
    int32_t dataB[MAX_LEN];
    int32_t dataC[MAX_LEN];
    int32_t expA[MAX_LEN];
    bfp_s32_t A, B, C;

    A.data = dataA;
    B.data = dataB;
    C.data = dataC;

    double Af[MAX_LEN];
    double Bf[MAX_LEN];
    double Cf[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        test_random_bfp_s32(&B, MAX_LEN, &seed, &A, 0);
        test_random_bfp_s32(&C, MAX_LEN, &seed, &A, B.length);

        test_double_from_s32(Bf, &B);
        test_double_from_s32(Cf, &C);

        for(int i = 0; i < B.length; i++){
            Af[i] = Bf[i] - Cf[i];
        }

        bfp_sub_vect_s32(&A, &B, &C);

        test_s32_from_double(expA, Af, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            // PRINTF("! %08X\t%08X\n", (unsigned) expA[i], (unsigned) A.data[i]);
            TEST_ASSERT_INT32_WITHIN(1, expA[i], A.data[i]);
        }
    }
}






void test_bfp_sub_vect()
{
    SET_TEST_FILE();

    RUN_TEST(test_bfp_sub_vect_calc_params);
    
    RUN_TEST(test_bfp_sub_vect_s16_0);
    RUN_TEST(test_bfp_sub_vect_s16_1);

    RUN_TEST(test_bfp_sub_vect_s32_0);
    RUN_TEST(test_bfp_sub_vect_s32_1);
}