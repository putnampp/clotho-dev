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

#include "Individual.hpp"

#include <iostream>
using std::cout;
using std::endl;

Individual::Individual( const GenomeFactory * gf, ploidy_t ploid ) 
    : m_nChroms( gf->chromosomes() ), m_nPloid( ploid ) {
    initialize(gf);
}

void Individual::initialize( const GenomeFactory * gf) {
    m_seqs = new ChromosomeTuplePtr[ m_nChroms ];
    size_t i = 0;
    for( GenomeFactory::ChromosomeIter it = gf->begin(); it != gf->end(); it++ ) {
        m_seqs[ i++ ].reset( new ChromosomeTuple( it->second , m_nPloid ) );
    }
}

chromid_t   Individual::chromosomes() const {
    return m_nChroms;
}

sex_t   Individual::sex() const {
    return m_sex;
}

SequencePtr Individual::getSequenceByID( chromid_t c, ploidy_t p ) {
    return m_seqs[ c ]->sequence( p );
}

SequencePtr Individual::getSequenceByIndex( size_t idx, ploidy_t p ) {
    return ((idx < m_nChroms) ? m_seqs[idx]->sequence(p) : NULL_SEQUENCE);
}

allele_t    Individual::allele( const LocusPtr l ) {
    allele_t all = 0;
    return m_seqs[ l->chrom ]->allele( l->ploid, l->start, all );
}

const genotype & Individual::operator[]( const LocusPtr l ) {
    m_seqs[ l->chrom ]->getGenotype( l, *m_geno );
    return *m_geno;
}

bool Individual::isHomozygous( const LocusPtr l ) {
    m_seqs[ l->chrom ]->getGenotype( l, *m_geno );
    return m_geno->isFlag( HOMOZYGOUS );
}

bool Individual::isDominant( const LocusPtr l ) {
    m_seqs[ l->chrom ]->getGenotype( l, *m_geno );
    return m_geno->isFlag( DOMINANT );
}

void Individual::phenotype( iTrait *, Phenotype * ) {
}
