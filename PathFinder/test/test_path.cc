#define BOOST_TEST_MODULE PathTest
#include <boost/test/unit_test.hpp>

#include "boost/function.hpp"
#include "finders/astarfinder.hpp"
#include "test_path.hpp"

template <typename Finder, typename Maze>
void Handler(Finder &finder, Maze &maze) {
    RandomAccessMatrix<Maze::matrix_t> matrix(&maze.matrix);
    Finder::grid_t grid(matrix.Width(), matrix.Height(), &matrix);
    Finder::pnode_vector_t nodes =
    finder.FindPath(maze.start_x, maze.start_y,
                    maze.end_x, maze.end_y, grid);
    BOOST_REQUIRE_EQUAL(maze.expected_length, nodes->size());
}

template <typename Finder>
void EachMaze(Finder &finder) {
    static struct Mazes {
        Maze<2, 2> maze_a;
        Maze<5, 6> maze_b;
        Maze<5, 6> maze_c;
        Maze<20, 20> maze_d;
    } mazes = {
        {0, 0, 1, 1, {{
            {{0, 0}},
            {{1, 0}},
        }}, 3},
        {1, 1, 4, 4, {{
            {{0, 0, 0, 0, 0}},
            {{1, 0, 1, 1, 0}},
            {{1, 0, 1, 0, 0}},
            {{0, 1, 0, 0, 0}},
            {{1, 0, 1, 1, 0}},
            {{0, 0, 1, 0, 0}},
        }}, 9},
        {0, 3, 3, 3, {{
            {{0, 0, 0, 0, 0}},
            {{0, 0, 1, 1, 0}},
            {{0, 0, 1, 0, 0}},
            {{0, 0, 1, 0, 0}},
            {{1, 0, 1, 1, 0}},
            {{0, 0, 0, 0, 0}},
        }}, 10},
        {4, 4, 19, 19, {{
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
            {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
        }}, 31},
    };
    Handler(finder, mazes.maze_a);
    Handler(finder, mazes.maze_b);
    Handler(finder, mazes.maze_c);
    Handler(finder, mazes.maze_d);
};

void TestAStarFinder() {
    AStarFinder finder;
    EachMaze(finder);
}

BOOST_AUTO_TEST_CASE(should_solve_maze) {
    TestAStarFinder();
}
