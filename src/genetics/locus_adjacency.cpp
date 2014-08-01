#include "genetics/locus_adjacency.h"

locus_adjacency::locus_adjacency( alphabet_t * a ) :
    bit_vector(),
    m_copies(1),
    m_alphabet( a ) {
}

locus_adjacency::locus_adjacency( const locus_adjacency & la ) :
    bit_vector( la ),
    m_copies( la.m_copies ),
    m_alphabet(la.m_alphabet) {
}

locus_adjacency::pointer locus_adjacency::copy() {
    ++m_copies;
    return this;
}

locus_adjacency::pointer locus_adjacency::clone() {
    pointer c = new locus_adjacency( *this );

    c->m_copies = 1;

    return c;
}

locus_adjacency::allele_iterator locus_adjacency::allele_begin() {
    return allele_iterator( begin(), m_alphabet);
}

locus_adjacency::allele_iterator locus_adjacency::allele_end() {
    return allele_iterator( end(), m_alphabet);
}

void locus_adjacency::release() {
    if( --m_copies == 0 ) {
        delete this;
    }
}

locus_adjacency::~locus_adjacency() {}
