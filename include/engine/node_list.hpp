#ifndef NODE_LIST_HPP_
#define NODE_LIST_HPP_

#include "managed_page.hpp"

template < typename O >
class node_list {
public:
    struct _node {
        _node * next;
        unsigned int free_count;
        void *  page;
    };

    typedef ManagedPage< _node, O > object_page_t;
protected:

};

#endif  // NODE_LIST_HPP_
