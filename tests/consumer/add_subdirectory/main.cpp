#include <ds/ds.hpp>

#include <cmath>
#include <cstdlib>
#include <iostream>

// Minimal consumer: prove an external target can use the INTERFACE library.
int main()
{
    constexpr std::size_t N = 2;
    using Set = ds::FocalSet<N>;

    const Set a{0b01};
    const Set theta{0b11};

    ds::MassFunction<N> s1;
    s1.set(a, 0.6);
    s1.set(theta, 0.4);

    ds::MassFunction<N> s2;
    s2.set(a, 0.7);
    s2.set(theta, 0.3);

    const auto fused = ds::dempster_combination(s1, s2);
    if (!fused.is_valid())
        return EXIT_FAILURE;
    if (std::abs(fused.mass(a) - 0.88) > 1e-9)
        return EXIT_FAILURE;

    std::cout << "consumer ok: m({a}) = " << fused.mass(a) << "\n";
    return EXIT_SUCCESS;
}
