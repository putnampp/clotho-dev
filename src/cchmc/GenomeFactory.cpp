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

#include "GenomeFactory.hpp"

#include <boost/lexical_cast.hpp>

#include <iostream>
using std::cout;
using std::endl;

GenomeFactory::GenomeFactory() : m_nBases(0), m_nLoci(0) {}

GenomeFactory::GenomeFactory( size_t chroms ) : m_nBases(0), m_nLoci(0) {
    for( size_t i = 0; i < chroms; ++i ) {
        ChromosomePtr c( new Chromosome( "chr" + boost::lexical_cast<String>( i ) ) );
        m_chroms.insert( make_pair(c->id(), c) );

        m_nBases += c->length();
        m_nLoci += c->loci();
    }
}

GenomeFactory::GenomeFactory( const vector< ChromosomePtr > & c ) : m_nBases(0), m_nLoci(0) {
    for( vector< ChromosomePtr >::const_iterator it = c.begin(); it != c.end(); it++ ) {
        m_chroms.insert( make_pair( (*it)->id(), (*it) ));
        m_nBases += (*it)->length();
        m_nLoci += (*it)->loci();
    }
}

GenomeFactory::~GenomeFactory() { reset(); }

bool GenomeFactory::addChromosomeSite( chromid_t c, size_t pos, bool bByIndex ) {

    ChromosomePtr chr = ((bByIndex) ? getChromosomeByIndex( (size_t) c ) : getChromosomeByID( c ));

    bool bRet = (chr != NULL_CHROMOSOME );
    if( bRet ) {
        m_nBases -= chr->length();
        m_nLoci  -= chr->loci();

        bRet = chr->add_site( pos );

        m_nBases += chr->length();
        m_nLoci  += chr->loci();
    }

    return bRet;
}
/*
SequencePtr GenomeFactory::build_sequence( chromid_t c ) {
    ChromosomePtr chr = getChromosomeByIndex( c );
    if( chr == NULL ) {
        cout << "Null chromosome" << endl;
    }
    return ((chr == NULL ) ? NULL_SEQUENCE :
                SequencePtr( new Sequence( chr->loci() ) ));
}
*/

ChromosomePtr GenomeFactory::getChromosomeByID( chromid_t c ) const {
    ChromosomeIter it = m_chroms.find( c );
    return ((it == m_chroms.end() ) ? 
                NULL_CHROMOSOME : it->second);
}

ChromosomePtr GenomeFactory::getChromosomeByIndex( size_t idx ) const {
    ChromosomePtr p = NULL_CHROMOSOME;
    if( idx < m_chroms.size() ) {
        ChromosomeIter it = m_chroms.begin();
        while ( idx > 0 && it != m_chroms.end() ) { it++; --idx; }
        p = it->second;
    }
    return p;
}

bool GenomeFactory::isChromosome( chromid_t c ) {
    return (m_chroms.find(c) != m_chroms.end());
}

size_t  GenomeFactory::size() const {
    return m_nBases;
}

size_t  GenomeFactory::loci() const {
    return m_nLoci;
}

size_t GenomeFactory::chromosomes() const {
    return m_chroms.size();
}

void    GenomeFactory::reset() {
    for( Chromosomes::iterator it = m_chroms.begin(); it != m_chroms.end(); it++ ) {
        it->second->clear_sites();
    }

    m_nLoci = 0;
}

GenomeFactory::ChromosomeIter GenomeFactory::begin() const {
    return m_chroms.begin();
}

GenomeFactory::ChromosomeIter GenomeFactory::end() const {
    return m_chroms.end();
}


