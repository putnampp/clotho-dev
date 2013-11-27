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

#include "GeneticMap.h"

GeneticMap::GeneticMap() :
    m_loci( new Loci() ),
    m_genotypers( new vector< LocusGenotyper * >() ),
    m_trait_map( new TraitMap() ),
    m_traits( new Traits() )
{}

GeneticMap::~GeneticMap() {
    // delete all trait_loci mappings
    m_trait_map->clear();

    // delete all loci
    while( !m_loci->empty() ) {
        LocusPtr lp = m_loci->back();
        m_loci->pop_back();

        lp.reset();
    }

    // delete all traits
    m_traits->clear();
}

GeneticMap::locus_index_t GeneticMap::getLocusIndex( LocusPtr lp ) const {
    locus_index_t idx = 0;
    for( Loci::iterator it = m_loci->begin(); it != m_loci->end(); it++, ++idx ) {
        if( **it == *lp ) {
            break;
        }
    }
    return idx;
}

GeneticMap::trait_index_t GeneticMap::getTraitIndex( TraitPtr tp ) const {
    TraitMap::iterator it = m_trait_map->find(tp->getName());
    if( it == m_trait_map->end() ) {
        return getTraitCount();
    }
    return it->second;
}

/*
GeneticMap::locus_index_t GeneticMap::addLocus( LocusPtr lp ) {
    locus_index_t idx = getLocusIndex( lp );
    if( idx == m_loci->size() ) {
        m_loci->push_back( lp );
        m_genotypers->push_back( NULL );
    }

    return idx;
}
*/

GeneticMap::locus_index_t GeneticMap::addGenotyper( LocusGenotyper * lg, bool bShouldUpdate ) {
    locus_index_t idx = getLocusIndex( lg->getLocus() );
    if( idx == getLociCount() ) {
        // locus does not exist
        m_loci->push_back( lg->getLocus() );
        m_genotypers->push_back( lg );
    } else if( bShouldUpdate ) {
        // replace existing genotyper
        m_genotypers->at( idx ) = lg;
    }

    return idx;
}

GeneticMap::trait_index_t   GeneticMap::addTrait( TraitPtr t, bool bShouldUpdate ) {
    TraitMap::iterator it = m_trait_map->find(t->getName());
    if( it == m_trait_map->end() ) {
        trait_index_t idx = m_traits->size();

        (*m_trait_map)[ t->getName() ] = idx;
        m_traits->push_back( t );
        return idx;
    } else if( bShouldUpdate ) {
        m_traits->at( it->second )= t;
    }
    return it->second;
}

size_t GeneticMap::getLociCount() const {
    return m_loci->size();
}

size_t GeneticMap::getTraitCount() const {
    return m_traits->size();
}

bool GeneticMap::addTraitLocus( TraitPtr tp, LocusGenotyper * lg ) {
    locus_index_t lidx = getLocusIndex( lg->getLocus() );
    trait_index_t tidx = getTraitIndex( tp );

    if( lidx >= getLociCount() ) {
        lidx = addGenotyper( lg );
    }

    if( tidx >= getTraitCount() ) {
        tidx = addTrait( tp );
    }

    return m_traits->at( tidx )->addIndexedGenotyper( lidx, lg );
}

double GeneticMap::computeGenotype( locus_index_t l, const AlleleGroupPtr ag ) const {
    if( l < m_genotypers->size() && l < ag->size() ) {
        return m_genotypers->at(l)->genotype(ag->at(l));
    }

    return 0.0;
}

double GeneticMap::computePhenotype( TraitPtr t, const AlleleGroupPtr ag, const environmental * env ) const {
    TraitMap::iterator it = m_trait_map->find( t->getName() );

    if( it == m_trait_map->end()) {
        // trait dne
        return 0.0;
    }
    return computePhenotype( it->second, ag, env );
}

double GeneticMap::computePhenotype( trait_index_t t, const AlleleGroupPtr ag, const environmental * env ) const {
    if( t >= m_traits->size() ) return 0.0;

    if( !m_traits->at( t ) ) {
        return 0.0;
    }

    return m_traits->at( t )->phenotype( ag, env);
}

AlleleGroupPtr GeneticMap::createLociAlleles() const {
    AlleleGroupPtr la( new AlleleGroup() );
    la->reserve( m_loci->size() );
    return la;
}
