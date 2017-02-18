#ifndef CORE_NODE_HPP_
#define CORE_NODE_HPP_

#include "boost/scoped_ptr.hpp"

template <typename subscript_t>
struct BaseNode {
    BaseNode(subscript_t x, subscript_t y, bool walkable)
        : x(x), y(y), walkable(walkable) {}
    ~BaseNode() {}
    subscript_t x;
    subscript_t y;
    bool walkable;
};

#endif // CORE_NODE_HPP_
