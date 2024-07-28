#include "intrinsics.hpp"

namespace Archa {

#ifdef USING_SIMD_SSE2
__m128i SSE2::minus_one_ints{_mm_set1_epi32(-1)};

__m128i SSE2::load_ints(const int *src) {
  return _mm_load_si128(reinterpret_cast<const __m128i *>(src));
}

__m128 SSE2::load_floats(const float *src) { return _mm_load_ps(src); }

__m128i SSE2::set_zero_int() { return _mm_setzero_si128(); }
__m128 SSE2::set_zero_float() { return _mm_setzero_ps(); }

__m128i SSE2::set_int(int a) { return _mm_set1_epi32(a); }
__m128 SSE2::set_float(float a) { return _mm_set1_ps(a); }

__m128i SSE2::set_ints(int a, int b, int c, int d) {
  return _mm_set_epi32(a, b, c, d);
}

__m128 SSE2::set_floats(float a, float b, float c, float d) {
  return _mm_set_ps(a, b, c, d);
}

__m128i SSE2::or_ints(const std::array<__m128i, 3> &vecs) {
  return _mm_or_si128(_mm_or_si128(vecs[0], vecs[1]), vecs[2]);
}

__m128 SSE2::divide_floats(const __m128 &a, const __m128 &b) {
  return _mm_div_ps(a, b);
}

__m128i SSE2::multiply_ints(const __m128i &a, const __m128i &b) {
  return _mm_mullo_epi32(a, b);
}

__m128 SSE2::multiply_floats(const __m128 &a, const __m128 &b) {
  return _mm_mul_ps(a, b);
}

__m128i SSE2::add_ints(const __m128i &a, const __m128i &b) {
  return _mm_add_epi32(a, b);
}

__m128 SSE2::add_floats(const __m128 &a, const __m128 &b) {
  return _mm_add_ps(a, b);
}

__m128 SSE2::add_floats(const std::array<__m128, 3> &vecs) {
  return _mm_add_ps(_mm_add_ps(vecs[0], vecs[1]), vecs[2]);
}

__m128i SSE2::subtract_ints(const __m128i &a, const __m128i &b) {
  return _mm_sub_epi32(a, b);
}

__m128 SSE2::subtract_floats(const __m128 &a, const __m128 &b) {
  return _mm_sub_ps(a, b);
}

__m128i SSE2::horizontal_add_ints(const __m128i &vec) {
  return _mm_hadd_epi32(vec, vec);
}

__m128 SSE2::horizontal_add_floats(const __m128 &vec) {
  return _mm_hadd_ps(vec, vec);
}

float SSE2::sum_floats(__m128 vec) {
  vec = horizontal_add_floats(vec);
  vec = horizontal_add_floats(vec);

  return _mm_cvtss_f32(vec);
}

__m128i SSE2::convert_to_ints(const __m128 &vec) {
  return _mm_cvtps_epi32(vec);
}

__m128 SSE2::convert_to_floats(const __m128i &vec) {
  return _mm_cvtepi32_ps(vec);
}

__m128i SSE2::compare_ints_gt(const __m128i &a, const __m128i &b) {
  return _mm_cmpgt_epi32(a, b);
}

int SSE2::pack_as_int(__m128i vec) {
  vec = _mm_packus_epi32(vec, vec);
  vec = _mm_packus_epi16(vec, vec);

  return extract_int<0>(vec);
}

int SSE2::move_mask_int8(const __m128i &vec) { return _mm_movemask_epi8(vec); }

void SSE2::store_ints(int *dest, const __m128i &src) {
  _mm_store_si128(reinterpret_cast<__m128i *>(dest), src);
}

void SSE2::store_floats(float *dest, const __m128 &src) {
  _mm_store_ps(dest, src);
}
#endif

#ifdef USING_SIMD_AVX2
__m256i AVX2::minus_one_ints{_mm256_set1_epi32(-1)};

__m256i AVX2::load_ints(const int *src) {
  return _mm256_load_si256(reinterpret_cast<const __m256i *>(src));
}

__m256 AVX2::load_floats(const float *src) { return _mm256_load_ps(src); }

__m256i AVX2::set_zero_int() { return _mm256_setzero_si256(); }
__m256 AVX2::set_zero_float() { return _mm256_setzero_ps(); }

__m256i AVX2::set_int(int a) { return _mm256_set1_epi32(a); }
__m256 AVX2::set_float(float a) { return _mm256_set1_ps(a); }

__m256i AVX2::set_ints(int a, int b, int c, int d, int e, int f, int g, int h) {
  return _mm256_set_epi32(a, b, c, d, e, f, g, h);
}

__m256 AVX2::set_floats(float a, float b, float c, float d, float e, float f,
                        float g, float h) {
  return _mm256_set_ps(a, b, c, d, e, f, g, h);
}

__m256i AVX2::or_ints(const std::array<__m256i, 3> &vecs) {
  return _mm256_or_si256(_mm256_or_si256(vecs[0], vecs[1]), vecs[2]);
}

__m256 AVX2::divide_floats(const __m256 &a, const __m256 &b) {
  return _mm256_div_ps(a, b);
}

__m256i AVX2::multiply_ints(const __m256i &a, const __m256i &b) {
  return _mm256_mullo_epi32(a, b);
}

__m256 AVX2::multiply_floats(const __m256 &a, const __m256 &b) {
  return _mm256_mul_ps(a, b);
}

__m256i AVX2::add_ints(const __m256i &a, const __m256i &b) {
  return _mm256_add_epi32(a, b);
}

__m256 AVX2::add_floats(const __m256 &a, const __m256 &b) {
  return _mm256_add_ps(a, b);
}

__m256 AVX2::add_floats(const std::array<__m256, 3> &vecs) {
  return _mm256_add_ps(_mm256_add_ps(vecs[0], vecs[1]), vecs[2]);
}

__m256i AVX2::subtract_ints(const __m256i &a, const __m256i &b) {
  return _mm256_sub_epi32(a, b);
}

__m256 AVX2::subtract_floats(const __m256 &a, const __m256 &b) {
  return _mm256_sub_ps(a, b);
}

__m256i AVX2::horizontal_add_ints(const __m256i &vec) {
  return _mm256_hadd_epi32(vec, vec);
}

__m256 AVX2::horizontal_add_floats(const __m256 &vec) {
  return _mm256_hadd_ps(vec, vec);
}

__m256i AVX2::convert_to_ints(const __m256 &vec) {
  return _mm256_cvtps_epi32(vec);
}

__m256 AVX2::convert_to_floats(const __m256i &vec) {
  return _mm256_cvtepi32_ps(vec);
}

__m256i AVX2::compare_ints_gt(const __m256i &a, const __m256i &b) {
  return _mm256_cmpgt_epi32(a, b);
}

int AVX2::move_mask_int8(const __m256i &vec) {
  return _mm256_movemask_epi8(vec);
}

void AVX2::store_ints(int *dest, const __m256i &src) {
  _mm256_store_si256(reinterpret_cast<__m256i *>(dest), src);
}

void AVX2::store_floats(float *dest, const __m256 &src) {
  _mm256_store_ps(dest, src);
}
#endif

} // namespace Archa
