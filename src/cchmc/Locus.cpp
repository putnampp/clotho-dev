/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 ******************************************************************************/

#include "cchmc/Locus.h"

#include <sstream>
using std::stringstream;

locus_id::locus_id( chromid_t c, pos_t s, pos_t e ) :
    m_id( createID( c, s, e ) ) {}

locus_id::~locus_id() {}

unsigned long locus_id::createID( chromid_t c, pos_t s, pos_t e ) {
    assert( c < 255 );
    assert( s < (1 << 28));
    assert( e < (1 << 28));
    unsigned long id = ((unsigned long) c) << 56;
    id |= ((unsigned long) s) << 28;
    id |= ((unsigned long) e);
    return id;
}

unsigned long locus_id::getKey() const {
    return m_id;
}

chromid_t  locus_id::getChromosome() const {
    return (chromid_t) (m_id >> 56);
}

pos_t locus_id::getStart() const {
    return (pos_t)(( m_id >> 28 ) & 0x000000000FFFFFFF);
}

pos_t locus_id::getEnd() const {
    return (pos_t)( m_id & 0x000000000FFFFFFF);
}

bool locus_id::operator==( const locus_id & rhs ) const {
    return m_id == rhs.m_id;
}

Locus::Locus( chromid_t c, pos_t s, pos_t e, allele_t n ) :
    m_id( c, s, e ),
    m_nAlleles(n)
/* alt_alleles( new unordered_map<allele_t, double>() ) */ {}

const locus_id & Locus::getID() const {
    return m_id;
}

chromid_t   Locus::getChromosome() const {
    return m_id.getChromosome();
}

pos_t       Locus::getStart()   const {
    return m_id.getStart();
}

pos_t       Locus::getEnd()     const {
    return m_id.getEnd();
}

allele_t    Locus::getMaxAlleles() const {
    return m_nAlleles;
}

/*
bool Locus::addAlternateAllele( allele_t all ) {
    unordered_map< allele_t, double >::iterator it = alt_alleles->find(all);

    if( it == alt_alleles->end()) {
        // dne
        (*alt_alleles)[all] = (double)(alt_alleles->size());
        return true;
    }
    return false;
}

bool Locus::addAlternateAllele( allele_t all, double all_eff ) {
    (*alt_alleles)[ all ] = all_eff;
    return true;
}
*/

bool Locus::isKnownAllele( allele_t all ) const {
    /*
        unordered_map< allele_t, double >::iterator it = alt_alleles->find(all);

        return it != alt_alleles->end();
    */
    return (all < m_nAlleles );
}

/**
 *
 * The default allelic effect of each allele
 *
 */
/*double Locus::operator()( const allele_t all ) const {
    unordered_map< allele_t, double >::iterator it = alt_alleles->find(all);
    if( it != alt_alleles->end() ) {
        return it->second;
    }
    return  0.0;
}*/

/***
 *
 * default algorithm for computing genotype:
 *
 * foreach allele of chromosomes
 *   - geno += index_of allele in alt_alleles, if allele not deleted
 *   - geno += -1.0, otherwise
 *
 */
/*double Locus::genotype( const allele_tuple * at ) const {
    double geno = 0.0;

    for( ploidy_t p = 0; p < at->ploidy(); ++p ) {
        allele_t a = (*at)[p];
        geno += operator()(a);
    }

    return geno;
}

double Locus::genotype( const allele_tuple * at, const allelic_effect * ae ) const {
    if( ae ) {
        double geno = 0.0;
        for( ploidy_t p = 0; p < at->ploidy(); ++p ) {
            geno += (*ae)( (*at)[p] );
        }
        return geno;
    }
    return genotype(at);
}*/

bool    Locus::operator==( const Locus & rhs ) const {
    return m_id == rhs.m_id;
}

Locus::~Locus() {
    /*
        alt_alleles->clear();
        alt_alleles.reset();
    */
}
