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

#ifndef GENOTYPE_H_
#define GENOTYPE_H_

#include "common.h"
#include "ploidy.h"
#include "Allele.h"
#include "Locus.h"

enum GenotypeFlag { HOMOZYGOUS  = 1,
                     DOMINANT   = 2 };

typedef unsigned short flag_t;

class genotype {
public:
    virtual ploidy_t    ploidy() const = 0;
    virtual flag_t      getFlags() const = 0;
    virtual void        setFlag( GenotypeFlag f, bool bVal ) = 0;
    virtual bool        isFlag( GenotypeFlag f ) const = 0;

    virtual allele_t & operator[]( ploidy_t p ) = 0;

    virtual ~genotype() {}
protected:
    genotype(){}
};

/*
template < ploidy_t P >
struct Genotype : public genotype {
public:
    Genotype() : m_flags(0) {}

    ploidy_t ploidy() const {   return PLOIDY; }
    flag_t   getFlags() const { return m_flags; }
    void     setFlag( GenotypeFlag f, bool bVal = true ) { m_flags = ((bVal) ? (m_flags | f) : (m_flags & (~f))); }
    bool     isFlag( GenotypeFlag f ) const { return ((m_flags & f ) > 0); }

    allele_t & operator[]( ploidy_t p ) { assert( p < PLOIDY ); return m_geno[ p ]; }
    
    virtual ~Genotype() {}
protected:
    static const ploidy_t PLOIDY = P;
    flag_t      m_flags; 
    allele_t    m_geno[ PLOIDY ];
};
*/

struct Genotypeable {
    virtual bool isHomozygous( const LocusPtr l ) = 0;
    virtual bool isDominant( const LocusPtr l ) = 0;
    virtual const genotype & operator[]( const LocusPtr l ) = 0;
};

#endif  // GENOTYPE_H_
