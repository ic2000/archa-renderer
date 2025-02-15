#pragma once

#include "config.hpp"

#ifdef USING_SIMD_SSE2
#include <emmintrin.h>
#endif

#ifdef USING_SIMD_AVX2
#include <immintrin.h>
#endif

#include <array>

#include "constants.hpp"

namespace Archa {

class SSE2 {
#ifdef USING_SIMD_SSE2
public:
  static constexpr auto LANE_WIDTH{4};

  using IntVec = __m128i;
  using FloatVec = __m128;

  template <typename T> using Array = std::array<T, LANE_WIDTH>;

  template <int index> static int extract_int(const __m128i &vec) {
    return _mm_extract_epi32(vec, index);
  }

  static __m128i minus_one_ints;

  static __m128i load_ints(const int *src);
  static __m128 load_floats(const float *src);

  static __m128i set_zero_int();
  static __m128 set_zero_float();

  static __m128i set_int(int a);
  static __m128 set_float(float a);

  static __m128i set_ints(int a, int b, int c, int d);
  static __m128 set_floats(float a, float b, float c, float d);

  static __m128i or_ints(const std::array<__m128i, 3> &vecs);

  static __m128 divide_floats(const __m128 &a, const __m128 &b);

  static __m128i multiply_ints(const __m128i &a, const __m128i &b);
  static __m128 multiply_floats(const __m128 &a, const __m128 &b);

  static __m128i add_ints(const __m128i &a, const __m128i &b);
  static __m128 add_floats(const __m128 &a, const __m128 &b);
  static __m128 add_floats(const std::array<__m128, 3> &vecs);

  static __m128i subtract_ints(const __m128i &a, const __m128i &b);
  static __m128 subtract_floats(const __m128 &a, const __m128 &b);

  static __m128i horizontal_add_ints(const __m128i &vec);
  static __m128 horizontal_add_floats(const __m128 &vec);

  static float sum_floats(__m128 vec);

  static __m128i convert_to_ints(const __m128 &vec);
  static __m128 convert_to_floats(const __m128i &vec);

  static __m128i compare_ints_gt(const __m128i &a, const __m128i &b);

  static int pack_as_int(__m128i vec);
  static int move_mask_int8(const __m128i &vec);

  static void store_ints(int *dest, const __m128i &src);
  static void store_floats(float *dest, const __m128 &src);
#endif
};

class AVX2 {
#ifdef USING_SIMD_AVX2
public:
  static constexpr auto LANE_WIDTH{8};

  using IntVec = __m256i;
  using FloatVec = __m256;

  template <typename T> using Array = std::array<T, LANE_WIDTH>;

  static __m256i minus_one_ints;

  template <int index> static int extract_int(const __m256i &vec) {
    return _mm256_extract_epi32(vec, index);
  }

  template <int index> static __m128i extract_ints128(const __m256i &vec) {
    return _mm256_extractf128_si256(vec, index);
  }

  template <int index> static __m128 extract_floats128(const __m256 &vec) {
    return _mm256_extractf128_ps(vec, index);
  }

  static __m256i load_ints(const int *src);
  static __m256 load_floats(const float *src);

  static __m256i set_zero_int();
  static __m256 set_zero_float();

  static __m256i set_int(int a);
  static __m256 set_float(float a);

  static __m256i set_ints(int a, int b, int c, int d, int e, int f, int g,
                          int h);

  static __m256 set_floats(float a, float b, float c, float d, float e, float f,
                           float g, float h);

  static __m256i or_ints(const std::array<__m256i, 3> &vecs);

  static __m256 divide_floats(const __m256 &a, const __m256 &b);

  static __m256i multiply_ints(const __m256i &a, const __m256i &b);
  static __m256 multiply_floats(const __m256 &a, const __m256 &b);

  static __m256i add_ints(const __m256i &a, const __m256i &b);
  static __m256 add_floats(const __m256 &a, const __m256 &b);

  static __m256i subtract_ints(const __m256i &a, const __m256i &b);
  static __m256 subtract_floats(const __m256 &a, const __m256 &b);

  static __m256i horizontal_add_ints(const __m256i &vec);
  static __m256 horizontal_add_floats(const __m256 &vec);

  static __m256i convert_to_ints(const __m256 &vec);
  static __m256 convert_to_floats(const __m256i &vec);

  static __m256i compare_ints_gt(const __m256i &a, const __m256i &b);

  static int move_mask_int8(const __m256i &vec);

  static void store_ints(int *dest, const __m256i &src);
  static void store_floats(float *dest, const __m256 &src);

  static __m256 add_floats(const std::array<__m256, 3> &vecs);
#endif
};

} // namespace Archa
