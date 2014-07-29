#include "ActiveSpace.h"

ActiveSpace::ActiveSpace() : m_active(), m_indexed(), m_free_space() {}

ActiveSpace::indirect_type   ActiveSpace::getValue( value_type v, bool bCreate ) {
    typename map_type::iterator it = m_active.find( v );
    indirect_type idx = npos;
    if( it == m_active.end() ) {
        if( !bCreate ) return idx;

        idx = m_free_space.find_first();
        if( idx == bitset_type::npos ) {
            idx = m_indexed.size();
            std::pair< typename map_type::iterator, bool > res = m_active.insert( std::make_pair( v, idx));
            m_indexed.push_back( res.first );
            m_free_space.push_back(false);
        } else {
            m_active.erase( m_indexed[ idx ] );
            std::pair< typename map_type::iterator, bool > res = m_active.insert( std::make_pair( v, idx));
            m_indexed[ idx ] = res.first;
            m_free_space[ idx ] = false;
        }
    } else {
        idx = it->second;
    }
    return idx;
}

ActiveSpace::value_type      ActiveSpace::operator[]( indirect_type ind ) {
    assert( !m_free_space[ind] );
    return m_indexed[ ind ]->first;
}

ActiveSpace::indirect_type ActiveSpace::operator[]( value_type v ) {
    typename map_type::iterator it = m_active.find( v );
    return (( it == m_active.end() ) ? npos : it->second);
}

size_t ActiveSpace::size() const {
    return m_indexed.size();
}

ActiveSpace::~ActiveSpace() {
    m_indexed.clear();
    m_active.clear();
}
