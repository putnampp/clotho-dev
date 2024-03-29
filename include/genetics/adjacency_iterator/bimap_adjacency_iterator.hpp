#ifndef BIMAP_ADJACENCY_ITERATOR_HPP_
#define BIMAP_ADJACENCY_ITERATOR_HPP_

#include "adjacency_iterator_decl.hpp"

#include <boost/bimap.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <boost/dynamic_bitset.hpp>

//
// SPECIALIZATION
//
template < class NS1, class NS2, class NR, class B >
class AdjacencyIterator< boost::bimaps::bimap< NS1, NS2, NR >,
    boost::dynamic_bitset< B > > {
public:
    typedef boost::bimaps::bimap< NS1, NS2, NR > graph_type;
    typedef boost::dynamic_bitset< B > edge_set_type;

    typedef typename graph_type::iterator edge_type;

    typedef graph_type *      graph_pointer;
    typedef edge_set_type *   edge_set_pointer;

    AdjacencyIterator( graph_pointer gp, edge_set_pointer es, size_t offset = edge_set_type::npos );
    AdjacencyIterator( const AdjacencyIterator< graph_type, edge_set_type > & other );

    AdjacencyIterator< graph_type, edge_set_type > & operator++();
    AdjacencyIterator< graph_type, edge_set_type > operator++( int );

    bool operator==( const AdjacencyIterator< graph_type, edge_set_type> & );
    bool operator!=( const AdjacencyIterator< graph_type, edge_set_type> & );

    bool operator<( const AdjacencyIterator< graph_type, edge_set_type > & );

    bool comparable( const AdjacencyIterator< graph_type, edge_set_type > & );

    edge_type operator*();

    virtual ~AdjacencyIterator();
protected:
    graph_pointer           m_graph;
    edge_set_pointer        m_edges;
    size_t                  m_edge_index;
    edge_type               m_it;
};

//
// IMPLEMENTATION
//

#define THEADER template < class NS1, class NS2, class NR, class B >

#define SPECIALIZATION AdjacencyIterator< boost::bimaps::bimap< NS1, NS2, NR >, boost::dynamic_bitset< B > >

THEADER SPECIALIZATION::AdjacencyIterator( graph_pointer gp, edge_set_pointer es, size_t idx ) :
    m_graph(gp),
    m_edges(es),
    m_edge_index( idx ),
    m_it( m_graph->end() ) {
    if( m_edge_index != edge_set_type::npos ) {
        if( m_edges->size() == 0 || m_edge_index >= m_edges->size() ) {
            m_edge_index = edge_set_type::npos;
        } else if( !(*m_edges)[ m_edge_index] ) {
            operator++();
        }
    }
}

THEADER SPECIALIZATION::AdjacencyIterator( const SPECIALIZATION & other ) :
    m_graph(other.m_graph),
    m_edges(other.m_edges),
    m_edge_index( other.m_edge_index ),
    m_it( other.m_it ) {
}

THEADER
SPECIALIZATION & SPECIALIZATION::operator++() {
    if( m_edge_index != edge_set_type::npos ) {
        m_edge_index = m_edges->find_next( m_edge_index );
        if( m_edge_index != edge_set_type::npos) {
            m_it = m_graph->begin() + m_edge_index;
        } else {
            m_it = m_graph->end();
        }
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
typename SPECIALIZATION::edge_type SPECIALIZATION::operator*() {
    return m_it;
}

THEADER
bool SPECIALIZATION::operator==( const SPECIALIZATION & rhs) {
    return m_graph == rhs.m_graph && m_edges == rhs.m_edges && m_it == rhs.m_it;
}

THEADER
bool SPECIALIZATION::operator!=( const SPECIALIZATION & rhs) {
    return !operator==(rhs);
}

THEADER
bool SPECIALIZATION::operator<( const SPECIALIZATION & rhs ) {
    return m_edge_index < rhs.m_edge_index;
}

THEADER
bool SPECIALIZATION::comparable( const SPECIALIZATION & rhs ) {
    return m_graph == rhs.m_graph;
}

THEADER
SPECIALIZATION::~AdjacencyIterator() {}

#undef THEADER
#undef SPECIALIZATION

#endif  // BIMAP_ADJACENCY_ITERATOR_HPP_
