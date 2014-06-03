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

#ifndef GENETICMAP_H_
#define GENETICMAP_H_

#include "common.h"
#include "Locus.h"
#include "Trait.h"
#include "Environmental.h"

#include "Genotype.h"

#include <map>

using std::map;
using std::pair;
using std::make_pair;

/**
 *
 * A GeneticMap is intended to retain the Loci information for the current
 * simulation.
 *
 * One locus can affect multiple Phenotypic traits (pleitropy). However, multiple
 * traits need not necessarily contain the same set of Loci.
 *
 */
class GeneticMap {
public:
    typedef shared_ptr< GeneticMap > Ptr;
    typedef size_t  locus_index_t;
    typedef size_t  trait_index_t;

    typedef map< chromid_t, size_t > chrom_offset_map_t;

    GeneticMap();

    locus_index_t getLocusIndex( Locus::Ptr lp ) const;
    trait_index_t getTraitIndex( Trait::Ptr tp) const;

    /// assumes only one method of computing genotype per locus
    locus_index_t addGenotyper( LocusGenotyper * lg, bool bShouldUpdate = false);

    /// multilocus genotyper responsible for weighting each genotype
    /// relative to the current set of loci
    trait_index_t addTrait( Trait::Ptr tp, bool bShouldUpdate = false );

    bool addTraitLocus( Trait::Ptr tp, LocusGenotyper * lg );

    virtual double computeGenotype( locus_index_t locus, const AlleleGroupPtr la ) const;

    virtual double computePhenotype( Trait::Ptr t, const AlleleGroupPtr ag, const environmental * env ) const;
    virtual double computePhenotype( trait_index_t trait, const AlleleGroupPtr ag, const environmental * env ) const;

    size_t getLociCount() const;
    size_t getTraitCount() const;

    size_t getMaxAlleles( locus_index_t idx ) const;

    AlleleGroupPtr createLociAlleles() const;

    size_t loci_per_chromosome( chromid_t id ) const;

    size_t chromosome_count() const;
    chrom_offset_map_t::const_iterator begin_offsets() const;
    chrom_offset_map_t::const_iterator end_offsets() const;

    virtual ~GeneticMap();
protected:
    shared_ptr< Loci >              m_loci;

    chrom_offset_map_t                m_chrom_offsets;

    shared_ptr< vector< LocusGenotyper * > > m_genotypers;
    
    typedef unordered_map< string, trait_index_t > TraitMap;
    shared_ptr< TraitMap >            m_trait_map;

    shared_ptr< Traits >  m_traits;
};

#endif  // GENETICMAP_H_
