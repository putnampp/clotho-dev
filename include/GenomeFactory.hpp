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



template< chromid_t C, ploidy_t P >
class GenomeFactory {
public:
    static const chromid_t CHROMOSOMES = C;
    static const ploidy_t PLOIDY = P;
    static GenomeFactory<C, P> * getInstance() {
        static GenomeFactory<C, P > * instance =  new GenomeFactory< C, P >();
        return instance;
    }

    virtual bool addChromosome( const ChromosomePtr c) { assert( false ); return false; }

    virtual shared_ptr< Sequence > build_sequence( chromid_t c ) {
        assert( c < CHROMOSOMES );
        return shared_ptr< Sequence >( new Sequence( m_chroms[ c ]->loci() ));
    }

    virtual size_t total_size() const {
        return m_base_count;
    }

    virtual size_t loci() const {
        return m_nLoci;
    }

    virtual ~GenomeFactory() {}
protected:
    GenomeFactory( ) : m_base_count(0), m_nLoci(0) {
        for( chromid_t c = 0; c < CHROMOSOMES * PLOIDY; ++c ) {
            m_chroms[ c ] = ChromosomePtr( new Chromosome( "name" ) );

            m_base_count += m_chroms[ c ]->length();
            m_nLoci += m_chroms[ c ]->loci();
        }
    }

    size_t          m_base_count;
    size_t          m_nLoci;
    ChromosomePtr   m_chroms[ CHROMOSOMES * PLOIDY ];
};


#endif  // GENOME_H_
