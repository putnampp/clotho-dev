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

#ifndef GENERALGENETICMAP_H_
#define GENERALGENETICMAP_H_

#include "common.h"
//#include "Locus.h"
//#include "Trait.h"
#include "Environmental.h"

#include "Genotype.h"

//#include "locus_copy_genotyper.h"
//#include "multilocus_copy_genotyper.h"
//
#include "Phenotyper.h"

#include <map>

/**
 *
 * A GeneticMap is intended to retain the Loci information for the current
 * simulation.
 *
 * One locus can affect multiple Phenotypic traits (pleitropy). However, multiple
 * traits need not necessarily contain the same set of Loci.
 *
 */
template < ploidy_t P >
class GeneralGeneticMap {
public:
    typedef size_t  locus_index_t;
    typedef size_t  trait_index_t;

    typedef Phenotyper< P >                         phenotyper;
    typedef phenotyper::multilocus_genotyper        multilocus_genotyper;
    typedef multilocus_genotyper::locus_genotyper   locus_genotyper;
    typedef multilocus_genotyper::allele_group      allele_group;
    typedef multilocus_genotyper::allele_copy       allele_copy;
    typedef allele_group::copy_group_ptr            copy_group_ptr;

    typedef LocusIndexMap::const_iterator locus_iterator;
    typedef TraitIndexMap::const_iterator trait_iterator;

    GeneralGeneticMap() :
        m_loci( new LocusIndexMap() ),
        m_traits( new TraitIndexMap() ) {}

    locus_index_t getLocusIndex( Locus::Ptr lp ) const {
        locus_iterator it = m_loci->find( lp );
        if( it == m_loci->end() ) {
            return getLociCount();
        }
        return it->second;
    }

    Locus::Ptr getLocus( const locus_id & id ) {
        locus_iterator it = m_loci->find( lp );
        if( it == m_loci->end() ) {
            return Locus::Ptr();
        }
        return it->first;
    }

    trait_index_t getTraitIndex( TraitPtr tp ) const {
        trait_iterator it = m_traits->find( tp );
        if( it == m_traits->end() ) {
            return getTraitCount();
        }
        return it->second;
    }

    TraitPtr getTrait( const string & name ) const {
        TraitPtr tmp( new Trait( name ) );
        trait_iterator it = m_traits->find( tmp );

        if( it == m_traits->end() ) {
            return TraitPtr();
        }
        return it->first;
    }

    locus_index_t addLocus( Locus::Ptr lp, bool bShouldUpdate = false ) {
        locus_iterator it = m_loci->find( lp );
        locus_index_t idx = m_loci->size();

        if( it == m_loci->end() ) {
            // new locus
            m_loci->insert( std::make_pair( lp, idx ) );
            m_genotypers->push_back( NULL );
        } else {
            idx = it->second;

            if( bShouldUpdate ) {
                m_loci->erase( it );
                m_loci->insert( std::make_pair( lp, idx ) );
            }
        }

        return idx;
    }

    /// assumes only one method of computing genotype per locus
    locus_index_t addGenotyper( locus_genotyper * lg, bool bShouldUpdate = false) {
        locus_iterator it = m_loci->find( lg->getLocus() );

        locus_index_t idx = getLociCount();
        if( it == m_loci->end() ) {
            // new locus
            m_loci->insert( std::make_pair( lg->getLocus(), idx ) );
            m_genotypers->push_back( lg );
        } else {
            // known locus
            idx = it->second;
            if( bShouldUpdate ) {
                m_genotypers->at(idx) = lg;
            }
        }

        return idx;
    }

    /// multilocus genotyper responsible for weighting each genotype
    /// relative to the current set of loci
    trait_index_t addTrait( TraitPtr tp, bool bShouldUpdate = false ) {
        trait_iterator it = m_traits->find( tp );
        trait_index_t idx = m_traits->size();
        if( it == m_traits->end() ) {
            // unknown trait
            m_traits->insert( std::make_pair( tp, idx) );
            m_phenotypers->push_back( NULL );
        } else {
            idx = it->second;
            if( bShouldUpdate ) {
                m_traits->erase( it );
                m_traits->insert( std::make_pair( tp, idx ) );
            }
        }

        return idx;
    }

    trait_index_t addPhenotyper( phenotyper * pheno, bool bShouldUpdate = false ) {
        trait_iterator it = m_traits->find( pheno->getTrait() );
        trait_index_t idx = getTraitCount();
        if( it == m_traits->end() ) {
            // new trait
            m_traits->insert( std::make_pair( pheno->getTrait(), idx ) );
            m_phenotypers->push_back( pheno );
        } else if( bShouldUpdate ) {
            m_phenotypers->at( it->second ) = pheno;
        }

        return idx;
    }

    bool addTraitLocus( TraitPtr tp, Locus::Ptr  lp ) {
        locus_index_t lidx = getLocusIndex( lp );
        trait_index_t tidx = getTraitIndex( tp );

        if( lidx >= getLociCount() ) {
            return false;
        }

        if( tidx >= getTraitCount() ) {
            return false;
        }

        phenotyper * p = m_phenotypers[tidx];

        if( p != NULL )
            return p->addIndexedGenotyper( lidx );

        return false;
    }


    virtual double computeGenotype( locus_index_t locus, const copy_group_ptr la ) const;

    double computePhenotype( const string & trait, const copy_group_ptr ag, const environmental * env ) const;
    double computePhenotype( TraitPtr t, const copy_group_ptr ag, const environmental * env ) const;
    virtual double computePhenotype( trait_index_t trait, const copy_group_ptr ag, const environmental * env ) const;

    inline size_t getLociCount() const {
        return m_loci->size();
    }
    inline size_t getTraitCount() const {
        return m_trait->size();
    }

    copy_group_ptr createLociAlleles() const {
        return allele_group::createAlleleCopyGroup( m_loci );
    }

    virtual ~GeneralGeneticMap() {
        m_loci->clear();
        m_loci.reset();

        m_traits->clear();
        m_traits.reset();
    }

protected:
    shared_ptr< LocusIndexMap >       m_loci;
    shared_ptr< TraitIndexMap >       m_traits;

    shared_ptr< vector< locus_genotyper * > > m_genotypers;
    shared_ptr< vector< phenotypers * > > m_phenotypers;
};

/*******************************************************************************
 * General Implementation
 ******************************************************************************/

#endif  // GENETICMAP_H_
