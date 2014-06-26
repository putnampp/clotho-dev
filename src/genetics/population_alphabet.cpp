#include "genetics/population_alphabet.h"

#include <cassert>

#include "utility/bitset_ops.hpp"

//PopulationAlphabet::variant_db_t PopulationAlphabet::m_db;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_list;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_intersect;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_union;

PopulationAlphabet::index_type     PopulationAlphabet::getSymbol( const locus_t & l, const allele_t & a, bool createNew ) {
    locus_alleles_t res = m_db.left.equal_range( l );
    variant_db_t::iterator it = m_db.end();

    while( res.first != res.second ) {
        if( res.first->second == a ) {
            it = m_db.project_up( res.first );
        } else {
            res.first++;
        }
    }

    if( it == m_db.end() ) {
        if( !createNew ) return npos;

        m_db.push_back( variant_db_t::value_type(l, a) );
        it = m_db.begin() + (m_db.size() - 1);
    }

    size_t offset = (it - m_db.begin());
    if( m_free_list.size() <= offset ) {
        size_t fspace = m_free_list.find_first();

        if( fspace == bitset_type::npos ) {
            fspace = m_free_list.size();
            m_free_list.push_back(false);
        }

        if( fspace != offset ) {
            variant_db_t::iterator sit = m_db.begin() + fspace;
            variant_db_t::iterator nit = it + 1;
            m_db.relocate( sit, it);
            m_db.relocate( nit, sit);
            m_free_list[fspace] = false;

            offset = fspace;
        }
    }

    return offset;
}

void PopulationAlphabet::updateFreeSymbols( const bitset_type & fs ) {
    // boost::dynamic_bitset returns # of bits as size
    size_t max = ((m_free_intersect.size() >= m_free_union.size() ) ? m_free_intersect.size() : m_free_union.size());
    max = ((max >= m_free_list.size()) ? max : m_free_list.size());
    max = ((max >= fs.size()) ? max : fs.size());

    assert( max <= fs.size());
    m_free_list.resize( max );
    m_free_intersect.resize( max );
    m_free_union.resize( max );

    typedef bitset_type::block_type block_type;

    block_type * res = &m_free_list.m_bits[0], 
               * inter = &m_free_intersect.m_bits[0], 
               * uni = &m_free_union.m_bits[0];

    const block_type * next = &fs.m_bits[0];

    size_t i = m_free_list.num_blocks();
    while( i-- ) {
        block_type b = (*next++);
        (*inter) &= b;
        (*uni)   |= b;
        (*res++) = ((*inter++) | (~(*uni++)));
    }
}

void PopulationAlphabet::resetFreeSymbols() {
    m_free_list.reset();
    m_free_union.reset();

    typedef bitset_type::block_type block_type;

    block_type * p = &m_free_intersect.m_bits[0];
    size_t i = m_free_intersect.num_blocks();
    while( i-- ) {
        (*p++) = (block_type)-1;
    }
}

size_t PopulationAlphabet::block_count() {
    return m_free_list.num_blocks();
}

PopulationAlphabet::edge_type PopulationAlphabet::operator[]( index_type idx ) {
    assert( idx < m_db.size() );
    return m_db.begin() + idx;
}

PopulationAlphabet::index_type PopulationAlphabet::operator[]( edge_type sym ) {
    return (sym - m_db.begin());
}
