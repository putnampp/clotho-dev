#include "genetics/locus.h"

#include <cassert>

Locus::Locus( key_t k ) : m_key(k) {}

void Locus::addAllele( allele_t * m ) {
    m_alleles.push_back(m);
}

allele * Locus::operator[]( size_t idx ) {
    assert( idx < m_alleles.size() );

    return m_alleles[idx];
}

Locus::~Locus() {
    m_alleles.clear();
}
