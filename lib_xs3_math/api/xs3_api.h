// Copyright 2021 XMOS LIMITED. This Software is subject to the terms of the 
// XMOS Public License: Version 1

#pragma once

#if defined(__cplusplus)
#  define EXTERN_C extern "C"
#else
#  define EXTERN_C 
#endif //__cplusplus

/// Used to indicate a function uses the C function ABI
#define C_API EXTERN_C
/// Used to indicate a struct is a C type
#define C_TYPE EXTERN_C

/// Many API functions require word or double word alignment
#ifdef __xcore__
#  define ALIGNMENT(N)  __attribute__((aligned (N)))
#else
#  define ALIGNMENT(N)
#endif

/// (on xcore) Force variable to word alignment
#define WORD_ALIGNED   ALIGNMENT(4)

/// (on xcore) Force variable to double word alignment
#define DWORD_ALIGNED  ALIGNMENT(8)

//