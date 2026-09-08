#pragma once

#include <bitset>
#include <cstddef>

namespace ds {

// A focal set is a subset of the frame of discernment Theta.
// Bit i set means hypothesis i is a member (LSB = hypothesis 0).
template <std::size_t FrameSize>
using FocalSet = std::bitset<FrameSize>;

template <std::size_t FrameSize>
inline bool is_empty(const FocalSet<FrameSize>& s)
{
    return s.none();
}

} // namespace ds
