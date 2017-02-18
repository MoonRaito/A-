#define BOOST_TEST_MODULE GridTest
#include <boost/test/unit_test.hpp>

#include "boost/array.hpp"
#include "boost/bind.hpp"
#include "boost/foreach.hpp"
#include "boost/function.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/range/algorithm/stable_sort.hpp"
#include "core/grid.hpp"

/* generate without matrix */

struct GridWithoutMatrix {
    typedef std::size_t size_type;
    GridWithoutMatrix() : width(10), height(20) {
        grid.reset(new Grid<>(width, height));
    }
    size_type width;
    size_type height;
    boost::scoped_ptr<Grid<> > grid;
};

BOOST_FIXTURE_TEST_SUITE(generate_without_matrix, GridWithoutMatrix)

BOOST_AUTO_TEST_CASE(should_have_correct_size) {
    BOOST_REQUIRE_EQUAL(width, grid->width());
    BOOST_REQUIRE_EQUAL(height, grid->height());

    Grid<>::pnode_grid_t nodes = grid->nodes();

    BOOST_REQUIRE_EQUAL(height, nodes->size());
    for (size_type i = 0; i < height; ++i) {
        BOOST_REQUIRE_EQUAL(width, (*nodes)[i].size());
    }
}

BOOST_AUTO_TEST_CASE(should_set_all_nodes_walkable_attribute) {
    for (size_type i = 0; i < height; ++i) {
        for (size_type j = 0; j < width; ++j) {
            BOOST_REQUIRE(grid->IsWalkableAt(j, i));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

/* generate with matrix */

template<std::size_t MAT_W = 4, std::size_t MAT_H = 5>
struct GridWithMatrix {
    typedef std::size_t size_type;
    typedef boost::array<boost::array<char, MAT_W>, MAT_H> matrix_data_t;
    typedef RandomAccessMatrix<matrix_data_t> matrix_t;
    typedef Grid<> grid_t;
    typedef boost::function<void(size_type, size_type,
        grid_t *)> function_t;

    GridWithMatrix() : width(MAT_W), height(MAT_H) {
        const matrix_data_t temp = {{
            {{1, 0, 0, 1}},
            {{0, 1, 0, 0}},
            {{0, 1, 0, 0}},
            {{0, 0, 0, 0}},
            {{1, 0, 0, 1}},
        }};
        // create the matrix data which duplicated from the temporary one
        matrix_data.reset(new matrix_data_t(temp));
        matrix_t matrix(matrix_data.get());
        grid.reset(new grid_t(width, height, &matrix));
    }
    size_type width;
    size_type height;
    boost::scoped_ptr<matrix_data_t> matrix_data;
    boost::scoped_ptr<grid_t> grid;

    void EnumPos(function_t f) {
        for (size_type y = 0; y < height; ++y) {
            for (size_type x = 0; x < width; ++x) {
                f(x, y, grid.get());
            }
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(generate_with_matrix, GridWithMatrix<>)

BOOST_AUTO_TEST_CASE(should_have_correct_size) {
    BOOST_REQUIRE_EQUAL(width, grid->width());
    BOOST_REQUIRE_EQUAL(height, grid->height());

    Grid<>::pnode_grid_t nodes = grid->nodes();

    BOOST_REQUIRE_EQUAL(height, nodes->size());
    for (size_type i = 0; i < height; ++i) {
        BOOST_REQUIRE_EQUAL(width, (*nodes)[i].size());
    }
}

void initiate_all_nodes(std::size_t x,
        std::size_t y,
        GridWithMatrix<>::grid_t *grid,
        GridWithMatrix<>::matrix_data_t *matrix) {
    if ((*matrix)[y][x]) {
        BOOST_REQUIRE_EQUAL(false, grid->IsWalkableAt(x, y));
    } else {
        BOOST_REQUIRE_EQUAL(true, grid->IsWalkableAt(x, y));
    }
}

BOOST_AUTO_TEST_CASE(should_initiate_all_nodes_walkable_attribute) {
    EnumPos(boost::bind(&initiate_all_nodes, _1, _2, _3, matrix_data.get()));
}

void set_nodes_walkable(std::size_t x, std::size_t y,
        GridWithMatrix<>::grid_t *grid, bool walkable) {
    grid->SetWalkableAt(x, y, walkable);
}

void check_nodes_unwalkable(std::size_t x, std::size_t y,
        GridWithMatrix<>::grid_t *grid, bool walkable) {
    BOOST_REQUIRE_EQUAL(walkable, grid->IsWalkableAt(x, y));
}

BOOST_AUTO_TEST_CASE(should_be_able_to_set_nodes_walkable_attribute) {
    EnumPos(boost::bind(&set_nodes_walkable, _1, _2, _3, false));
    EnumPos(boost::bind(&check_nodes_unwalkable, _1, _2, _3, false));
    EnumPos(boost::bind(&set_nodes_walkable, _1, _2, _3, true));
    EnumPos(boost::bind(&check_nodes_unwalkable, _1, _2, _3, true));
}

BOOST_AUTO_TEST_CASE(should_return_correct_answer_for_position_validity_query) {
    struct Assert {
        size_type x;
        size_type y;
        bool is_inside;
    } asserts[] = {
        {0, 0, true},
        {0, height - 1, true},
        {width - 1, 0, true},
        {width - 1, height - 1, true},
        /*{-1, -1, false},
        {0, -1, false},
        {-1, 0, false},*/
        {0, height, false},
        {width, 0, false},
        {width, height, false},
    };
    BOOST_FOREACH(Assert &a, asserts) {
        BOOST_REQUIRE_EQUAL(a.is_inside, grid->IsInside(a.x, a.y));
    }
}

bool Cmp(const GridWithMatrix<>::grid_t::pnode_t &a,
         const GridWithMatrix<>::grid_t::pnode_t &b) {
    return (a->x * 100 + a->y) > (b->x * 100 + b->y);
};

BOOST_AUTO_TEST_CASE(should_return_correct_neighbors) {
    grid_t::pnode_vector_t &neighbors =
        grid->GetNeighbors(grid->GetNodeAt(1, 0), false, false);
    BOOST_REQUIRE_EQUAL(1, neighbors->size());
    BOOST_REQUIRE_EQUAL(grid->GetNodeAt(2, 0), (*neighbors)[0]);

    grid_t::node_grid_t &nodes = *(grid->nodes());
    neighbors = grid->GetNeighbors(nodes[0][2], true, false);
    boost::range::stable_sort(*neighbors, Cmp);

    grid_t::node_vector_t expected;
    expected.push_back(nodes[0][1]);
    expected.push_back(nodes[1][2]);
    expected.push_back(nodes[1][3]);
    boost::range::stable_sort(expected, Cmp);

    BOOST_REQUIRE_EQUAL(expected.size(), neighbors->size());
    for (size_type i = 0, sz = expected.size(); i < sz; ++i ) {
        BOOST_REQUIRE_EQUAL(expected[i], (*neighbors)[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()

