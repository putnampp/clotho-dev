#include "genetics/allele_alphabet.h"

#include <cassert>

#include "utility/bitset_ops.hpp"

AlleleAlphabet::AlleleAlphabet() {}

AlleleAlphabet::index_type     AlleleAlphabet::getSymbol( const locus_t & l, const allele_t & a, bool createNew ) {
//    std::cout << "Getting symbol for: " << l << std::endl;

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
        offset = it->second.second;
    }

    if( offset == bitset_type::npos ) {
        // inactive variant
        //
//        std::cout << "Free list size: " << m_free_list.size() << std::endl;
//        std::cout << "count size: " << m_free_list.count() << std::endl;
        offset = m_free_list.find_first();
        if( offset == bitset_type::npos ) {
            offset = m_active.size();
            m_active.push_back( it );
            m_free_list.push_back( false );
            m_free_intersect.push_back(false);
            m_free_union.push_back(false);
        } else {
//            std::cout << "Active size: " << m_active.size() << std::endl;
            assert( offset < m_active.size() );
//            std::cout << "Reusing offset: " << offset << std::endl;
            m_active[ offset ]->second.second = bitset_type::npos;
            m_free_list[ offset ] = false;
        }
        it->second.second = offset;
//        std::cout << "count size: " << m_free_list.count() << std::endl;
//        std::cout << "Free list size: " << m_free_list.size() << std::endl;
    }
//    std::cout << "Offset: " << offset << std::endl;
    return offset;
}

void AlleleAlphabet::updateFreeSymbols( const bitset_type & fs ) {
    // boost::dynamic_bitset returns # of bits as size
//    if( fs.num_blocks() > m_free_list.num_blocks() ) {
//        size_t max = ((m_free_intersect.size() >= m_free_union.size() ) ? m_free_intersect.size() : m_free_union.size());
//        max = ((max >= m_free_list.size()) ? max : m_free_list.size());
//        max = ((max >= fs.size()) ? max : fs.size());

//        assert( max <= fs.size());
//        size_t max = fs.num_blocks() - m_free_list.num_blocks();
//        while( max-- ) {
//            m_free_list.append( 0 );
//            m_free_intersect.append(0);
//            m_free_union.append(0);
//        }
//    }

    
//    typedef bitset_type::block_type block_type;

//    std::cout << "FS: " << fs.size() << std::endl;
//    std::cout << "FL: " << m_free_list.size() 
//                << "; FI: " << m_free_intersect.size()
//                << "; FU: " << m_free_union.size() << std::endl;
    assert((m_free_list.size() == m_free_intersect.size()) && (m_free_list.size() == m_free_union.size()));
    bitset_type b(fs);
    while( b.size() < m_free_list.size() ) {
        b.push_back(false);
    }
//    std::cout << "B: " << b.size() << std::endl;
//    assert( b.size() == m_free_intersect.size() );
    m_free_intersect &= b;
//    assert( b.size() == m_free_union.size());
    m_free_union |= b;
    m_free_list = m_free_intersect | ~m_free_union;
//    block_type * res = &m_free_list.m_bits[0], 
//               * inter = &m_free_intersect.m_bits[0], 
//               * uni = &m_free_union.m_bits[0];
//
//    const block_type * next = &fs.m_bits[0];
//
//    //size_t i = m_free_list.num_blocks();
//    size_t i = fs.num_blocks();
//    while( i-- ) {
//        block_type b = (*next++);
//        (*inter) &= b;
//        (*res) = (*inter++);
//        (*uni)   |= b;
//        (*res++) |= ~(*uni++);
//    }
}

void AlleleAlphabet::resetFreeSymbols() {
    m_free_list.reset();
    m_free_union.reset();
    m_free_intersect.reset();
    m_free_intersect.flip();
//    typedef bitset_type::block_type block_type;
//    block_type * p = &m_free_intersect.m_bits[0];
//    size_t i = m_free_intersect.num_blocks();
//    while( i-- ) {
//        (*p++) = (block_type)-1;
//    }
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
    return m_free_list.size() - m_free_list.count();
}

bool AlleleAlphabet::isLocus( locus_t & l ) const {
    clocus_iterator res = m_db.find(l);
    return res != m_db.end();
}

AlleleAlphabet::adjacency_iterator AlleleAlphabet::begin( edge_set_type * es ) {
    return adjacency_iterator( &m_active, es, 0 );
}

AlleleAlphabet::adjacency_iterator AlleleAlphabet::end( edge_set_type * es ) {
    return adjacency_iterator( &m_active, es );
}

AlleleAlphabet::~AlleleAlphabet() {}
