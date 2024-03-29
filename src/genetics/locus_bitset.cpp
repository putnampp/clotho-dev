#include "genetics/locus_bitset.h"
#include <iostream>

// statics
//
locus_bitset  locus_bitset::EMPTY;
typename locus_bitset::active_bitsets locus_bitset::m_active;

bool locus_bitset::isGamete( locus_bitset * lb ) {
    return lb == &EMPTY || m_active.find( lb ) != m_active.end();
}

locus_bitset::active_iterator locus_bitset::active_begin() {
    return m_active.begin();
}

locus_bitset::active_iterator locus_bitset::active_end() {
    return m_active.end();
}

size_t locus_bitset::updateActiveAlphabet() {
    size_t nBlocks = 0;
    active_iterator first = active_begin(), last = active_end();
    if( first != last ) {
        alphabet_t::pointer alpha = (*first)->getAlphabet();
        alpha->resetFreeSymbols();
        while( first != last ) {
            assert( (*first)->getAlphabet() == alpha );
            (*first)->updateSymbols();
            first++;
        }

        if( EMPTY.copies() > 1 ) {
            EMPTY.updateSymbols();
        }

        alpha->setState();

        first = active_begin();
        last = active_end();

        while( first != last ) {
            nBlocks += (*first)->clearFree();
            ++first;
        }
    }
    return nBlocks;
}

locus_bitset::locus_bitset( alphabet_t::pointer a ) :
    m_bits(),
    m_copies(1),
    m_alphabet( a ) {
    if( this != &EMPTY ) {
        m_active.insert( this );
    }
}

locus_bitset::locus_bitset( const bitset_type & bs, alphabet_t::pointer a ) :
    m_bits(bs),
    m_copies(1),
    m_alphabet( a ) {
    if( this != &EMPTY ) {
        m_active.insert( this );
    }
}

locus_bitset::locus_bitset( const locus_bitset & la ) :
    m_bits(la.m_bits),
    m_copies( la.m_copies ),
    m_alphabet(la.m_alphabet) {
    m_active.insert(this);
}

locus_bitset::pointer locus_bitset::copy() {
    ++m_copies;
    return this;
}

locus_bitset::pointer locus_bitset::clone() const {
    pointer c = new locus_bitset( *this );

    c->m_copies = 1;

//    std::pair< active_iterator, bool > res =  m_active.insert( c );
//    assert( res.second );
//
    return c;
}

void locus_bitset::addVariant( index_type idx ) {
    assert( idx != alphabet_t::npos );

    if( m_bits.size() <= idx) {
        m_bits.resize( idx + 1, false );
        if( m_bits.size() > m_alphabet->size() ) {
            std::cerr << "Unexpected size: " << m_bits.size() << " [" << m_alphabet->size() << "]" << std::endl;
        }
        assert( m_bits.size() <= m_alphabet->size());
    }

    m_bits[idx] = true;
}

void locus_bitset::removeVariant( index_type idx ) {
    if( idx == alphabet_t::npos || idx >= m_bits.size() ) return;

    m_bits[idx] = false;
}

void locus_bitset::addVariant( const locus_type & l, const allele_type & a ) {
    index_type idx = m_alphabet->getSymbol( l, a );
    addVariant( idx );
}

void locus_bitset::removeVariant( const locus_type & l, const allele_type & a ) {
    index_type idx = m_alphabet->getSymbol( l, a, false );
    removeVariant( idx );
}

size_t locus_bitset::size() {
    return m_bits.count();
}

size_t locus_bitset::set_size() {
    return m_bits.size();
}

//locus_bitset::adjacency_iterator locus_bitset::begin() {
//    return m_alphabet->begin( &m_bits );
//}
//
//locus_bitset::adjacency_iterator locus_bitset::end() {
//    return m_alphabet->end( &m_bits );
//}

bool locus_bitset::operator[]( index_type idx ) {
    return ((idx < m_bits.size()) ? m_bits[idx] : false);
}

