#ifndef EDGE_ITERATOR_DEC_HPP_
#define EDGE_ITERATOR_DEC_HPP_

#include <type_traits>

template < class G, class ES >
class EdgeIterator {
public:
    typedef G   graph_type;
    typedef ES  edge_set_type;

    typedef typename graph_type::edge_type   edge_type;

    typedef std::add_pointer< graph_type >   graph_pointer;
    typedef std::add_pointer< edge_set_type >   edge_set_pointer;

    EdgeIterator( graph_pointer gp, edge_set_pointer es );
    EdgeIterator( const EdgeIterator< graph_type, edge_set_type > & other );

    EdgeIterator< graph_type, edge_set_type > & operator++();
    EdgeIterator< graph_type, edge_set_type > operator++( int );

    bool operator==( const EdgeIterator< graph_type, edge_set_type > & rhs );
    bool operator!=( const EdgeIterator< graph_type, edge_set_type > & rhs );

    bool operator<( const EdgeIterator< graph_type, edge_set_type > & rhs );
    bool comparable( const EdgeIterator< graph_type, edge_set_type > & rhs );

    edge_type operator*();

    virtual ~EdgeIterator();
protected:
    graph_pointer       m_graph;
    edge_set_pointer    m_edges;
};

#endif  // EDGE_ITERATOR_DEC_HPP_
