#include "genetics/locus_bitset.h"

locus_bitset::pool_type locus_bitset::m_pool;

// statics
//

void * locus_bitset::operator new( size_t s ) {
    return m_pool.malloc();
}

locus_bitset::locus_bitset( alphabet_t * a ) :
    m_bits(),
    m_copies(1),
    m_alphabet( a )
{}

locus_bitset::locus_bitset( const locus_bitset & la ) :
    m_bits(la.m_bits),
    m_copies( la.m_copies ),
    m_alphabet(la.m_alphabet)
{}

locus_bitset::pointer locus_bitset::copy() {
    ++m_copies;
    return this;
}

locus_bitset::pointer locus_bitset::clone() {
    pointer c = new locus_bitset( *this );

    c->m_copies = 1;

    return c;
}

void locus_bitset::addVariant( const locus_type & l, const allele_type & a ) {
    index_type idx = alphabet_t::getSymbol( l, a );

    assert( idx != alphabet_t::npos );

    while( m_bits.size() < idx ) {
        m_bits.push_back(false);
    }

    assert( !m_bits[idx] );
    m_bits[idx] = true;
}

void locus_bitset::removeVariant( const locus_type & l, const allele_type & a ) {
   index_type idx = alphabet_t::getSymbol( l, a, false );

    if( idx == alphabet_t::npos || idx >= m_bits.size() ) return;

    m_bits[idx] = false;
}

size_t locus_bitset::size() {
    return m_bits.count();
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
        m_pool.free( this );
    }
}

locus_bitset::~locus_bitset() {}