void locus_bitset::updateSymbols() {
    m_alphabet->updateFreeSymbols( m_bits );
}

size_t locus_bitset::clearFree() {
    if( m_alphabet->getFreeMask()->size() == 0 ) {
        return m_bits.num_blocks();
    }

    if( m_bits.size() != m_alphabet->getFreeMask()->size() ) {
        bitset_type b( *m_alphabet->getFreeMask());
        b.resize( m_bits.size(), true );
        m_bits &= b;
    } else {
        m_bits &= *m_alphabet->getFreeMask();
    }

    return m_bits.num_blocks();
}

/*
locus_bitset::allele_iterator locus_bitset::allele_begin() {
    return allele_iterator( begin(), m_alphabet);
}

locus_bitset::allele_iterator locus_bitset::allele_end() {
    return allele_iterator( end(), m_alphabet);
}
*/
void locus_bitset::release() {
    if( --m_copies == 0 ) {
        m_bits.reset();
        if( this != &EMPTY ) {
            m_active.erase( this );
            delete this;
        }
    }
}

locus_bitset & locus_bitset::operator^=( const locus_bitset & rhs ) {
    assert( m_alphabet == rhs.m_alphabet );

    if( rhs.m_bits.size() < m_bits.size() ) {
        bitset_type b( rhs.m_bits );
        b.resize( m_bits.size(), false);
        m_bits ^= b;
    } else {
        if( m_bits.size() < rhs.m_bits.size() ) {
            m_bits.resize( rhs.m_bits.size(), false );
        }

        m_bits ^= rhs.m_bits;
    }

    return *this;
}

locus_bitset & locus_bitset::operator|=( const locus_bitset & rhs ) {
    assert( m_alphabet == rhs.m_alphabet );

    if( rhs.m_bits.size() < m_bits.size() ) {
        bitset_type b( rhs.m_bits );
        b.resize( m_bits.size(), false);
        m_bits |= b;
    } else {
        if( m_bits.size() < rhs.m_bits.size() ) {
            m_bits.resize( rhs.m_bits.size(), false );
        }

        m_bits |= rhs.m_bits;
    }

    return *this;
}

void locus_bitset::masked_join( const locus_bitset & rhs, const bitset_type & mask ) {
    assert( rhs.m_bits.size() <= mask.size() && m_bits.size() <= mask.size() );
    if( m_bits.size() < mask.size() ) {
        m_bits.resize( mask.size(), false );
    }
    if( rhs.m_bits.size() == mask.size() ) {
        m_bits |= (rhs.m_bits & mask);
    } else {
        bitset_type r = rhs.m_bits;
        r.resize( mask.size(), false );
        m_bits |= (r & mask);
    }
}

void locus_bitset::updateFrequencies( std::vector< size_t > & freqs ) const {
    assert( freqs.size() >= m_bits.size() );

    index_type idx = m_bits.find_first();
    while( idx != bitset_type::npos ) {
        freqs[idx] += m_copies;
        idx = m_bits.find_next(idx);
    }
}

locus_bitset::~locus_bitset() {}

bool operator==( const locus_bitset & lhs, const locus_bitset & rhs ) {
    typedef std::vector< locus_bitset::bitset_type::block_type, locus_bitset::bitset_type::allocator_type > buffer_type;
    typedef buffer_type::const_iterator citerator;

    citerator l_it = lhs.m_bits.m_bits.begin(), r_it = rhs.m_bits.m_bits.begin(),
              l_end = lhs.m_bits.m_bits.end(), r_end = rhs.m_bits.m_bits.end();

    bool isSame = true;

    while( isSame ) {
        if( l_it == l_end ) {
            while( isSame && r_it != r_end ) {
                isSame = ((*r_it++) == 0);
            }
        }

        if( r_it == r_end ) {
            while( isSame && l_it != l_end ) {
                isSame = ((*l_it++) == 0);
            }
            break;
        }

        isSame = ((*r_it++) == (*l_it++));
    }

    return isSame;
}
