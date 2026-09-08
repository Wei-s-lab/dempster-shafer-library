#pragma once

#include <ds/mass_function.hpp>

#include <cstddef>
#include <stdexcept>

namespace ds {

// Degree of conflict between two bodies of evidence:
//   K = sum of m1(B) * m2(C) over all pairs with B intersect C == emptyset.
template <std::size_t FrameSize>
double conflict(const MassFunction<FrameSize>& m1, const MassFunction<FrameSize>& m2)
{
    double k = 0.0;
    for (const auto& [b, mb] : m1.masses())
        for (const auto& [c, mc] : m2.masses())
            if ((b & c).none())
                k += mb * mc;
    return k;
}

// Conjunctive (unnormalized) rule:
//   m(A) = sum of m1(B) * m2(C) over all pairs with B intersect C == A.
// The conflict is retained as mass on the empty set.
template <std::size_t FrameSize>
MassFunction<FrameSize> conjunctive_combination(const MassFunction<FrameSize>& m1,
                                                 const MassFunction<FrameSize>& m2)
{
    MassFunction<FrameSize> out;
    for (const auto& [b, mb] : m1.masses())
        for (const auto& [c, mc] : m2.masses())
            out.add(b & c, mb * mc);
    return out;
}

// Disjunctive rule:
//   m(A) = sum of m1(B) * m2(C) over all pairs with B union C == A.
template <std::size_t FrameSize>
MassFunction<FrameSize> disjunctive_combination(const MassFunction<FrameSize>& m1,
                                                 const MassFunction<FrameSize>& m2)
{
    MassFunction<FrameSize> out;
    for (const auto& [b, mb] : m1.masses())
        for (const auto& [c, mc] : m2.masses())
            out.add(b | c, mb * mc);
    return out;
}

// Dempster's rule of combination (the normalized orthogonal sum):
// the conjunctive rule followed by normalization that removes the conflict.
// Throws std::domain_error when the two sources are in total conflict (K == 1),
// where the combination is undefined.
template <std::size_t FrameSize>
MassFunction<FrameSize> dempster_combination(const MassFunction<FrameSize>& m1,
                                             const MassFunction<FrameSize>& m2)
{
    const MassFunction<FrameSize> conjunctive = conjunctive_combination(m1, m2);
    const double k = conjunctive.conflict_mass();
    const double denominator = 1.0 - k;
    if (denominator <= 0.0)
        throw std::domain_error(
            "ds::dempster_combination: total conflict (K == 1), evidence cannot be combined");

    MassFunction<FrameSize> out;
    for (const auto& [focal, value] : conjunctive.masses())
    {
        if (focal.none()) // drop the empty set
            continue;
        out.set(focal, value / denominator);
    }
    return out;
}

// Yager's rule: like the conjunctive rule, but instead of normalizing away the
// conflict, the conflict mass is reassigned to the whole frame (uncertainty).
template <std::size_t FrameSize>
MassFunction<FrameSize> yager_combination(const MassFunction<FrameSize>& m1,
                                          const MassFunction<FrameSize>& m2)
{
    const MassFunction<FrameSize> conjunctive = conjunctive_combination(m1, m2);
    const double k = conjunctive.conflict_mass();

    MassFunction<FrameSize> out;
    for (const auto& [focal, value] : conjunctive.masses())
    {
        if (focal.none()) // remove the empty set
            continue;
        out.set(focal, value);
    }

    FocalSet<FrameSize> theta;
    theta.set(); // the whole frame of discernment
    out.add(theta, k);
    return out;
}

// Convenience alias for dempster_combination (orthogonal sum ⊕).
// Prefer the named function in new code when clarity matters.
template <std::size_t FrameSize>
MassFunction<FrameSize> operator+(const MassFunction<FrameSize>& a,
                                  const MassFunction<FrameSize>& b)
{
    return dempster_combination(a, b);
}

} // namespace ds
