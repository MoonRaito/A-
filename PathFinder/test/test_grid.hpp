#ifndef TEST_GRID_HPP_
#define TEST_GRID_HPP_

#include <iostream>
#include "boost/array.hpp"

template<std::size_t W, std::size_t H>
std::ostream &operator<<(std::ostream &os,
        const boost::array<boost::array<char, W>, H> &matrix) {
    int i, j, value;
    for (i = 0; i < H; ++i) {
        for (j = 0; j < W; ++j) {
            value = matrix[i][j];
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    return os;
}

template<std::size_t W, std::size_t H>
void AssignBoundary(boost::array<boost::array<char, W>, H> &matrix) {
    size_t i, j;
    size_t h = H - 1, w = W - 1;
    // inside
    for (i = 1; i < h; ++i) {
        for (j = 1; j < w; ++j) {
            matrix[i][j] = 0;
        }
    }
    // left & right boundaries
    for (i = 0; i <= h; ++i) {
        matrix[i][0] = matrix[i][w] = 1;
    }
    // up & down boundaries
    for (j = 1; j < w; ++j) {
        matrix[0][j] = matrix[h][j] = 1;
    }
}

#endif // TEST_GRID_HPP_
