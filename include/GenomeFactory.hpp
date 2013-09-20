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

#ifndef GENOME_H_
#define GENOME_H_

#include "common.h"
#include "ploidy.h"
#include "Chromosome.h"
#include "Sequence.h"

#include "Individual.hpp"

#include <vector>
#include <map>
using std::vector;
using std::map;
using std::make_pair;


/***
 *
 * GenomeFactory maintains the set of chromosomes in an indexed
 * array.
 *
 */
class GenomeFactory {
public:
    typedef map< chromid_t, ChromosomePtr > Chromosomes;
    typedef Chromosomes::const_iterator ChromosomeIter;

    GenomeFactory ( );
    GenomeFactory( size_t chroms );
    GenomeFactory( const vector< ChromosomePtr > & c );

    virtual IndividualPtr createIndividual( ploidy_t p );

    virtual bool addChromosomeSite( chromid_t c, size_t posi, bool bByIndex = true );
//    virtual SequencePtr build_sequence( chromid_t c );

    virtual ChromosomePtr getChromosomeByID( chromid_t c ) const;
    virtual ChromosomePtr getChromosomeByIndex( size_t ) const;

    virtual bool isChromosome( chromid_t c );

    virtual size_t chromosomes() const;
    virtual size_t size() const;
    virtual size_t loci() const;

    virtual ChromosomeIter begin() const;
    virtual ChromosomeIter end() const;

    virtual void reset();

    virtual ~GenomeFactory();
protected:
    size_t          m_nBases;
    size_t          m_nLoci;
    Chromosomes     m_chroms;
};

#endif  // GENOME_H_
