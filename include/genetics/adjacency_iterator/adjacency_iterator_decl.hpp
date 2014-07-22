#ifndef ADJACENCY_ITERATOR_DEC_HPP_
#define ADJACENCY_ITERATOR_DEC_HPP_

#include <type_traits>

template < class G, class ES >
class AdjacencyIterator {
public:
    typedef G   graph_type;
    typedef ES  edge_set_type;

    typedef typename graph_type::iterator   vertex_type;

    typedef graph_type *    graph_pointer;
    typedef edge_set_type * edge_set_pointer;

    AdjacencyIterator( graph_pointer gp, edge_set_pointer es );
    AdjacencyIterator( const AdjacencyIterator< graph_type, edge_set_type > & other );

    AdjacencyIterator< graph_type, edge_set_type > & operator++();
    AdjacencyIterator< graph_type, edge_set_type > operator++( int );

    bool operator==( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
    bool operator!=( const AdjacencyIterator< graph_type, edge_set_type > & rhs );

    bool operator<( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
    bool comparable( const AdjacencyIterator< graph_type, edge_set_type > & rhs );

    vertex_type operator*();

    virtual ~AdjacencyIterator();
protected:
    graph_pointer       m_graph;
    edge_set_pointer    m_edges;
};

#endif  // ADJACENCY_ITERATOR_DEC_HPP_
