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

#ifndef INDIVIDUAL_H_
#define INDIVIDUAL_H_

#include "common.h"
#include "Allele.h"

#include "Genome.h"
#include "Sequence.h"

#include "Genotype.h"
#include "Phenotype.h"

/*******************************************************************************
 * An Individual is an encapsulation object consisting of a genotype and
 * phenotype.
 *
 *
 ******************************************************************************/
template < chromid_t C, ploidy_t P >
class Individual : public Genotypeable< P >, public Phenotypeable {
public:
    Individual() {}

    virtual bool sex();

    virtual allele_t allele( const LocusPtr locus );

    virtual bool isHomozygous( const LocusPtr l );
    virtual bool isDominant( const LocusPtr l );
    virtual void genotype( const LocusPtr l, Genotype< P > & g );

    virtual void phenotype( iTrait *, Phenotype * );

    virtual ~Individual() {}
protected:
    virtual void inspectLocus( const LocusPtr l, Genotype< P > & g );

private:
    static const chromid_t CHROMOSOMES = C;
    static const ploidy_t PLOIDY = P;

    Sequence *  m_seqs[ PLOIDY ][ CHROMOSOMES ];
    Genotype< P >   m_geno;
};

/**
 *
 * IMPLEMENTATION
 *
 */

#define IND_MEMBER_DECL(t, f) template < chromid_t C, ploidy_t P > t Individual<C,P>::f

IND_MEMBER_DECL(bool, sex)() {
    return false;
}

IND_MEMBER_DECL( allele_t, allele)( const LocusPtr locus ) {
    assert( locus->chrom < CHROMOSOMES && locus->ploid < PLOIDY );
    return m_seqs[locus->ploid][locus->chrom]->allele(locus->start);
}

IND_MEMBER_DECL( bool, isHomozygous)( const LocusPtr locus ) {
    inspectLocus( locus, m_geno );
    return m_geno.bHomo;
}

IND_MEMBER_DECL( bool, isDominant )( const LocusPtr locus ) {
    inspectLocus( locus, m_geno );
    return m_geno.bDominant;
}

IND_MEMBER_DECL( void, inspectLocus)( const LocusPtr l, Genotype< P > & g ) {
    assert( l->chrom < CHROMOSOMES );
    g.bHomo = true;
    chromid_t c = l->chrom;
    size_t    pos = l->start;
    allele_t all = m_seqs[0][c]->allele(pos);
    for( ploidy_t p = 1; p < PLOIDY; ++p ) {
        g.geno[p] = m_seqs[p][c]->allele(pos);
        g.bHomo = (g.bHomo && all == g.geno[p]);
    }
    g.bDominant = (g.bHomo && all == l->dominant_allele);
}



IND_MEMBER_DECL( void, genotype)( const LocusPtr locus, Genotype< P > & g ) {
    inspectLocus( locus, g );
}

IND_MEMBER_DECL( void, phenotype)( iTrait * trait, Phenotype * p ) {
    
} 

#endif  // INDIVIDUAL_H_
