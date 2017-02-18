#ifndef TEST_PATH_HPP_
#define TEST_PATH_HPP_

#include "boost/array.hpp"

template<std::size_t W, std::size_t H>
struct Maze {
    typedef boost::array<boost::array<int, W>, H> matrix_t;
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    matrix_t matrix;
    int expected_length;
};

#endif // TEST_PATH_HPP_
