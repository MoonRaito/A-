#ifndef CORE_UTILS_HPP_
#define CORE_UTILS_HPP_

template <typename grid_t>
typename grid_t::pnode_vector_t
Backtrace(typename grid_t::pnode_t pnode) {
    typedef typename grid_t::node_vector_t node_vector_t;
    typedef typename grid_t::pnode_vector_t pnode_vector_t;
    pnode_vector_t pnode_path(new node_vector_t);
    pnode_path->insert(pnode_path->begin(), pnode);
    while (pnode->parent) {
        pnode = pnode->parent;
        pnode_path->insert(pnode_path->begin(), pnode);
    }
    return pnode_path;
}

#endif // CORE_UTILS_HPP_
