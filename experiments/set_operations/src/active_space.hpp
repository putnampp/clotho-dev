#ifndef ACTIVE_SPACE_HPP_
#define ACTIVE_SPACE_HPP_

#include <boost/dynamic_bitset.hpp>
#include <map>
#include <vector>


//
// Chose to implement "space" as a map of reference counted values
// instead of a multimap of values because the multimap approach
// will enumerate every instance of a value resulting in
// duplicate instances of the same value.  This consequently increases the space
// needed to represent a subset.  Furthermore, it complicates the set operations 
//
//
template < class ValueType = double, class IndirectType = size_t, class BlockType = unsigned long >
class active_space {
public:
    typedef ValueType value_type;
    typedef IndirectType indirect_type;
    typedef std::pair< indirect_type, size_t >          id_count_type;
    typedef std::map< value_type, id_count_type >       active_space_type;
    typedef std::vector< typename active_space_type::iterator >  indexed_active_space;
    typedef boost::dynamic_bitset<BlockType> bitset_type;

    static const indirect_type npos = -1;

    active_space() {}

    indirect_type   getValue( value_type v, bool bCreate = true ) {
        typename active_space_type::iterator it = m_active.find( v );
        indirect_type idx = npos;
        if( it == m_active.end() ) {
            if( !bCreate ) return idx;

            idx = m_free_space.find_first();
            if( idx == bitset_type::npos ) {
                idx = m_indexed.size();
                std::pair< typename active_space_type::iterator, bool > res = m_active.insert( std::make_pair( v, std::make_pair( idx, 1)));
                m_indexed.push_back( res.first );
                m_free_space.push_back(false);
            } else {
                m_active.erase( m_indexed[ idx ] );
                std::pair< typename active_space_type::iterator, bool > res = m_active.insert( std::make_pair( v, std::make_pair( idx, 1)));
                m_indexed[ idx ] = res.first;
                m_free_space[ idx ] = false;
            }
        } else {
            idx = it->second.first;
            it->second.second++;
        }
        return idx;
    }

    void addReference( indirect_type ind ) {
        assert( ind < m_indexed.size() );
        ++m_indexed[ind]->second.second;
    }

    void releaseValue( indirect_type ind ) {
        if( --m_indexed[ ind ]->second.second == 0) {
            m_free_space[ ind ] = true;
        }
    }

    value_type      operator[]( indirect_type ind ) {
        assert( !m_free_space[ind] );
        return m_indexed[ ind ]->first;
    }

    indirect_type operator[]( value_type v ) {
        typename active_space_type::iterator it = m_active.find( v );
        return (( it == m_active.end() ) ? npos : it->second.first);
    }

    virtual ~active_space() {
        m_indexed.clear();
        m_active.clear();
    }
protected:
    active_space_type    m_active;
    indexed_active_space m_indexed;
    bitset_type          m_free_space;
};

#endif  // ACTIVE_SPACE_HPP_
