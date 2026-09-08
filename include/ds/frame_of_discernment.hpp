#pragma once

#include <bitset>

namespace ds {
    template <std::size_t FrameSize>
    // @brief FocalSet is a subset of the discernment framework
    using FocalSet = std::bitset<FrameSize>;

    
    template <std::size_t FrameSize>
    inline bool is_empty(const FocalSet<FrameSize>& s)
    {
        return s.none();
    }
} // namespace ds