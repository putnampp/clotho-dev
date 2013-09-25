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

#include "ChromosomeCopies.h"

ChromosomeCopies::ChromosomeCopies( const ChromosomePtr c, ploidy_t copies ) : m_nPloid( copies ), m_chrom(c) {
    m_seqs = new SequencePtr[ m_nPloid ];
    for( ploidy_t p = 0; p < m_nPloid; ++p ) {
        m_seqs[p].reset(new Sequence( m_chrom->loci() ));
    }
}

ChromosomeCopies::~ChromosomeCopies() {
    delete [] m_seqs;
}

chromid_t ChromosomeCopies::id() const {
    return m_chrom->id();
}

size_t  ChromosomeCopies::length() const {
    return m_chrom->length();
}

ploidy_t ChromosomeCopies::ploidy() const {
    return m_nPloid;
}

bool    ChromosomeCopies::allele( ploidy_t copy, size_t pos, allele_t & all ) {
    assert( copy < m_nPloid );

    size_t offset = 0;
    bool bIsLocus = m_chrom->is_locus( pos, offset );
    if( bIsLocus ) {
        all = m_seqs[ copy ]->allele( pos );
    }
    return bIsLocus;
}

SequencePtr ChromosomeCopies::sequence( ploidy_t copy ) {
    assert( copy < m_nPloid );
    return m_seqs[ copy ];
}

void ChromosomeCopies::getGenotype( const LocusPtr l, genotype & g ) {
    size_t   pos = l->start;
    ploidy_t p = 0;
    bool bHomo = true;
    g[p] = m_seqs[p]->allele(pos);
    while( ++p < m_nPloid ) {
        g[p] = m_seqs[p]->allele(pos);
        bHomo = (bHomo && (g[0] == g[p]));
    }
    g.setFlag(HOMOZYGOUS, bHomo );
    g.setFlag( DOMINANT, (bHomo && (g[0] == l->dominant_allele )));
}
