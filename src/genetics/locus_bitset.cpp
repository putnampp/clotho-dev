#include "genetics/locus_bitset.h"

locus_bitset::locus_bitset( alphabet_t * a ) :
    m_bits(),
    m_copies(1),
    m_alphabet( a )
{}

locus_bitset::locus_bitset( const locus_bitset & la ) :
    m_bits(la.m_bits),
    bit_vector( la ),
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

locus_bitset::allele_iterator locus_bitset::allele_begin() {
    return allele_iterator( begin(), m_alphabet);
}

locus_bitset::allele_iterator locus_bitset::allele_end() {
    return allele_iterator( end(), m_alphabet);
}

void locus_bitset::release() {
    if( --m_copies == 0 ) { delete this; }
}

locus_bitset::~locus_bitset() {}
