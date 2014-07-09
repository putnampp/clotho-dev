#include "genetics/population_alphabet.h"

#include <cassert>

#include "utility/bitset_ops.hpp"

//PopulationAlphabet::variant_db_t PopulationAlphabet::m_db;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_list;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_intersect;
//PopulationAlphabet::bitset_type  PopulationAlphabet::m_free_union;
//
PopulationAlphabet::PopulationAlphabet() {}

PopulationAlphabet::index_type     PopulationAlphabet::getSymbol( const locus_t & l, const allele_t & a, bool createNew ) {
//    std::cout << "Getting symbol for: " << l << std::endl;
    locus_alleles_t res = m_db.left.equal_range( l );
    variant_db_t::iterator it = m_db.end();

    while( res.first != res.second ) {
        if( res.first->second == a ) {
            it = m_db.project_up( res.first );
        } else {
            res.first++;
        }
    }

    size_t offset = m_db.size();
    if( it == m_db.end() ) {
        if( !createNew ) return npos;

        variant_db_t::value_type vt(l, a);

        std::pair< variant_db_t::iterator, bool > res = m_db.insert( m_db.end(), vt);
        assert( res.second );

        it = res.first;
  //      std::cout << it->left << " ?== " << l << std::endl;
        assert( it->left == l );
    //    std::cout << "Added new variant to database: " << l << std::endl;
    } else {
        offset = (it - m_db.begin());
    }

//    std::cout << "variant offset in database: " << offset << std::endl;
    if( m_free_list.size() <= offset ) {
        size_t fspace = m_free_list.find_first();

        if( fspace == bitset_type::npos ) {
            fspace = m_free_list.size();
            m_free_list.push_back(false);
            m_free_intersect.push_back(false);
            m_free_union.push_back(true);
        }

//        std::cout << "First free space: " << fspace << std::endl;

        if( fspace != offset ) {
            variant_db_t::iterator sit = m_db.begin() + fspace;
            variant_db_t::iterator nit = it + 1;
            m_db.relocate( sit, it);
            m_db.relocate( nit, sit);
            m_free_list[fspace] = false;

            offset = fspace;
//        } else {
//            std::cout << "Variant in first free space already" << std::endl;
        }
    }

    return offset;
}

void PopulationAlphabet::updateFreeSymbols( const bitset_type & fs ) {
    // boost::dynamic_bitset returns # of bits as size
    if( fs.num_blocks() > m_free_list.num_blocks() ) {
//        size_t max = ((m_free_intersect.size() >= m_free_union.size() ) ? m_free_intersect.size() : m_free_union.size());
//        max = ((max >= m_free_list.size()) ? max : m_free_list.size());
//        max = ((max >= fs.size()) ? max : fs.size());

//        assert( max <= fs.size());
        size_t max = fs.num_blocks() - m_free_list.num_blocks();
        while( max-- ) {
            m_free_list.append( 0 );
            m_free_intersect.append(0);
            m_free_union.append(0);
        }
    }

    assert( fs.num_blocks() <= m_free_list.num_blocks() );
    typedef bitset_type::block_type block_type;

    block_type * res = &m_free_list.m_bits[0], 
               * inter = &m_free_intersect.m_bits[0], 
               * uni = &m_free_union.m_bits[0];

    const block_type * next = &fs.m_bits[0];

    //size_t i = m_free_list.num_blocks();
    size_t i = fs.num_blocks();
    while( i-- ) {
        block_type b = (*next++);
        (*inter) &= b;
        (*res) = (*inter++);
        (*uni)   |= b;
        (*res++) |= ~(*uni++);
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

size_t PopulationAlphabet::size() {
    return m_db.size();
}

size_t PopulationAlphabet::active_size() {
    return m_free_list.size() - m_free_list.count();
}

bool PopulationAlphabet::isLocus( locus_t & l ) const {
    locus_citerator it = m_db.left.find( l );
    return it != m_db.left.end();
}

PopulationAlphabet::edge_iterator PopulationAlphabet::begin( edge_set_type * es ) {
    return edge_iterator( &m_db, es, 0 );
}

PopulationAlphabet::edge_iterator PopulationAlphabet::end( edge_set_type * es ) {
    return edge_iterator( &m_db, es );
}

PopulationAlphabet::~PopulationAlphabet() {}
