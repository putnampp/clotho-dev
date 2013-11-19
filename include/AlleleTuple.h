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
#ifndef ALLELETUPLE_H_
#define ALLELETUPLE_H_

#include "ploidy.h"
#include "Allele.h"

#include <vector>
#include <ostream>
#include <cstring>

using std::vector;
using std::ostream;
using std::memcpy;

struct allele_tuple {
    virtual ploidy_t ploidy() const = 0;

    virtual allele_t & operator[]( ploidy_t p ) = 0;
    virtual allele_t operator[]( ploidy_t p ) const = 0;

    virtual void print( ostream & out ) const = 0; 
};

template < ploidy_t P >
struct AlleleTuple : public allele_tuple {
public:
    static const ploidy_t PLOIDY = P;
    AlleleTuple( const vector< allele_t > & alleles) {
        vector< allele_t >::const_iterator it = alleles.begin();
        ploidy_t p = 0;
        while( it != alleles.end() && p < PLOIDY ) {
            m_alleles[ p++ ] = (*it++);
        }
    }

    AlleleTuple( const AlleleTuple< P > & alleles ) {
        memcpy( m_alleles, alleles.m_alleles, sizeof(allele_t) * P);
    }

    ploidy_t ploidy() const {   return PLOIDY; }

    allele_t & operator[]( ploidy_t p ) {
        assert( p < PLOIDY ); return m_alleles[ p ]; 
    }

    allele_t operator[]( ploidy_t p ) const {
        assert( p < PLOIDY );
        return m_alleles[ p ];
    }

    void print( ostream & out ) const {
        if( !PLOIDY ) return;

        ploidy_t p = 0;
        out << (int) m_alleles[ p++ ];
        while( p < P ) {
            out << ":" << (int) m_alleles[p++];
        }
    }
    virtual ~AlleleTuple() {}
protected:
    allele_t    m_alleles[ PLOIDY ];
};

/*
struct allelic_tuple_weight : public allelic_effect {
    
    /// map the allele_tuple to an allelic effect tuple (vector)
    virtual shared_ptr< double [] > operator()( const allele_tuple * alleles ) = 0;

};
*/
typedef vector< allele_tuple * >                AlleleGroup;

#endif  // ALLELETUPLE_H_
