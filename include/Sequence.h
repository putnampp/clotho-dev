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

struct sequence {
    virtual size_t      length() const = 0;
    virtual allele_t &  allele( size_t l ) = 0;
};

/**
 * A sequence is an in-silico representation of a contiguous vector of 
 * allelic values. Each value corresponds to a specific locus.
 *
 * Each locus is associated with an enumerated set of forms.
 * The allelic values of the sequence are indices into the enumerated set
 * of forms.
 *
 * The allele_t of the a sequence is intended to be an integral
 * type with sufficient bits to represent the index.
 *
 * A default sequence allows for MAX_ALLELES forms per loci, thus a
 * byte (unsigned char) is sufficient to represent all possible
 * form indices.
 *
 **/
class Sequence : public sequence {
public:
//    Sequence( );
    Sequence( size_t loci );
    Sequence( shared_ptr< allele_t [] > alleles, size_t size );

    virtual size_t length() const;
    virtual allele_t & allele(size_t locus);

    virtual ~Sequence();

protected:
    shared_ptr< allele_t [] >   m_alleles;

    size_t       m_nLoci;
    size_t       m_maxForms;
    size_t    m_allocatedLoci;

    void    resizeSeq( size_t nLoci );
};

typedef shared_ptr< Sequence > SequencePtr;

#endif  // SEQUENCE_H_
