#ifndef CORE_GRID_H_
#define CORE_GRID_H_

#include <stdexcept>
#include <vector>
#include "boost/assert.hpp"
#include "boost/shared_ptr.hpp"
#include "node.hpp"

struct BaseMatrix {
    typedef std::size_t size_type;
    BaseMatrix() {}
    size_type Width() { return 0; }
    size_type Height() { return 0; }
    bool IsWalkableAt(size_type x, size_type y) { return true; }
};

template <typename Data>
struct RandomAccessMatrix : public BaseMatrix {
    RandomAccessMatrix(Data *data = 0) : data(data) {}
    size_type Width() { return (*data)[0].size(); }
    size_type Height() { return (*data).size(); }
    bool IsWalkableAt(size_type x, size_type y) {
        // 0, false, null will be walkable
        // while others will be un-walkable
        return !((*data)[y][x]);
    }
    Data *data;
};

template <class Node = BaseNode<std::size_t> >
class Grid {
public:
    typedef std::size_t size_type;
    typedef Node node_t;
    typedef boost::shared_ptr<node_t> pnode_t;
    typedef std::vector<pnode_t> node_vector_t;
    typedef std::vector<node_vector_t> node_grid_t;
    typedef boost::shared_ptr<node_vector_t> pnode_vector_t;
    typedef boost::shared_ptr<node_grid_t> pnode_grid_t;

    Grid(size_type width, size_type height);
    template <class Matrix>
    Grid(size_type width, size_type height, Matrix *matrix);

    pnode_t GetNodeAt(size_type x, size_type y) const;
    bool IsWalkableAt(size_type x, size_type y) const;
    bool IsInside(size_type x, size_type y) const;
    void SetWalkableAt(size_type x, size_type y, bool walkable);
    /**
     * Get the neighbors of the given node.
     *
     *     offsets      diagonalOffsets:
     *  +---+---+---+    +---+---+---+
     *  |   | 0 |   |    | 0 |   | 1 |
     *  +---+---+---+    +---+---+---+
     *  | 3 |   | 1 |    |   |   |   |
     *  +---+---+---+    +---+---+---+
     *  |   | 2 |   |    | 3 |   | 2 |
     *  +---+---+---+    +---+---+---+
     *
     *  When allow_diagonal is true, if offsets[i] is valid, then
     *  diagonalOffsets[i] and
     *  diagonalOffsets[(i + 1) % 4] is valid.
     * @param {pnode_t} node
     * @param {boolean} allow_diagonal
     * @param {boolean} dont_cross_corners
     */
    pnode_vector_t GetNeighbors(pnode_t node,
            bool allow_diagonal,
            bool dont_cross_corners) const;

    size_type width() const { return width_; }
    size_type height() const { return height_; }
    pnode_grid_t nodes() const { return nodes_; }

private:
    template <class Matrix>
    node_grid_t *BuildNodes(size_type width, size_type height,
            Matrix *matrix);

    size_type width_;
    size_type height_;
    pnode_grid_t nodes_;
};

template <class Node>
Grid<Node>::Grid(size_type width, size_type height)
        : width_(width), height_(height) {
    this->nodes_.reset(BuildNodes(width, height, (BaseMatrix *)0));
}

template <class Node>
template <class Matrix>
Grid<Node>::Grid(size_type width, size_type height, Matrix *matrix)
        : width_(width), height_(height) {
    this->nodes_.reset(BuildNodes(width, height, matrix));
}

template <class Node>
typename Grid<Node>::pnode_t
Grid<Node>::GetNodeAt(size_type x, size_type y) const {
    BOOST_ASSERT_MSG(IsInside(x, y), "Oops, GetNodeAt() with incorrect position.");
    return (*(this->nodes_))[y][x];
}

template <class Node>
bool Grid<Node>::IsWalkableAt(size_type x, size_type y) const {
    return this->IsInside(x, y) && (*(this->nodes_))[y][x]->walkable;
}

template <class Node>
bool Grid<Node>::IsInside(size_type x, size_type y) const {
    return (/*x >= 0 &&*/ x < this->width_) &&
           (/*y >= 0 &&*/ y < this->height_);
}

template <class Node>
void Grid<Node>::SetWalkableAt(size_type x, size_type y, bool walkable) {
    (*(this->nodes_))[y][x]->walkable = walkable;
}

template <class Node>
typename Grid<Node>::pnode_vector_t
Grid<Node>::GetNeighbors(pnode_t node,
        bool allow_diagonal,
        bool dont_cross_corners) const {
    size_type x = node->x,
              y = node->y;
    bool s0 = false, d0 = false,
         s1 = false, d1 = false,
         s2 = false, d2 = false,
         s3 = false, d3 = false;
    pnode_vector_t neighbors(new node_vector_t);
    node_grid_t &nodes = *(this->nodes_);

    // Attention: -1 will be a large number when size_type is unsigned.
    // ↑
    if (IsWalkableAt(x, y - 1)) {
        neighbors->push_back(nodes[y - 1][x]);
        s0 = true;
    }
    // →
    if (IsWalkableAt(x + 1, y)) {
        neighbors->push_back(nodes[y][x + 1]);
        s1 = true;
    }
    // ↓
    if (IsWalkableAt(x, y + 1)) {
        neighbors->push_back(nodes[y + 1][x]);
        s2 = true;
    }
    // ←
    if (IsWalkableAt(x - 1, y)) {
        neighbors->push_back(nodes[y][x - 1]);
        s3 = true;
    }

    if (!allow_diagonal) {
        return neighbors;
    }

    if (dont_cross_corners) {
        d0 = s3 && s0;
        d1 = s0 && s1;
        d2 = s1 && s2;
        d3 = s2 && s3;
    } else {
        d0 = s3 || s0;
        d1 = s0 || s1;
        d2 = s1 || s2;
        d3 = s2 || s3;
    }

    // ↖
    if (d0 && IsWalkableAt(x - 1, y - 1)) {
        neighbors->push_back(nodes[y - 1][x - 1]);
    }
    // ↗
    if (d1 && IsWalkableAt(x + 1, y - 1)) {
        neighbors->push_back(nodes[y - 1][x + 1]);
    }
    // ↘
    if (d2 && IsWalkableAt(x + 1, y + 1)) {
        neighbors->push_back(nodes[y + 1][x + 1]);
    }
    // ↙
    if (d3 && IsWalkableAt(x - 1, y + 1)) {
        neighbors->push_back(nodes[y + 1][x - 1]);
    }

    return neighbors;
}

template <class Node>
template <class Matrix>
typename Grid<Node>::node_grid_t *
Grid<Node>::BuildNodes(size_type width,
        size_type height, Matrix *matrix) {
    size_type i, j;

    node_grid_t *nodes = new node_grid_t;
    nodes->resize(height);
    for (i = 0; i < height; ++i) {
        (*nodes)[i].resize(width);
        for (j = 0; j < width; ++j) {
            (*nodes)[i][j].reset(new node_t(j, i, true));
        }
    }

    if (!matrix) {
        return nodes;
    }

    if (width != matrix->Width() || height != matrix->Height()) {
        throw std::runtime_error("Matrix size does not fit");
    }

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            if (!matrix->IsWalkableAt(j, i)) {
                (*nodes)[i][j]->walkable = false;
            }
        }
    }

    return nodes;
}

#endif // CORE_GRID_H_
