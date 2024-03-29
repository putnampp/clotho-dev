#include "genetics/allele_alphabet.h"

#include <cassert>

#include "utility/bitset_ops.hpp"

AlleleAlphabet::AlleleAlphabet() : m_next_free( bitset_type::npos ) {}

AlleleAlphabet::index_type     AlleleAlphabet::getSymbol( const locus_t & l, const allele_t & a, bool createNew ) {
    locus_iterator it = m_db.end();
    allele_iterator a_it = m_alleles.find(a);

    if( a_it != m_alleles.end() ) {
        // replect allele value
        // check existing edges for repeat edge
        std::pair< locus_iterator, locus_iterator > res = m_db.equal_range( l );
        while( res.first != res.second ) {
            if( res.first->second.first == a_it ) {
                it = res.first;
                break;
            } else {
                res.first++;
            }
        }
    } else {
        std::pair< allele_iterator, bool > res = m_alleles.insert( a );
        assert( res.second );
        a_it = res.first;
    }

    index_type offset = bitset_type::npos;
    if( it == m_db.end() ) {
        if( !createNew ) return npos;

        it = m_db.insert( make_pair( l, make_pair( a_it, offset)));
    } else {
        // existing edge
        return npos;
    }

    if( offset == bitset_type::npos ) {
        // inactive variant
        offset = m_next_free;

        if( offset == bitset_type::npos ) {
            offset = m_active.size();
            m_active.push_back( it );
            m_free_list.push_back( false );
            m_free_intersect.push_back(false);
            m_free_union.push_back(false);
        } else {
            assert( offset < m_active.size() );
            m_db.erase( m_active[offset] );
            m_active[offset] = it;
            m_next_free = m_free_list.find_next( m_next_free );
        }
        it->second.second = offset;
    }
    return offset;
}

void AlleleAlphabet::updateFreeSymbols( const bitset_type & fs ) {
    // boost::dynamic_bitset returns # of bits as size
    assert(m_free_intersect.size() == m_free_union.size());

    typedef std::vector< typename bitset_type::block_type, typename bitset_type::allocator_type > buffer_type;

    typedef typename buffer_type::iterator iterator;
    typedef typename buffer_type::const_iterator citerator;

    if( fs.size() == 0 ) {
        m_free_intersect.reset();
    } else if( fs.size() < m_free_intersect.size() ) {
//        bitset_type b(fs);
//        b.resize( m_free_list.size(), false );
//        m_free_intersect &= b;
//        m_free_union |= b;
//
        citerator first = fs.m_bits.begin(), last = fs.m_bits.end();

        iterator int_it = m_free_intersect.m_bits.begin(), int_last = m_free_intersect.m_bits.end(),
                 un_it = m_free_union.m_bits.begin();
        while( first != last ) {
            (*int_it++) &= (*first);
            (*un_it++) |= (*first++);
        }
        while( int_it != int_last ) {
            (*int_it++) = 0;
        }

    } else {
        if( fs.size() > m_free_intersect.size() ) {
            m_free_intersect.resize( fs.size(), false);
            m_free_union.resize( fs.size(), false );
        }

        m_free_intersect &= fs;
        m_free_union |= fs;
    }
}

void AlleleAlphabet::setState() {
    m_free_list = m_free_intersect | ~m_free_union;
    m_next_free = m_free_list.find_first();
    m_free_mask = ~m_free_list;
}

void AlleleAlphabet::resetFreeSymbols() {
    m_free_list.reset();
    m_free_union.reset();
    m_free_intersect.reset().flip();
}

size_t AlleleAlphabet::block_count() {
    return m_free_list.num_blocks();
}

AlleleAlphabet::locus_iterator AlleleAlphabet::operator[]( index_type idx ) {
    assert( idx < m_active.size() );
    return m_active[idx];
}

size_t AlleleAlphabet::size() {
    return m_db.size();
}

size_t AlleleAlphabet::active_size() {
    return m_free_union.count();
    //return m_free_list.size() - m_free_list.count();
}

bool AlleleAlphabet::isLocus( locus_t & l ) const {
    clocus_iterator res = m_db.find(l);
    return res != m_db.end();
}

//AlleleAlphabet::adjacency_iterator AlleleAlphabet::begin( edge_set_type * es ) {
//    return adjacency_iterator( &m_active, es, 0 );
//}

//AlleleAlphabet::adjacency_iterator AlleleAlphabet::end( edge_set_type * es ) {
//    return adjacency_iterator( &m_active, es );
//}

AlleleAlphabet::~AlleleAlphabet() {}
