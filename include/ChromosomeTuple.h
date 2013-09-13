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

#ifndef CHROMOSOMETUPLE_H_
#define CHROMOSOMETUPLE_H_

#include "common.h"
#include "ploidy.h"
#include "Allele.h"
#include "Sequence.h"
#include "Chromosome.h"
#include "Locus.h"

struct chromosome_set {
    virtual size_t   length() const = 0;
    virtual ploidy_t ploidy() const = 0;
    virtual bool allele( ploidy_t copy, size_t locus, allele_t & all ) = 0;
};

template < ploidy_t P >
class ChromosomeTuple : public chromosome_set {
public:
    ChromosomeTuple( const ChromosomePtr c ) : m_chrom(c) {
        for( ploidy_t p = 0; p < PLOIDY; ++p ) {
            m_seqs[p].reset(new Sequence( m_chrom->loci() ));
        }
    }

    virtual chromid_t   id()     const { return m_chrom->id(); }
    virtual size_t      length() const { return m_chrom->length(); }
    virtual ploidy_t    ploidy() const { return PLOIDY; }

    virtual bool  allele( ploidy_t copy, size_t locus, allele_t & all ) {
        assert( copy < PLOIDY );

        size_t offset = 0;
        bool bIsLocus = m_chrom->is_locus( locus, offset );
        if( bIsLocus ) {
            all = m_seqs[ copy ]->allele( locus );
        }
        return bIsLocus;
    }

    virtual SequencePtr sequence( ploidy_t copy ) {
        assert( copy < PLOIDY );
        return m_seqs[ copy ];
    }

    virtual void genotype( const LocusPtr l, Genotype< P > & g ) {  
        size_t   pos = l->start;
        ploidy_t p = 0;
        g.bHomo = true;
        g.geno[p] = m_seqs[p]->allele(pos);
        while( ++p < PLOIDY ) {
            g.geno[p] = m_seqs[p]->allele(pos);
            g.bHomo = (g.bHomo && (g.geno[0] == g.geno[p]));
        }

        g.bDominant = (g.bHomo && (g.geno[0] == l->dominant_allele ));
    }

    virtual ~ChromosomeTuple() {}
protected:
    static const ploidy_t PLOIDY = P;
    ChromosomePtr   m_chrom;
    SequencePtr     m_seqs[ P ];
};

#endif  // CHROMOSOMETUPLE_H_
