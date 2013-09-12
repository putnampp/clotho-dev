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

#ifndef GENOME_H_
#define GENOME_H_

#include "common.h"
#include "ploidy.h"
#include "Chromosome.h"
//#include "Individual.h"
#include "Sequence.h"


/***
 *
 * GenomeFactory maintains the set of chromosomes in an indexed
 * array.
 *
 */
template< chromid_t C >
class GenomeFactory {
public:
    static const chromid_t CHROMOSOMES = C;
    static GenomeFactory<C> * getInstance() {
        static GenomeFactory<C > * instance =  new GenomeFactory< C >();
        return instance;
    }

/**
 * Set all copies of a chromosome to be the same length
 */
    virtual void setChromosomeSize( chromid_t c, size_t size ) {
        assert( c < CHROMOSOMES );

        m_base_count -= m_chroms[ c ]->length();
        m_chroms[ c ]->setLength( size );
        m_base_count += m_chroms[ c ]->length();
    }

    virtual void addChromosomeSite( chromid_t c, size_t pos ) {
        assert( c < CHROMOSOMES );

        m_nLoci -= m_chroms[ c ]->loci();
        m_chroms[ c ]->add_site( pos );
        m_nLoci += m_chroms[ c ]->loci();
    }

    virtual SequencePtr build_sequence( chromid_t c ) {
        assert( c < CHROMOSOMES );
        return SequencePtr( new Sequence( m_chroms[ c ]->loci() ));
    }

    virtual ChromosomePtr getChromosome( chromid_t c ) {
        assert( c < CHROMOSOMES );
        return m_chroms[ c ];
    }

    virtual size_t total_size() const {
        return m_base_count;
    }

    virtual size_t loci() const {
        return m_nLoci;
    }

    virtual void reset() {
        for( chromid_t c = 0; c < CHROMOSOMES; ++c ) {
            m_chroms[c].reset();
        }

        m_base_count = 0;
        m_nLoci = 0;

        initialize();
    }

    virtual ~GenomeFactory() {}
protected:
    GenomeFactory( ) : m_base_count(0), m_nLoci(0) {
        initialize();
    }

    virtual void initialize() {
        /// prepopulate the chromosome array with
        /// default chromosomes
        for( chromid_t c = 0; c < CHROMOSOMES; ++c ) {
            m_chroms[ c ] = ChromosomePtr( new Chromosome( "name" ) );

            m_base_count += m_chroms[ c ]->length();
            m_nLoci += m_chroms[ c ]->loci();
        }
    }

    size_t          m_base_count;
    size_t          m_nLoci;
    ChromosomePtr   m_chroms[ CHROMOSOMES ];
};


#endif  // GENOME_H_
