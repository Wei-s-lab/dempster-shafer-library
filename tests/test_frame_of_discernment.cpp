#include <ds/frame_of_discernment.hpp>

#include <cassert>
#include <iostream>

// Dependency-free assertion-based tests so the environment can validate the
// library end-to-end without fetching any external test framework.
int main()
{
    using Focal4 = ds::FocalSet<4>;

    // A default-constructed focal set is the empty set.
    const Focal4 empty;
    assert(ds::is_empty(empty));
    assert(empty.count() == 0);

    // A non-empty focal set must not be reported as empty.
    const Focal4 singleton{0b0001};
    assert(!ds::is_empty(singleton));
    assert(singleton.count() == 1);

    // Set-theoretic operations behave as expected on focal sets.
    const Focal4 a{0b0011};
    const Focal4 b{0b0110};
    assert((a | b) == Focal4{0b0111});
    assert((a & b) == Focal4{0b0010});
    assert(ds::is_empty(Focal4{0b0001} & Focal4{0b0010}));

    // is_empty works across different frame sizes (template instantiation).
    assert(ds::is_empty(ds::FocalSet<1>{}));
    assert(!ds::is_empty(ds::FocalSet<8>{0b1000'0000}));

    std::cout << "All Dempster-Shafer frame_of_discernment tests passed.\n";
    return 0;
}
