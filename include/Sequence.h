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

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include "common.h"
#include "ploidy.h"

struct sequence {
    virtual size_t length() const = 0;
    virtual allele_t &   allele( size_t strand, size_t locus ) = 0;
};

/**
 * A sequence is an in-silico representation of a set of 
 * allelic values for a given set of loci.
 *
 * Each loci is associated with an enumerated set of forms.
 * The values of the sequence are indices into the enumerated set
 * of forms.
 *
 * The allele_t of the a sequence is intended to be an integral
 * type with sufficient bits to represent the index.
 *
 * A default sequence allows for 256 forms per loci, thus a
 * byte (unsigned char) is sufficient to represent all possible
 * form indices.
 *
 * It is desirable to use one sequence to represent multiple parallel
 * forms of the sequence (ie. use 1 sequence to represent both
 * strands of a chromosome.) Thus, a sequence is best thought of 
 * as a Matrix of values, where the rows correspond to alleles, 
 * and columns are the loci.
 * 
 * A -> ______LOCUS______
 * 0 ->| L0|  L1|  L2|...|
 * 1 ->| L0|  L1|  L2|...|
 * ...
 *
 * An alternative implementation decision would tie the value type
 * V to the ploidy of the sequence. For instance, a Haploid sequence
 * would use a single byte per locus. A Diploid sequence would
 * use a short to represent both strand alleles. This would be essentially
 * the transpose of the current implementation.
 *
 * L ->  _____Allele__
 * 0 -> |  0    |    1| 
 * 1 -> |  0    |    1| 
 * 2 -> |  0    |    1|
 * ...
 *
 *
 **/
template < ploidy_t P >
class Sequence : public sequence {
public:
    Sequence( ) : m_nLoci(0), m_maxForms(MAX_ALLELES), m_allocatedLoci(0) {}

    Sequence( size_t loci ) : m_nLoci(loci), m_maxForms( MAX_ALLELES ), m_allocatedLoci(0) {
        resizeSeq( loci );
    }

    virtual ploidy_t ploidy() const {
        return PLOIDY;
    }

    virtual size_t length() const {
        return m_nLoci;
    }

    virtual allele_t & allele(size_t strand, size_t locus) {
        assert( strand < PLOIDY && locus < m_nLoci );
        return m_alleles[strand][locus];
    }

    virtual ~Sequence() {
        if( m_nLoci > 0 ) delete [] m_alleles[0];
    }

protected:
    static const unsigned char PLOIDY = P;
    allele_t *   m_alleles[ PLOIDY ];

    size_t    m_nLoci;
    size_t    m_maxForms;
private:
    size_t    m_allocatedLoci;

    void    resizeSeq( size_t nLoci ) {
        allele_t * tmp = new allele_t[ ((size_t) PLOIDY) * nLoci ];
        if( m_allocatedLoci != 0 ) {
            // copy current loci values over
            for( unsigned char i = 0; i < PLOIDY; ++i ) {
                memcpy( &tmp[ i * nLoci ], m_alleles[i], m_allocatedLoci * sizeof( allele_t ) );
            }

            // delete currently allocated memory
            delete [] m_alleles[0];
        }

        // establish new memory space
        for( int i = 0; i < PLOIDY; ++i ) {
            m_alleles[i] = &tmp[i * nLoci];
        }

        m_allocatedLoci = nLoci;
    }
};

typedef Sequence< DIPLOID > DiploidSequence;

#endif  // SEQUENCE_H_
