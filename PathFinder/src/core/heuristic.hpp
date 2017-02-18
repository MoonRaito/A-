#ifndef CORE_HEURISTIC_HPP_
#define CORE_HEURISTIC_HPP_

#include <math.h>
#include <iostream>

namespace heuristic {

// diff_t: Negative is allowed.

// Manhattan distance.
struct Manhattan {
    int operator()(int dx, int dy) {
        return std::abs(dx) + std::abs(dy);
    }
};

// Euclidean distance.
struct Euclidean {
    int operator()(int dx, int dy) {
        return int(sqrt(double(dx*dx + dy*dy)));
    }
};

// Chebyshev distance.
struct Chebyshev {
    int operator()(int dx, int dy) {
        return std::max(std::abs(dx), std::abs(dy));
    }
};

}  // heuristic

#endif // CORE_HEURISTIC_HPP_
