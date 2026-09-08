#include <ds/mass_function.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace {

constexpr double kTol = 1e-9;

bool approx(double a, double b) { return std::abs(a - b) <= kTol; }

// Frame of discernment {a, b, c}.
using Mass3 = ds::MassFunction<3>;
using Set3 = ds::FocalSet<3>;

const Set3 kEmpty{};
const Set3 kA{0b001};
const Set3 kB{0b010};
const Set3 kC{0b100};
const Set3 kAB{0b011};
const Set3 kAC{0b101};
const Set3 kBC{0b110};
const Set3 kABC{0b111};

} // namespace

// set / add / mass bookkeeping for a sparse BPA.
void test_set_add_and_query()
{
    Mass3 m;
    assert(m.empty());
    assert(m.size() == 0);
    assert(approx(m.mass(kA), 0.0));

    m.set(kA, 0.4);
    m.set(kAB, 0.3);
    m.set(kABC, 0.3);

    assert(!m.empty());
    assert(m.size() == 3);
    assert(approx(m.mass(kA), 0.4));
    assert(approx(m.mass(kB), 0.0));
    assert(approx(m.total_mass(), 1.0));
    assert(m.is_valid());

    // add accumulates; set(0) removes the focal set.
    m.add(kA, 0.1);
    assert(approx(m.mass(kA), 0.5));
    assert(!m.is_valid()); // total mass is now 1.1

    m.set(kA, 0.0);
    assert(approx(m.mass(kA), 0.0));
    assert(m.size() == 2);
}

// Classic hand-checked Bel / Pl / Q on a simple BPA.
//   m({a}) = 0.4, m({a,b}) = 0.3, m(Theta) = 0.3
void test_belief_plausibility_commonality()
{
    Mass3 m;
    m.set(kA, 0.4);
    m.set(kAB, 0.3);
    m.set(kABC, 0.3);
    assert(m.is_valid());

    // Bel(A) = sum of m(B) for non-empty Bsubseteq A.
    assert(approx(m.belief(kA), 0.4));          // only {a}
    assert(approx(m.belief(kAB), 0.7));         // {a} + {a,b}
    assert(approx(m.belief(kABC), 1.0));        // all mass
    assert(approx(m.belief(kB), 0.0));
    assert(approx(m.belief(kC), 0.0));

    // Pl(A) = 1 - Bel(complement A); equivalently mass of sets that hit A.
    assert(approx(m.plausibility(kA), 1.0));    // every focal set meets {a}
    assert(approx(m.plausibility(kB), 0.6));    // {a,b} + Theta
    assert(approx(m.plausibility(kC), 0.3));    // only Theta
    assert(approx(m.plausibility(kBC), 0.6));

    // Q(A) = sum of m(B) for B supseteq A.
    assert(approx(m.commonality(kA), 1.0));     // all three focal sets
    assert(approx(m.commonality(kAB), 0.6));    // {a,b} + Theta
    assert(approx(m.commonality(kABC), 0.3));
    assert(approx(m.commonality(kC), 0.3));
    assert(approx(m.commonality(kAC), 0.3));
}

// normalize() turns an unnormalized assignment into a valid BPA.
void test_normalize()
{
    Mass3 m;
    m.set(kA, 2.0);
    m.set(kB, 3.0);
    assert(approx(m.total_mass(), 5.0));
    assert(!m.is_valid());

    m.normalize();
    assert(m.is_valid());
    assert(approx(m.mass(kA), 0.4));
    assert(approx(m.mass(kB), 0.6));
    assert(approx(m.conflict_mass(), 0.0));
}

// Mass on the empty set makes a BPA invalid (conflict retained).
void test_conflict_mass_invalidates()
{
    Mass3 m;
    m.set(kEmpty, 0.2);
    m.set(kA, 0.8);
    assert(approx(m.conflict_mass(), 0.2));
    assert(!m.is_valid());
}

int main()
{
    test_set_add_and_query();
    test_belief_plausibility_commonality();
    test_normalize();
    test_conflict_mass_invalidates();
    std::cout << "All Dempster-Shafer mass_function tests passed.\n";
    return 0;
}
