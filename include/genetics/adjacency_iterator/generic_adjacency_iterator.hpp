#ifndef GENERIC_ADJACENCY_ITERATOR_HPP_
#define GENERIC_ADJACENCY_ITERATOR_HPP_

#include "adjacency_iterator_dec.hpp"

//
// SPECIALIZATION
//
//template < class G, class B >
//class AdjacencyIterator< G, boost::dynamic_bitset< B > > {
//public:
//    typedef G                           graph_type;
//    typedef boost::dynamic_bitset< B >  edge_set_type;
//
//    typedef typename graph_type::vertex_type     vertex_type;
//
//    typedef graph_type *    graph_pointer;
//    typedef edge_set_type * edge_set_pointer;
//
//    AdjacencyIterator( graph_pointer gp, edge_set_pointer es );
//    AdjacencyIterator( const AdjacencyIterator< graph_type, edge_set_type > & other );
//
//    AdjacencyIterator< graph_type, edge_set_type > & operator++();
//    AdjacencyIterator< graph_type, edge_set_type > operator++(int);
//
//    bool operator==( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
//    bool operator!=( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
//
//    bool operator<( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
//    bool comparable( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
//
//    vertex_type operator*();
//
//    virtual ~AdjacencyIterator();
//protected:
//    graph_pointer   m_graph;
//    edge_set_pointer m_edges;
//};

#endif  // GENERIC_ADJACENCY_ITERATOR_HPP_
