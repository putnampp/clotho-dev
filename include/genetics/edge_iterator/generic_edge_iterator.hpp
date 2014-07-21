#ifndef GENERIC_EDGE_ITERATOR_HPP_
#define GENERIC_EDGE_ITERATOR_HPP_

#include "edge_iterator_dec.hpp"

//
// SPECIALIZATION
//
template < class G, class B >
class EdgeIterator< G, boost::dynamic_bitset< B > > {
public:
    typedef G                           graph_type;
    typedef boost::dynamic_bitset< B >  edge_set_type;

    typedef graph_type *    graph_pointer;
    typedef edge_set_type * edge_set_pointer;

    EdgeIterator( graph_pointer gp, edge_set_pointer es );
    EdgeIterator( const EdgeIterator< graph_type, edge_set_type > & other );

    EdgeIterator< graph_type, edge_set_type > & operator++();
    EdgeIterator< graph_type, edge_set_type > operator++(int);

    bool operator==( const EdgeIterator< graph_type, edge_set_type > & rhs );
    bool operator!=( const EdgeIterator< graph_type, edge_set_type > & rhs );

    bool operator<( const EdgeIterator< graph_type, edge_set_type > & rhs );
    bool comparable( const EdgeIterator< graph_type, edge_set_type > & rhs );

    edge_type operator*();

    virtual ~EdgeIterator();
protected:
    graph_pointer   m_graph;
    edge_set_pointer m_edges;
};

#endif  // GENERIC_EDGE_ITERATOR_HPP_
