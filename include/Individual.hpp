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

#include "Sequence.h"

#include "Genotype.h"
#include "Phenotype.h"

#include "ChromosomeCopies.h"

enum SEX { MALE = 0, FEMALE, UNKNOWN_SEX };

typedef SEX    sex_t;

struct individual {
    virtual sex_t sex() const = 0;

    virtual chromid_t   chromosomes() const = 0;
    virtual ploidy_t    ploidy()    const = 0;

    virtual allele_t    allele( const LocusPtr l ) = 0;
};

class IndividualCreator;

/*******************************************************************************
 * An Individual is an encapsulation object consisting of a genotype and
 * phenotype.
 *
 ******************************************************************************/
class Individual : public Genotypeable, public Phenotypeable {
public:
    friend class GenomeFactory;

    virtual size_t   chromosomes() const;
    virtual sex_t sex() const;

    virtual SequencePtr getSequenceByID( chromid_t c, ploidy_t p );
    virtual SequencePtr getSequenceByIndex( size_t idx, ploidy_t p );

    virtual allele_t allele( const LocusPtr locus );

    virtual bool isHomozygous( const LocusPtr l );
    virtual bool isDominant( const LocusPtr l );
    virtual const genotype & operator[]( const LocusPtr l );

    virtual void phenotype( iTrait *, Phenotype * );

    virtual ~Individual() {}
protected:
    Individual( size_t chroms, ploidy_t copies = 1 );

    size_t m_nChroms;
    ploidy_t  m_nPloid;
    sex_t   m_sex;
    
    ChromosomeCopiesPtr  * m_seqs;
    genotype            * m_geno;
};

typedef shared_ptr< Individual > IndividualPtr;

class IndividualCreator {
public:
    virtual IndividualPtr create( istream & is ) = 0;
};

#endif  // INDIVIDUAL_H_
