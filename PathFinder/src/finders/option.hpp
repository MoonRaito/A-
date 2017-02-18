#ifndef FINDERS_OPTION_HPP_
#define FINDERS_OPTION_HPP_

#include "boost/function.hpp"

struct FinderOption {
    typedef boost::function<int(int, int)> heuristic_t;
    // Whether diagonal movement is allowed.
    bool allow_diagonal;
    // Disallow diagonal movement touching block corners.
    bool dont_cross_corners;
    // Heuristic function to estimate the distance.
    heuristic_t heuristic;
    // Weight to apply to the heuristic to allow for suboptimal paths,
    // in order to speed up the search.
    int weight;
};

#endif // FINDERS_OPTION_HPP_
