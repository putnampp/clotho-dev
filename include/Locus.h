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

#ifndef LOCUS_H_
#define LOCUS_H_

#include "common.h"
#include "Allele.h"
//#include "Mutate.h"
//#include "Genotype.h"

#include <vector>
#include <unordered_map>
#include <functional>

using std::vector;
using std::unordered_map;

class locus_id {
public:
    static unsigned long createID( chromid_t c, pos_t s, pos_t e );
    locus_id( chromid_t c, pos_t s, pos_t e );

    unsigned long getKey() const;
    chromid_t  getChromosome() const;
    pos_t       getStart() const;
    pos_t       getEnd() const;

    bool operator==( const locus_id & rhs ) const;

    virtual ~locus_id();
protected:
    unsigned long m_id;
};

/**
 * Implementation specific definition of a locus structure
 *
 * From a simulation perspective only the the pair <chromosome, position> are important. For extensibility reasons, position is
 * considered to be an open ended region [start, end)
 * All other fields are generally optional, depending upon simulation models.
 *
 * Other optional properties are:
 *  - properties (name, alleles, ... )
 *
 */
class Locus {
public:
    Locus( chromid_t c, pos_t s, pos_t e, allele_t nAlleles );

    const locus_id & getID()    const;
    chromid_t   getChromosome() const;
    pos_t       getStart()      const;
    pos_t       getEnd()        const;

    allele_t    getMaxAlleles() const;

    virtual bool isKnownAllele( allele_t all ) const;

    bool operator==( const Locus & rhs ) const;

    virtual ~Locus();
protected:
    locus_id    m_id;
    allele_t    m_nAlleles;
};

typedef shared_ptr< Locus > LocusPtr;
typedef vector< LocusPtr > Loci;
typedef Loci::iterator  LocusIterator;

namespace std {

template <>
struct hash< boost::shared_ptr< Locus > > {
    std::hash< unsigned long > hasher;
    size_t operator()( const boost::shared_ptr< Locus > & lp ) const {
        return hasher( lp->getID().getKey() );
    }
};

template <>
struct equal_to< boost::shared_ptr< Locus > > {
    bool operator()( const boost::shared_ptr< Locus > & lhs, const boost::shared_ptr< Locus > & rhs ) const {
        return ( lhs == rhs || *lhs == *rhs);
    }
};
}

typedef std::unordered_map< LocusPtr, size_t >  LocusIndexMap;

#endif  // LOCUS_H_
