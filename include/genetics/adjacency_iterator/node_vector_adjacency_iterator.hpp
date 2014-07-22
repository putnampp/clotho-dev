#ifndef NODE_VECTOR_ADJACENCY_ITERATOR_HPP_
#define NODE_VECTOR_ADJACENCY_ITERATOR_HPP_

#include "adjacency_iterator_decl.hpp"

#include <boost/dynamic_bitset.hpp>
#include <vector>

#include <iostream>

//
// SPECIALIZATION
//
template < class V, class B >
class AdjacencyIterator< std::vector< V >, boost::dynamic_bitset< B > > {
public:
    typedef std::vector< V >            graph_type;
    typedef boost::dynamic_bitset< B >  edge_set_type;

    typedef V               vertex_type;

    typedef graph_type *    graph_pointer;
    typedef edge_set_type * edge_set_pointer;

    AdjacencyIterator( graph_pointer gp, edge_set_pointer es, size_t idx = edge_set_type::npos );
    AdjacencyIterator( const AdjacencyIterator< graph_type, edge_set_type > & other );

    AdjacencyIterator< graph_type, edge_set_type > & operator++();
    AdjacencyIterator< graph_type, edge_set_type > operator++(int);

    bool operator==( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
    bool operator!=( const AdjacencyIterator< graph_type, edge_set_type > & rhs );

    bool operator<( const AdjacencyIterator< graph_type, edge_set_type > & rhs );
    bool comparable( const AdjacencyIterator< graph_type, edge_set_type > & rhs );

    vertex_type operator*();

    virtual ~AdjacencyIterator();
protected:
    graph_pointer       m_graph;
    edge_set_pointer    m_edges;
    size_t              m_index;
};

#define THEADER template < class V, class B >
#define SPECIALIZATION AdjacencyIterator< std::vector< V >, boost::dynamic_bitset< B > >

THEADER SPECIALIZATION::AdjacencyIterator( graph_pointer gp, edge_set_pointer es, size_t idx ) :
    m_graph(gp),
    m_edges(es),
    m_index( idx )
{
    if( m_index != edge_set_type::npos ) {
        if( m_edges->size() == 0 || m_index >= m_edges->size() ) {
            m_index = edge_set_type::npos;
        } else if( !(*m_edges)[m_index] ) {
            operator++();
        }
    }
}

THEADER SPECIALIZATION::AdjacencyIterator( const SPECIALIZATION & other ) :
    m_graph(other.m_graph),
    m_edges(other.m_edges),
    m_index( other.m_index )
{}

THEADER
SPECIALIZATION & SPECIALIZATION::operator++() {
    if( m_index != edge_set_type::npos ) {
        m_index = m_edges->find_next( m_index );
    }
    return *this;
}

THEADER
SPECIALIZATION SPECIALIZATION::operator++(int) {
    SPECIALIZATION tmp( *this );
    operator++();
    return tmp;
}

THEADER
typename SPECIALIZATION::vertex_type SPECIALIZATION::operator*() {
//    std::cout << m_graph->size() << std::endl;
    assert( m_index < m_graph->size() );
    return (*m_graph)[m_index];
}

THEADER
bool SPECIALIZATION::operator==( const SPECIALIZATION & rhs) {
//    if( m_graph == rhs.m_graph && m_edges == rhs.m_edges ) {
//        if( m_index != rhs.m_index ) {
//            std::cout << m_index << " != " << rhs.m_index << std::endl;
//        }
//    }

    return m_graph == rhs.m_graph && m_edges == rhs.m_edges && m_index == rhs.m_index;
}

THEADER
bool SPECIALIZATION::operator!=( const SPECIALIZATION & rhs) {
    return !operator==(rhs);
}

THEADER
bool SPECIALIZATION::operator<( const SPECIALIZATION & rhs ) {
    return m_index < rhs.m_index;
}

THEADER
bool SPECIALIZATION::comparable( const SPECIALIZATION & rhs ) {
    return m_graph == rhs.m_graph;
}

THEADER
SPECIALIZATION::~AdjacencyIterator() {}

#undef THEADER
#undef SPECIALIZATION
#endif  // NODE_VECTOR_ADJACENCY_ITERATOR_HPP_
