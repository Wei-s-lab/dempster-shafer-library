#include "test_utils.hpp"

#include <ds/mass_function.hpp>

#include <stdexcept>

namespace {

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

void test_set_add_and_query()
{
    Mass3 m;
    DS_CHECK(m.empty());
    DS_CHECK(m.size() == 0);
    DS_CHECK_APPROX(m.mass(kA), 0.0);

    m.set(kA, 0.4);
    m.set(kAB, 0.3);
    m.set(kABC, 0.3);

    DS_CHECK(!m.empty());
    DS_CHECK(m.size() == 3);
    DS_CHECK_APPROX(m.mass(kA), 0.4);
    DS_CHECK_APPROX(m.mass(kB), 0.0);
    DS_CHECK_APPROX(m.total_mass(), 1.0);
    DS_CHECK(m.is_valid());

    m.add(kA, 0.1);
    DS_CHECK_APPROX(m.mass(kA), 0.5);
    DS_CHECK(!m.is_valid()); // total mass is now 1.1

    m.set(kA, 0.0);
    DS_CHECK_APPROX(m.mass(kA), 0.0);
    DS_CHECK(m.size() == 2);
}

void test_belief_plausibility_commonality()
{
    // m({a}) = 0.4, m({a,b}) = 0.3, m(Theta) = 0.3
    Mass3 m;
    m.set(kA, 0.4);
    m.set(kAB, 0.3);
    m.set(kABC, 0.3);
    DS_CHECK(m.is_valid());

    DS_CHECK_APPROX(m.belief(kA), 0.4);
    DS_CHECK_APPROX(m.belief(kAB), 0.7);
    DS_CHECK_APPROX(m.belief(kABC), 1.0);
    DS_CHECK_APPROX(m.belief(kB), 0.0);
    DS_CHECK_APPROX(m.belief(kC), 0.0);

    DS_CHECK_APPROX(m.plausibility(kA), 1.0);
    DS_CHECK_APPROX(m.plausibility(kB), 0.6);
    DS_CHECK_APPROX(m.plausibility(kC), 0.3);
    DS_CHECK_APPROX(m.plausibility(kBC), 0.6);

    DS_CHECK_APPROX(m.commonality(kA), 1.0);
    DS_CHECK_APPROX(m.commonality(kAB), 0.6);
    DS_CHECK_APPROX(m.commonality(kABC), 0.3);
    DS_CHECK_APPROX(m.commonality(kC), 0.3);
    DS_CHECK_APPROX(m.commonality(kAC), 0.3);
}

void test_normalize()
{
    // Per-focal masses stay in [0, 1]; total > 1 is what normalize() fixes.
    Mass3 m;
    m.set(kA, 1.0);
    m.set(kB, 1.0);
    DS_CHECK_APPROX(m.total_mass(), 2.0);
    DS_CHECK(!m.is_valid());

    m.normalize();
    DS_CHECK(m.is_valid());
    DS_CHECK_APPROX(m.mass(kA), 0.5);
    DS_CHECK_APPROX(m.mass(kB), 0.5);
    DS_CHECK_APPROX(m.conflict_mass(), 0.0);
}

void test_conflict_mass_invalidates()
{
    Mass3 m;
    m.set(kEmpty, 0.2);
    m.set(kA, 0.8);
    DS_CHECK_APPROX(m.conflict_mass(), 0.2);
    DS_CHECK(!m.is_valid());
}

void test_rejects_out_of_range_mass()
{
    Mass3 m;

    bool threw_negative = false;
    try
    {
        m.set(kA, -0.1);
    }
    catch (const std::invalid_argument&)
    {
        threw_negative = true;
    }
    DS_CHECK(threw_negative);

    bool threw_above_one = false;
    try
    {
        m.set(kA, 1.1);
    }
    catch (const std::invalid_argument&)
    {
        threw_above_one = true;
    }
    DS_CHECK(threw_above_one);

    bool threw_nan = false;
    try
    {
        m.add(kA, std::nan(""));
    }
    catch (const std::invalid_argument&)
    {
        threw_nan = true;
    }
    DS_CHECK(threw_nan);

    m.set(kA, 0.6);
    bool threw_add_overflow = false;
    try
    {
        m.add(kA, 0.5); // 0.6 + 0.5 > 1
    }
    catch (const std::invalid_argument&)
    {
        threw_add_overflow = true;
    }
    DS_CHECK(threw_add_overflow);
    DS_CHECK_APPROX(m.mass(kA), 0.6); // unchanged after failed add
}

int main()
{
    test_set_add_and_query();
    test_belief_plausibility_commonality();
    test_normalize();
    test_conflict_mass_invalidates();
    test_rejects_out_of_range_mass();
    return ds_test::summary_and_exit("mass_function");
}
