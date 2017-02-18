#ifndef FINDERS_ASTARFINDER_HPP_
#define FINDERS_ASTARFINDER_HPP_

#define _USE_MATH_DEFINES

#include <math.h>
#include <boost/heap/pairing_heap.hpp>
#include "boost/foreach.hpp"
#include "boost/shared_ptr.hpp"
#include "core/grid.hpp"
#include "core/heuristic.hpp"
#include "core/utils.hpp"
#include "option.hpp"

template <typename subscript_t>
struct AStarNode : public BaseNode<subscript_t> {
    typedef boost::shared_ptr<AStarNode> pnode_t;
    struct FCmp {
        bool operator()(const pnode_t &lhs,
                        const pnode_t &rhs) const {
            return lhs->f > rhs->f;
        }
    };
    typedef boost::heap::pairing_heap<pnode_t,
        boost::heap::compare<FCmp> > heap_t;
    AStarNode(subscript_t x, subscript_t y, bool walkable)
        : BaseNode<subscript_t>(x, y, walkable) { Reset(); }
    void Reset() {
        f = 0; g = 0; h = 0;
        opened = false; closed = false;
        parent.reset();
    }
    int f;
    int g;
    int h;
    bool opened;
    bool closed;
    pnode_t parent;
    typename heap_t::handle_type handle;
};

class AStarFinder {
public:
    typedef std::size_t size_type;
    typedef AStarNode<size_type> node_t;
    typedef Grid<node_t> grid_t;
    typedef grid_t::pnode_t pnode_t;
    typedef grid_t::node_vector_t node_vector_t;
    typedef grid_t::pnode_vector_t pnode_vector_t;
    typedef grid_t::pnode_grid_t pnode_grid_t;
    typedef boost::shared_ptr<FinderOption> poption_t;

    AStarFinder(poption_t op = poption_t()) : op_(op) {
        if (!op_) {
            FinderOption temp = {false, false, heuristic::Manhattan(), 1};
            op_ = poption_t(new FinderOption(temp));  // copy constructor
        }
    }
    inline FinderOption &Option() {
        return *op_;
    }

    // It will change the node's additional attributes.
    // You should ResetGrid(), while you FindPath() again.
    pnode_vector_t
    FindPath(size_type start_x, size_type start_y,
             size_type end_x, size_type end_y,
             const grid_t &grid) const;
    // Reset all nodes' additional attributes.
    void ResetGrid(const grid_t &grid) const;

private:
    poption_t op_;
};

AStarFinder::pnode_vector_t
AStarFinder::FindPath(
        size_type start_x, size_type start_y,
        size_type end_x, size_type end_y,
        const grid_t &grid) const {
    pnode_t pstart_node = grid.GetNodeAt(start_x, start_y),
            pend_node = grid.GetNodeAt(end_x, end_y);
    bool allow_diagonal = op_->allow_diagonal,
         dont_cross_corners = op_->dont_cross_corners;

    node_t::heap_t open_list;

    // push the start node into the open list
    pstart_node->opened = true;
    pstart_node->handle = open_list.push(pstart_node);

    pnode_t pnode;
    size_type x = 0, y = 0;
    int ng = 0;
    // while the open list is not empty
    while (!open_list.empty()) {
        // pop the position of node which has the minimum `f` value.
        pnode = open_list.top();
        open_list.pop();
        pnode->closed = true;

        // if reached the end position, construct the path and return it
        if (pnode == pend_node) {
            return Backtrace<grid_t>(pend_node);
        }

        // get neigbours of the current node
        pnode_vector_t neighbors = grid.GetNeighbors(
            pnode, allow_diagonal, dont_cross_corners);
        BOOST_FOREACH(pnode_t &neighbor, *neighbors) {
            if (neighbor->closed) {
                continue;
            }

            x = neighbor->x;
            y = neighbor->y;

            // get the distance between current node and the neighbor
            // and calculate the next g score
            ng = pnode->g + ((x - pnode->x == 0 || y - pnode->y == 0) ? 10 : 14);

            // check if the neighbor has not been inspected yet, or
            // can be reached with smaller cost from the current node
            if (!neighbor->opened || ng < neighbor->g) {
                neighbor->g = ng;
                if (neighbor->h == 0) {
                    neighbor->h = op_->weight * op_->heuristic(10 * (x - end_x),
                                                               10 * (y - end_y));
                }
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = pnode;

                if (!neighbor->opened) {
                    neighbor->opened = true;
                    neighbor->handle = open_list.push(neighbor);
                } else {
                    // the neighbor can be reached with smaller cost.
                    // Since its f value has been updated, we have to
                    // update its position in the open list
                    open_list.update(neighbor->handle);
                }
            }  // end for each neighbor
        }  // end while not open list empty
    }

    // fail to find the path
    return pnode_vector_t();
}

void AStarFinder::ResetGrid(const grid_t &grid) const {
    const pnode_grid_t &nodes = grid.nodes();
    size_type w = grid.width(), h = grid.height();
    for (size_type i = 0; i < h; ++i) {
        for (size_type j = 0; j < w; ++j) {
            (*nodes)[i][j]->Reset();
        }
    }
}


#endif // FINDERS_ASTARFINDER_HPP_
