#pragma once

#define GLM_FORCE_AVX

#ifndef FORCE_INLINE

#ifndef NO_INLINE
#define FORCE_INLINE [[clang::always_inline]]
#else
#define FORCE_INLINE
#endif

#endif

// #define NO_SIMD
// #define NO_SIMD_AVX2

#ifdef NO_SIMD
#define NO_SIMD_SSE2
#define NO_SIMD_AVX2
#endif

#ifndef NO_SIMD_SSE2
#ifdef __SSE2__
#define USING_SIMD_SSE2
#else
#define NO_SIMD_SSE2
#define NO_SIMD_AVX2
#define NO_SIMD
#endif
#endif

#if !defined(NO_SIMD_SSE2) && !defined(NO_SIMD_AVX2)
#ifdef __AVX2__
#define USING_SIMD_AVX2
#else
#define NO_SIMD_AVX2
#endif
#endif

#define ALIGN_SSE2_WIDTH 16
#define ALIGN_AVX2_WIDTH 32

#define ALIGN_SSE2 alignas(ALIGN_SSE2_WIDTH)
#define ALIGN_AVX2 alignas(ALIGN_AVX2_WIDTH)

#ifdef USING_SIMD_AVX2
#define SIMD_ALIGN_WIDTH ALIGN_AVX2_WIDTH
#define ALIGN_SIMD ALIGN_AVX2
#elif defined USING_SIMD_SSE2
#define SIMD_ALIGN_WIDTH ALIGN_SSE2_WIDTH
#define ALIGN_SIMD ALIGN_SSE2
#else
#define SIMD_ALIGN_WIDTH 1
#define ALIGN_SIMD
#endif
