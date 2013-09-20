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

#include "Sequence.h"
#include <cstring>

//Sequence::Sequence() : m_alleles(NULL), m_nLoci(0), m_maxForms(MAX_ALLELES), m_allocatedLoci(0) {}

Sequence::Sequence( size_t loci ) : m_alleles(), m_nLoci(loci), m_maxForms( MAX_ALLELES ), m_allocatedLoci(0) {
    resizeSeq( m_nLoci );
}

Sequence::Sequence( shared_ptr< allele_t [] > alleles, size_t size ) : m_alleles(alleles), m_nLoci( size ), 
    m_maxForms( MAX_ALLELES ), m_allocatedLoci(size) { }

size_t Sequence::length() const {
    return m_nLoci;
}

allele_t & Sequence::allele( size_t locus ) {
    assert( locus < m_nLoci );
    return m_alleles[ locus ];
}

void    Sequence::resizeSeq( size_t nLoci ) {
    shared_ptr< allele_t [] > tmp( new allele_t[ nLoci ] );
    if( m_allocatedLoci != 0 ) {
        // copy current loci values over
        memcpy( &tmp[0], &m_alleles[0], m_allocatedLoci * sizeof( allele_t ) );

        // delete currently allocated memory
        //delete [] m_alleles;
        m_alleles.reset();
    }

    // establish new memory space
//    m_alleles = tmp;
    m_alleles = tmp;
    m_allocatedLoci = nLoci;
}

Sequence::~Sequence() {
//    if( m_nLoci > 0 )   delete [] m_alleles;
    m_alleles.reset();
}
