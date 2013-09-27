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
#include "Genotype.h"

struct chromosome_set {
    virtual chromid_t id()    const = 0;
    virtual size_t   length() const = 0;
    virtual ploidy_t ploidy() const = 0;

    virtual bool allele( ploidy_t copy, pos_t locus, allele_t & all ) = 0;
    virtual SequencePtr sequence( ploidy_t p ) = 0;
    virtual void    getGenotype( const LocusPtr l, genotype & g ) = 0;
};

class ChromosomeCopies : public chromosome_set {
public:
    ChromosomeCopies( const ChromosomePtr c, ploidy_t copies );

    virtual chromid_t   id()     const;
    virtual size_t      length() const;
    virtual ploidy_t    ploidy() const;

    virtual bool  allele( ploidy_t copy, pos_t locus, allele_t & all );

    virtual SequencePtr sequence( ploidy_t copy );

    virtual void getGenotype( const LocusPtr l, genotype & g );

    virtual ~ChromosomeCopies();
protected:
    ploidy_t        m_nPloid;
    ChromosomePtr   m_chrom;
    SequencePtr     *m_seqs;
};

typedef shared_ptr< chromosome_set > ChromosomeCopiesPtr;

#endif  // CHROMOSOMETUPLE_H_
