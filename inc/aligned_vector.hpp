#pragma once

#include "aligned_allocator.hpp"
#include "config.hpp"

namespace Archa {

template <typename T, std::size_t ALIGNMENT_IN_BYTES = 64>
using AlignedVector = std::vector<T, AlignedAllocator<T, ALIGNMENT_IN_BYTES>>;

} // namespace Archa
