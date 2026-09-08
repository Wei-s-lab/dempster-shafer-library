#pragma once

#include <ds/frame_of_discernment.hpp>

#include <cmath>
#include <cstddef>
#include <unordered_map>

namespace ds {

// A basic probability assignment (BPA), also called a mass function.
//
// It maps focal sets (subsets of the frame of discernment, represented as
// FocalSet<FrameSize>) to a mass in [0, 1]. Only focal sets with non-zero
// mass are stored. A valid BPA satisfies m(emptyset) == 0 and the masses of
// all focal sets sum to 1.
template <std::size_t FrameSize>
class MassFunction {
public:
    using Set = FocalSet<FrameSize>;
    using Container = std::unordered_map<Set, double>;

    MassFunction() = default;

    // Add mass to a focal set, accumulating with any mass already there.
    void add(const Set& focal, double mass)
    {
        if (mass == 0.0)
            return;
        m_[focal] += mass;
    }

    // Overwrite the mass of a focal set (removing it when set to zero).
    void set(const Set& focal, double mass)
    {
        if (mass == 0.0)
        {
            m_.erase(focal);
            return;
        }
        m_[focal] = mass;
    }

    // Mass assigned to a focal set (0 when it carries no mass).
    double mass(const Set& focal) const
    {
        const auto it = m_.find(focal);
        return it == m_.end() ? 0.0 : it->second;
    }

    const Container& masses() const { return m_; }
    bool empty() const { return m_.empty(); }
    std::size_t size() const { return m_.size(); }

    // Mass on the empty set; for combined evidence this is the conflict mass.
    double conflict_mass() const { return mass(Set{}); }

    double total_mass() const
    {
        double sum = 0.0;
        for (const auto& [focal, value] : m_)
            sum += value;
        return sum;
    }

    // A valid BPA has no negative masses, no mass on the empty set, and a
    // total mass of 1 (within the given tolerance).
    bool is_valid(double tolerance = 1e-9) const
    {
        if (conflict_mass() > tolerance)
            return false;

        double sum = 0.0;
        for (const auto& [focal, value] : m_)
        {
            if (value < -tolerance)
                return false;
            sum += value;
        }
        return std::abs(sum - 1.0) <= tolerance;
    }

    // Scale every mass so the total becomes 1 (no-op when the total is 0).
    void normalize()
    {
        const double sum = total_mass();
        if (sum == 0.0)
            return;
        for (auto& [focal, value] : m_)
            value /= sum;
    }

    // Bel(A) = sum of m(B) over non-empty focal sets B with B subset of A.
    double belief(const Set& a) const
    {
        double sum = 0.0;
        for (const auto& [b, value] : m_)
        {
            if (b.none())
                continue;
            if ((b & a) == b) // b is a subset of a
                sum += value;
        }
        return sum;
    }

    // Pl(A) = sum of m(B) over focal sets B whose intersection with A is non-empty.
    double plausibility(const Set& a) const
    {
        double sum = 0.0;
        for (const auto& [b, value] : m_)
        {
            if ((b & a).any())
                sum += value;
        }
        return sum;
    }

    // Q(A) = sum of m(B) over focal sets B that are supersets of A.
    double commonality(const Set& a) const
    {
        double sum = 0.0;
        for (const auto& [b, value] : m_)
        {
            if ((a & b) == a) // a is a subset of b
                sum += value;
        }
        return sum;
    }

private:
    Container m_;
};

} // namespace ds
