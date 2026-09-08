#include <ds/ds.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

namespace {

constexpr double kTol = 1e-9;

bool approx(double a, double b) { return std::abs(a - b) <= kTol; }

// Frame of discernment {a, b} for the two-hypothesis examples.
using Frame2 = ds::MassFunction<2>;
using Set2 = ds::FocalSet<2>;

const Set2 kA{0b01}; // {a}
const Set2 kB{0b10}; // {b}
const Set2 kAB{0b11}; // {a, b} = Theta

} // namespace

// Two sources that mostly agree on {a}; there is no conflict, so Dempster's
// rule equals the conjunctive rule. Verified by hand:
//   conj: {a} = 0.6*0.7 + 0.6*0.3 + 0.4*0.7 = 0.88, Theta = 0.12, empty = 0.
void test_agreeing_sources()
{
    Frame2 m1;
    m1.set(kA, 0.6);
    m1.set(kAB, 0.4);

    Frame2 m2;
    m2.set(kA, 0.7);
    m2.set(kAB, 0.3);

    assert(m1.is_valid());
    assert(m2.is_valid());
    assert(approx(ds::conflict(m1, m2), 0.0));

    const Frame2 fused = ds::dempster_combination(m1, m2);
    assert(fused.is_valid());
    assert(approx(fused.mass(kA), 0.88));
    assert(approx(fused.mass(kAB), 0.12));

    assert(approx(fused.belief(kA), 0.88));
    assert(approx(fused.plausibility(kA), 1.0));
    assert(approx(fused.commonality(kAB), 0.12));

    // The operator* form is Dempster's rule.
    const Frame2 via_operator = m1 * m2;
    assert(approx(via_operator.mass(kA), 0.88));
}

// Two strongly conflicting sources. Verified by hand:
//   conj: empty = 0.9*0.9 + 0.1*0.1 = 0.82 (= K), {a} = 0.09, {b} = 0.09.
//   Dempster normalizes by (1 - K) = 0.18 -> {a} = {b} = 0.5.
void test_conflicting_sources()
{
    Frame2 m1;
    m1.set(kA, 0.9);
    m1.set(kB, 0.1);

    Frame2 m2;
    m2.set(kA, 0.1);
    m2.set(kB, 0.9);

    assert(approx(ds::conflict(m1, m2), 0.82));

    const Frame2 conj = ds::conjunctive_combination(m1, m2);
    assert(approx(conj.conflict_mass(), 0.82));
    assert(approx(conj.mass(kA), 0.09));
    assert(approx(conj.mass(kB), 0.09));

    const Frame2 dempster = ds::dempster_combination(m1, m2);
    assert(dempster.is_valid());
    assert(approx(dempster.mass(kA), 0.5));
    assert(approx(dempster.mass(kB), 0.5));

    // Yager keeps the conflict as uncertainty on the whole frame.
    const Frame2 yager = ds::yager_combination(m1, m2);
    assert(yager.is_valid());
    assert(approx(yager.mass(kA), 0.09));
    assert(approx(yager.mass(kB), 0.09));
    assert(approx(yager.mass(kAB), 0.82));
}

// Total conflict: the two categorical, incompatible sources cannot be fused
// by Dempster's rule.
void test_total_conflict_throws()
{
    Frame2 m1;
    m1.set(kA, 1.0);

    Frame2 m2;
    m2.set(kB, 1.0);

    assert(approx(ds::conflict(m1, m2), 1.0));

    bool threw = false;
    try
    {
        ds::dempster_combination(m1, m2);
    }
    catch (const std::domain_error&)
    {
        threw = true;
    }
    assert(threw);

    // The disjunctive rule still works: {a} OR {b} -> {a, b}.
    const Frame2 disj = ds::disjunctive_combination(m1, m2);
    assert(approx(disj.mass(kAB), 1.0));
}

int main()
{
    test_agreeing_sources();
    test_conflicting_sources();
    test_total_conflict_throws();
    std::cout << "All Dempster-Shafer combination_rules tests passed.\n";
    return 0;
}
