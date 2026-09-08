#include "test_utils.hpp"

#include <ds/ds.hpp>

#include <stdexcept>

namespace {

using Frame2 = ds::MassFunction<2>;
using Set2 = ds::FocalSet<2>;

const Set2 kA{0b01};
const Set2 kB{0b10};
const Set2 kAB{0b11};

} // namespace

void test_agreeing_sources()
{
    Frame2 m1;
    m1.set(kA, 0.6);
    m1.set(kAB, 0.4);

    Frame2 m2;
    m2.set(kA, 0.7);
    m2.set(kAB, 0.3);

    DS_CHECK(m1.is_valid());
    DS_CHECK(m2.is_valid());
    DS_CHECK_APPROX(ds::conflict(m1, m2), 0.0);

    const Frame2 fused = ds::dempster_combination(m1, m2);
    DS_CHECK(fused.is_valid());
    DS_CHECK_APPROX(fused.mass(kA), 0.88);
    DS_CHECK_APPROX(fused.mass(kAB), 0.12);

    DS_CHECK_APPROX(fused.belief(kA), 0.88);
    DS_CHECK_APPROX(fused.plausibility(kA), 1.0);
    DS_CHECK_APPROX(fused.commonality(kAB), 0.12);

    const Frame2 via_operator = m1 + m2;
    DS_CHECK_APPROX(via_operator.mass(kA), 0.88);
}

void test_conflicting_sources()
{
    Frame2 m1;
    m1.set(kA, 0.9);
    m1.set(kB, 0.1);

    Frame2 m2;
    m2.set(kA, 0.1);
    m2.set(kB, 0.9);

    DS_CHECK_APPROX(ds::conflict(m1, m2), 0.82);

    const Frame2 conj = ds::conjunctive_combination(m1, m2);
    DS_CHECK_APPROX(conj.conflict_mass(), 0.82);
    DS_CHECK_APPROX(conj.mass(kA), 0.09);
    DS_CHECK_APPROX(conj.mass(kB), 0.09);

    const Frame2 dempster = ds::dempster_combination(m1, m2);
    DS_CHECK(dempster.is_valid());
    DS_CHECK_APPROX(dempster.mass(kA), 0.5);
    DS_CHECK_APPROX(dempster.mass(kB), 0.5);

    const Frame2 yager = ds::yager_combination(m1, m2);
    DS_CHECK(yager.is_valid());
    DS_CHECK_APPROX(yager.mass(kA), 0.09);
    DS_CHECK_APPROX(yager.mass(kB), 0.09);
    DS_CHECK_APPROX(yager.mass(kAB), 0.82);
}

void test_total_conflict_throws()
{
    Frame2 m1;
    m1.set(kA, 1.0);

    Frame2 m2;
    m2.set(kB, 1.0);

    DS_CHECK_APPROX(ds::conflict(m1, m2), 1.0);

    bool threw = false;
    try
    {
        ds::dempster_combination(m1, m2);
    }
    catch (const std::domain_error&)
    {
        threw = true;
    }
    DS_CHECK(threw);

    const Frame2 disj = ds::disjunctive_combination(m1, m2);
    DS_CHECK_APPROX(disj.mass(kAB), 1.0);
}

int main()
{
    test_agreeing_sources();
    test_conflicting_sources();
    test_total_conflict_throws();
    return ds_test::summary_and_exit("combination_rules");
}
