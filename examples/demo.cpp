#include <ds/ds.hpp>

#include <iostream>

// Small end-to-end demonstration of the Dempster-Shafer building blocks.
//
// A frame of discernment is the exhaustive set of mutually exclusive
// hypotheses under consideration. Here we model a 3-element frame
// {a, b, c}; every focal set is a subset represented as a bitset.
int main()
{
    constexpr std::size_t frame_size = 3;
    using Focal = ds::FocalSet<frame_size>;

    const Focal empty;                 // {}
    const Focal singleton_a{0b001};    // {a}
    const Focal pair_bc{0b110};        // {b, c}
    const Focal universe{0b111};       // {a, b, c}

    std::cout << "Frame of discernment size: " << frame_size << "\n\n";

    const std::pair<const char*, Focal> sets[] = {
        {"empty     ", empty},
        {"singleton ", singleton_a},
        {"pair      ", pair_bc},
        {"universe  ", universe},
    };

    for (const auto& [label, s] : sets)
    {
        std::cout << label << " = " << s
                  << "  cardinality=" << s.count()
                  << "  is_empty=" << std::boolalpha << ds::is_empty(s)
                  << "\n";
    }

    std::cout << "\nUnion(singleton, pair)     = " << (singleton_a | pair_bc) << "\n";
    std::cout << "Intersection(pair, universe) = " << (pair_bc & universe) << "\n";

    // Combine two bodies of evidence with Dempster's rule of combination.
    // Source 1 and source 2 both lean towards {a} but keep some uncertainty
    // on the whole frame; the fused belief in {a} should strengthen.
    std::cout << "\n--- Dempster's rule of combination ---\n";
    ds::MassFunction<frame_size> source1;
    source1.set(singleton_a, 0.6); // {a}
    source1.set(universe, 0.4);     // {a, b, c} (uncertainty)

    ds::MassFunction<frame_size> source2;
    source2.set(singleton_a, 0.7); // {a}
    source2.set(universe, 0.3);

    const auto fused = ds::dempster_combination(source1, source2);
    std::cout << "conflict K = " << ds::conflict(source1, source2) << "\n";
    for (const auto& [focal, mass] : fused.masses())
        std::cout << "  m(" << focal << ") = " << mass << "\n";
    std::cout << "Bel({a}) = " << fused.belief(singleton_a)
              << "  Pl({a}) = " << fused.plausibility(singleton_a) << "\n";

    return 0;
}
