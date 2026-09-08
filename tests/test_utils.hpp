#pragma once

// Assertion helpers that always run (unlike <cassert>, which is a no-op under
// NDEBUG / typical Release builds). Failed checks print and return non-zero
// from main via a shared failure counter.

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace ds_test {

inline int& failure_count()
{
    static int n = 0;
    return n;
}

inline void fail(const char* expr, const char* file, int line)
{
    ++failure_count();
    std::cerr << file << ':' << line << ": check failed: " << expr << '\n';
}

inline bool approx(double a, double b, double tol = 1e-9)
{
    return std::abs(a - b) <= tol;
}

inline int summary_and_exit(const char* suite_name)
{
    if (failure_count() != 0)
    {
        std::cerr << suite_name << ": " << failure_count() << " check(s) failed.\n";
        return EXIT_FAILURE;
    }
    std::cout << suite_name << ": all checks passed.\n";
    return EXIT_SUCCESS;
}

} // namespace ds_test

#define DS_CHECK(expr)                                                       \
    do                                                                       \
    {                                                                        \
        if (!(expr))                                                         \
            ::ds_test::fail(#expr, __FILE__, __LINE__);                      \
    } while (0)

#define DS_CHECK_APPROX(a, b) DS_CHECK(::ds_test::approx((a), (b)))
