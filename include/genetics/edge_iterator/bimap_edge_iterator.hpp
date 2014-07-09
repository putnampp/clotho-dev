#ifndef BIMAP_EDGE_ITERATOR_HPP_
#define BIMAP_EDGE_ITERATOR_HPP_

#include "edge_iterator_dec.hpp"

#include <boost/bimap.hpp>
#include <boost/bimap/vector_of.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>

#include <boost/dynamic_bitset.hpp>

//template < 
//    template < class, class...> class NS1,
//        class N1, class... NS1REST,
//    template < class, class...> class NS2,
//        class N2, class... NS2REST,
//    class AP1, class AP2,
//    class B,   class BA >
//class EdgeIterator< boost::bimaps::bimap< NS1<N1, NS1REST... >, NS2<N2, NS2REST... >, AP1, AP2 >, 
//                    boost::dynamic_bitset< B, BA > > {
//public:
//    typedef boost::bimaps::bimap< NS1<N1, NS1REST... >, NS2<N2, NS2REST... >, AP1, AP2 > graph_type;
//    typedef boost::dynamic_bitset<B, BA>   edge_set_type;
//
template < class NS1, class NS2, class NR, class B >
class EdgeIterator< boost::bimaps::bimap< NS1, NS2, NR >, 
                    boost::dynamic_bitset< B > > {
public:
    typedef boost::bimaps::bimap< NS1, NS2, NR > graph_type;
    typedef boost::dynamic_bitset< B > edge_set_type;

    typedef typename graph_type::iterator edge_type;

    typedef graph_type *      graph_pointer;
    typedef edge_set_type *   edge_set_pointer;

    EdgeIterator( graph_pointer gp, edge_set_pointer es, size_t offset = edge_set_type::npos );
    EdgeIterator( const EdgeIterator< graph_type, edge_set_type > & other );

    EdgeIterator< graph_type, edge_set_type > & operator++();
    EdgeIterator< graph_type, edge_set_type > operator++( int );

    bool operator==( const EdgeIterator< graph_type, edge_set_type> & );
    bool operator!=( const EdgeIterator< graph_type, edge_set_type> & );

    bool operator<( const EdgeIterator< graph_type, edge_set_type > & );

    bool comparable( const EdgeIterator< graph_type, edge_set_type > & );

    edge_type operator*();

    virtual ~EdgeIterator();
protected:
    graph_pointer           m_graph;
    edge_set_pointer        m_edges;
    size_t                  m_edge_index;
    edge_type               m_it;
};

//
// IMPLEMENTATION
//

//#define THEADER template < \
//    template < class, class...> class NS1,\
//        class N1, class... NS1REST,\
//    template < class, class...> class NS2,\
//        class N2, class... NS2REST,\
//    class AP1, class AP2,\
//    class B,   class BA >
//#define BITSET boost::dynamic_bitset< B, BA >
//
//#define SPECIALIZATION EdgeIterator< boost::bimaps::bimap< NS1< N1, NS1REST...>, NS2<N2, NS2REST...>, AP1, AP2 >, BITSET >

#define THEADER template < class NS1, class NS2, class NR, class B >

#define SPECIALIZATION EdgeIterator< boost::bimaps::bimap< NS1, NS2, NR >, boost::dynamic_bitset< B > >

THEADER SPECIALIZATION::EdgeIterator( graph_pointer gp, edge_set_pointer es, size_t idx ) :
    m_graph(gp),
    m_edges(es),
    m_edge_index( idx ),
    m_it( m_graph->end() )
{
    if( m_edge_index != edge_set_type::npos && m_edge_index < m_edges->size() && !(*m_edges)[ m_edge_index] ) {
        operator++();
    }
}

THEADER SPECIALIZATION::EdgeIterator( const SPECIALIZATION & other ) :
    m_graph(other.m_graph),
    m_edges(other.m_edges),
    m_edge_index( other.m_edge_index ),
    m_it( other.m_it )
{}

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
SPECIALIZATION::~EdgeIterator() {}

#undef THEADER
#undef SPECIALIZATION

#endif  // BIMAP_EDGE_ITERATOR_HPP_
