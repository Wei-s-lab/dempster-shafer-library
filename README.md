# Dempster-Shafer library

Header-only C++17 implementation of [Dempster–Shafer](https://de.wikipedia.org/wiki/Evidenztheorie)
belief functions (A. P. Dempster [[1]](#references), G. Shafer [[2]](#references)).
It generalizes Bayesian probability by assigning mass to *sets* of hypotheses
and representing uncertainty explicitly; Bayesian probability is the special case
where all mass lies on singletons. Dempster also co-invented the EM algorithm.

## Theory (core formulas)

Frame of discernment $\Theta$; basic probability assignment (BPA)
$m: 2^{\Theta} \to [0,1]$ with $m(\emptyset)=0$ and $\sum_{A\subseteq\Theta} m(A)=1$.

$$
\mathrm{Bel}(A)=\sum_{B\subseteq A} m(B),\qquad
\mathrm{Pl}(A)=\sum_{B\cap A\neq\emptyset} m(B),\qquad
Q(A)=\sum_{B\supseteq A} m(B).
$$

Conflict and Dempster's rule for independent BPAs $m_1,m_2$:

$$
K=\sum_{B\cap C=\emptyset} m_1(B)\,m_2(C),\qquad
(m_1\oplus m_2)(A)=\frac{1}{1-K}\sum_{B\cap C=A} m_1(B)\,m_2(C)
\quad(A\neq\emptyset).
$$

Also provided: unnormalized conjunctive / disjunctive combination, and
Yager's rule (assigns $K$ to $\Theta$ instead of normalizing).

## API

| Piece | Type / headers |
|-------|----------------|
| Focal sets | `ds::FocalSet<N>` (`std::bitset`) |
| Mass functions | `ds::MassFunction<N>` — `mass`, `belief`, `plausibility`, `commonality`, `normalize`, `is_valid` |
| Combination | `conflict`, `conjunctive_combination`, `disjunctive_combination`, `dempster_combination` (`operator*`), `yager_combination` |

Umbrella header: `#include <ds/ds.hpp>`.

## Example

Two sensors on $\Theta=\{a,b,c\}$, both lean toward $\{a\}$; Dempster fusion
yields $m(\{a\})=0.88$:

```cpp
#include <ds/ds.hpp>
#include <iostream>

int main() {
    constexpr std::size_t N = 3;
    using Set = ds::FocalSet<N>;
    const Set a{0b001}, theta{0b111};   // LSB-first: {a}, Θ

    ds::MassFunction<N> s1, s2;
    s1.set(a, 0.6); s1.set(theta, 0.4);
    s2.set(a, 0.7); s2.set(theta, 0.3);

    auto fused = s1 * s2;  // dempster_combination
    std::cout << fused.mass(a) << " " << fused.belief(a) << "\n";  // 0.88 0.88
}
```

## Build (Unix: g++ / clang++)

Requires C++17 and CMake ≥ 3.16. **Unix-only** (not supported for Windows).

```sh
# direct
g++ -std=c++17 -Iinclude example.cpp -o example
# or: clang++ -std=c++17 -Iinclude example.cpp -o example

# CMake (demo + tests)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=g++   # or clang++
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/examples/ds_demo
```

`-DDS_BUILD_EXAMPLES=OFF` / `-DDS_BUILD_TESTS=OFF` skips those targets.

### Use from another CMake project

INTERFACE target `ds::ds` (headers + `cxx_std_17` only). As a subdirectory,
examples/tests default **off**.

```cmake
# A: source tree
add_subdirectory(path/to/Dempster-Shafer-library)
target_link_libraries(my_app PRIVATE ds::ds)

# B: install + find_package
# cmake --install build --prefix $HOME/.local
find_package(ds 0.1 REQUIRED)
target_link_libraries(my_app PRIVATE ds::ds)
```

## References

[1] A. P. Dempster, "Upper and lower probabilities induced by a multivalued
    mapping," *Ann. Math. Statist.*, vol. 38, no. 2, pp. 325–339, Apr. 1967,
    doi: [10.1214/aoms/1177698950](https://doi.org/10.1214/aoms/1177698950).

[2] G. Shafer, *A Mathematical Theory of Evidence*. Princeton, NJ, USA:
    Princeton Univ. Press, 1976,
    doi: [10.1515/9780691214696](https://doi.org/10.1515/9780691214696).

[3] R. R. Yager, "On the Dempster–Shafer framework and new combination rules,"
    *Inf. Sci.*, vol. 41, no. 2, pp. 93–137, Mar. 1987,
    doi: [10.1016/0020-0255(87)90007-7](https://doi.org/10.1016/0020-0255(87)90007-7).

[4] P. Smets, "The combination of evidence in the transferable belief model,"
    *IEEE Trans. Pattern Anal. Mach. Intell.*, vol. 12, no. 5, pp. 447–458,
    May 1990, doi: [10.1109/34.55104](https://doi.org/10.1109/34.55104).

[5] D. Dubois and H. Prade, "Representation and combination of uncertainty with
    belief functions and possibility measures," *Comput. Intell.*, vol. 4,
    no. 3, pp. 244–264, Aug. 1988,
    doi: [10.1111/j.1467-8640.1988.tb00279.x](https://doi.org/10.1111/j.1467-8640.1988.tb00279.x).

[6] L. A. Zadeh, "A simple view of the Dempster–Shafer theory of evidence and
    its implication for the rule of combination," *AI Mag.*, vol. 7, no. 2,
    pp. 85–90, 1986,
    doi: [10.1609/aimag.v7i2.542](https://doi.org/10.1609/aimag.v7i2.542).

## License / Lizenz

**EN:** Proprietary. Use and link **unmodified** only; no modified distribution. See [LICENSE](LICENSE).

**DE:** Proprietär. Nur **unverändert** nutzen/linken; keine geänderten Kopien. Siehe [LICENSE](LICENSE).
