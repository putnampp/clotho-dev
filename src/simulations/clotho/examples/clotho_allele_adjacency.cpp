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

#include "clotho.h"
#include "clotho_commandline.h"
#include <cstdlib>
#include <functional>
#include <algorithm>

#include <vector>
#include <memory>

#include "utility/simulation_stats.h"

#include "object/individual.hpp"
#include "object/individual_properties.hpp"

#include "genetics/locus_bitset.h"

#include "rng/rng.hpp"
//#include "models/default_life_cycle.hpp"
#include "models/selection_models.hpp"
//#include "models/reproduction_models.hpp"

//#include "processes.hpp"
#include "processes/reproduction_models.hpp"

#define _PLOIDY 2

using std::shared_ptr;

namespace finalizer {

template < unsigned char P, class S >
class FinalWorker< individual_props< locus_bitset, P, S > > {
public:
    static void finalize( individual_props< locus_bitset, P, S > * props ) {
        for( unsigned char i = 0; i < P; ++i ) {
            if( props->m_gametes[i].second != NULL ) {
                props->m_gametes[i].second->release();
                props->m_gametes[i].second = NULL;
            }
        }
        props->m_free_gametes = P;
    }
};

}

template < >
struct locus_generator< double, RandomProcess::rng_pointer > {
    typedef double return_type;
    typedef RandomProcess::rng_pointer parameter_type;

    locus_generator() : m_params( RandomProcess::getRNG() ) {}
    locus_generator( parameter_type p ) : m_params( p ) {}

    return_type operator()() {
        return m_params->nextUniform();
    }
    parameter_type m_params;
};

struct infinite_site {};

//#define SYM_SPEC symbol_generator< PopulationAlphabet::locus_t, PopulationAlphabet::allele_t, PopulationAlphabet::index_type, PopulationAlphabet >

//template <> template <>
//SYM_SPEC::symbol_type SYM_SPEC::operator()< infinite_site >( PopulationAlphabet * alpha, infinite_site * inf) {
//    static locus_generator< PopulationAlphabet::locus_t, RandomProcess::rng_pointer> lgen;
//    static allele_generator< PopulationAlphabet::allele_t, void > agen;

//    PopulationAlphabet::locus_t l = lgen();
//    while( alpha->isLocus( l ) ) {
//        l = lgen();
//    }

//    return alpha->getSymbol( l, agen(), true );
//    return alpha->getSymbol(lgen(), agen(), true );
//}

#undef SYM_SPEC

#define SYM_SPEC symbol_generator< AlleleAlphabet::locus_t, AlleleAlphabet::allele_t, AlleleAlphabet::index_type, AlleleAlphabet >

template <> template <>
SYM_SPEC::symbol_type SYM_SPEC::operator()< infinite_site >( alphabet_pointer alpha, infinite_site * inf) {
    static locus_generator< AlleleAlphabet::locus_t, RandomProcess::rng_pointer> lgen;
    static allele_generator< AlleleAlphabet::allele_t, void > agen;

    SYM_SPEC::symbol_type res = alpha->getSymbol(lgen(), agen(), true );
    while( res == AlleleAlphabet::npos ) {
        res = alpha->getSymbol(lgen(), agen(), true );
    }
    return res;
}
#undef SYM_SPEC

#include "locus_bitset_mutate.tcc"

typedef locus_bitset gamete_type;
typedef typename gamete_type::alphabet_t    alphabet_type;
typedef typename gamete_type::allele_type   allele_type;

typedef allele_type *   allele_pointer;
typedef gamete_type *   gamete_pointer;

typedef typename gamete_type::adjacency_iterator       adjacency_iterator;

typedef reproduction::models::mutation::mutate_site< allele_type, alphabet_type, gamete_type >   mmodel_type;
typedef reproduction::models::recombination::no_recomb< _PLOIDY >     rcmodel_type;
typedef reproduction::IndividualReproduction< mmodel_type, rcmodel_type > rmodel_type;

typedef TIndividual< gamete_type, _PLOIDY, system_id, individual_props< gamete_type, _PLOIDY, system_id >> individual_type;
typedef individual_type  *                  individual_pointer;
typedef std::vector< individual_pointer >   environment_type;


class DiscreteSelector {
public:
    DiscreteSelector( gsl_rng * r, double * fitnesses, size_t s ) :
        m_rng( r ),
        m_lookup( NULL ) {
        m_lookup = gsl_ran_discrete_preproc( s, fitnesses );
    }

    size_t operator()() {
        return gsl_ran_discrete( m_rng, m_lookup );
    }

    std::pair< individual_pointer , individual_pointer > operator()( environment_type * env, double f = 0.0 ) {
        size_t i0 = gsl_ran_discrete( m_rng, m_lookup );
        size_t i1 = ((gsl_rng_uniform(m_rng) <= f ) ? i0 : gsl_ran_discrete( m_rng, m_lookup ));

        std::pair< individual_pointer, individual_pointer > res = make_pair( env->at(i0), env->at(i1));

        return res;
    }

    virtual ~DiscreteSelector() {
        gsl_ran_discrete_free( m_lookup );
    }
protected:
    gsl_rng * m_rng;
    gsl_ran_discrete_t * m_lookup;
};

class ReproduceWithRecombination : public RandomProcess {
public:
    ReproduceWithRecombination( double mu, double rho ) : m_mu(mu), m_rho(rho), m_nRecomb(0), m_nRecombEvents(0), m_nMut(0), m_nMutEvents(0) {}

    size_t getRecombinationCalls() {
        return m_nRecomb;
    }
    size_t getRecombinationEvents() {
        return m_nRecombEvents;
    }

    size_t getMutationCalls() {
        return m_nMut;
    }
    size_t getMutationEvents() {
        return m_nMutEvents;
    }

    gamete_pointer operator()( individual_pointer ind ) {
        unsigned int nMut = m_rng->nextPoisson( m_mu );
        unsigned int nRec = 0;
        if( ind->getProperties()->getGamete(0) != ind->getProperties()->getGamete(1) ) {
            nRec = m_rng->nextPoisson( m_rho );
        } else if( nMut == 0 ) {
            // both gametes are the same
            // AND no new mutations will be added
            // hence just return a copy
            return ind->getProperties()->getGamete(0)->copy();
        }

        gamete_pointer base_gamete, other_gamete;
        if( m_rng->nextBool() ) {
            base_gamete = ind->getProperties()->getGamete(0);
            other_gamete = ind->getProperties()->getGamete(1);
        } else {
            base_gamete = ind->getProperties()->getGamete(1);
            other_gamete = ind->getProperties()->getGamete(0);
        }

        gamete_pointer res = base_gamete;
        bool copied = true;

        if( nRec > 0 ) {
            ++m_nRecomb;
            m_nRecombEvents += nRec;

            gamete_type::bitset_type symm_diff, 
                                    base_copy( *base_gamete->getBits() ), 
                                    other_copy( *other_gamete->getBits() );

            if( base_copy.size() >= other_copy.size() ) {
                other_copy.resize( base_copy.size(), false);
            } else {
                base_copy.resize( other_copy.size(), false );
            }
            symm_diff = ( base_copy ^ other_copy);

            copied = false;
            gamete_type::bitset_type mask( symm_diff );
            mask.flip();

            static locus_generator< AlleleAlphabet::locus_t, RandomProcess::rng_pointer> lgen;
            typedef std::vector< AlleleAlphabet::locus_t > recombination_points;
            typedef typename recombination_points::iterator recombination_iterator;
            recombination_points rec_points(nRec, 0.0);
            do {
                rec_points[ --nRec ] = lgen();
            } while( nRec );
            rec_points.push_back(0.0);
            rec_points.push_back(1.0);
            std::sort( rec_points.begin(), rec_points.end() );

            gamete_type::alphabet_t::pointer alpha = base_gamete->getAlphabet();
            gamete_type::bitset_type::size_type offset = symm_diff.find_first();
            while( offset != gamete_type::bitset_type::npos ) {
                recombination_iterator rit = std::lower_bound( rec_points.begin(), rec_points.end(), (*alpha)[offset]->first );
                if( (rit - rec_points.begin()) % 2 ) {
                    symm_diff[offset] = other_copy[offset];
                } else {
                    symm_diff[offset] = base_copy[offset];
                }
                offset = symm_diff.find_next(offset);
            }

            symm_diff |= (base_copy & mask );

            if( nMut == 0) {
                if( symm_diff == base_copy ) {
                    return base_gamete->copy();
                } else if( symm_diff == other_copy ) {
                    return other_gamete->copy();
                } else {
                    res = new gamete_type( symm_diff, alpha );
                    return res;
                }
            } else {
                res = new gamete_type( symm_diff, alpha );
            }
        }

        if( nMut > 0 ) {
            ++m_nMut;
            m_nMutEvents += nMut;
            if( copied ) {
                res = res->clone();
            }

            do {
                typedef symbol_generator< AlleleAlphabet::locus_t, AlleleAlphabet::allele_t, AlleleAlphabet::index_type, AlleleAlphabet > sgen_type;
                typedef typename sgen_type::symbol_type symbol_type;

                static sgen_type sgen;
                symbol_type s = sgen( res->getAlphabet(), (infinite_site * ) NULL );
                //std::cout << "Adding variant: " << s << std::endl;
                res->addVariant( s );
            } while( --nMut );

        } else if( copied ) {
            res = res->copy();
        }

        return res;
    }

    void updateHeterozygous( gamete_type::bitset_type & symm_diff, gamete_type::bitset_type & mask ) {

    }

    virtual ~ReproduceWithRecombination() {}
protected:
    double m_mu, m_rho;
    size_t m_nRecomb, m_nRecombEvents;
    size_t m_nMut, m_nMutEvents;
};

class SimpleSelector : public RandomProcess {
public:
    static std::pair< individual_pointer , individual_pointer > select( environment_type * env ) {
        std::pair< individual_pointer, individual_pointer > res;

        size_t nInd = env->size();

        size_t i0 = m_rng->nextInt( nInd );
        size_t i1 = i0;

        while( i1 == i0 ) {
            i1 = m_rng->nextInt( nInd );
        }

        res.first = env->at( i0 );
        res.second = env->at( i1 );

        return res;
    }
};

typedef SimpleSelector selector_t;

struct het_fitness {
    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->dominance * v->selection);
    }

    inline void operator()( double & f, const allele_type & v ) {
//        std::cout << v.dominance << std::endl;
//        std::cout << v.selection << std::endl;
        f *= (1. + v.dominance * v.selection);
    }
};

struct hom_fitness {
    hom_fitness( double s = 1. ) : m_scaling(1.) {}
    inline void operator()( double & f, allele_pointer v ) {
        f *= (1. + v->selection * m_scaling);
    }

    inline void operator()( double & f, const allele_type & v ) {
        f *= (1. + v.selection * m_scaling);
    }

    double m_scaling;
};

template < class het_policy, class hom_policy >
class fitness_multiplicative {
public:
//    typedef void (*update_policy)( double & f, allele_pointer v );
//
    fitness_multiplicative() {}

    fitness_multiplicative( het_policy & het, hom_policy & hom ) :
        m_het_case(het),
        m_hom_case(hom) {
    }

    double operator()( double f, individual_pointer ind ) {
        return (*this)(f, ind->getProperties()->getGamete(0), ind->getProperties()->getGamete(1) );
    }

/*    double operator()( double f, gamete_pointer g1, gamete_pointer g2 ) {
        double res = f;
        if( g1 == g2 ) return res;

//        unsigned int s = g1->size() + g2->size();
//        if( s == 0 ) return res;

//        std::cout << "G1 has " << g1->size() << std::endl;
//        std::cout << "G2 has " << g2->size() << std::endl;

        adjacency_iterator g1_it = g1->begin(), g1_e = g1->end();
        adjacency_iterator g2_it = g2->begin(), g2_e = g2->end();

        assert( g1_it.comparable( g2_it ) );

        while( g1_it != g1_e && g2_it != g2_e ) {
            if( g1_it == g2_it )  {
                m_hom_case( res, *((*(*g1_it++)).second.first));
                g2_it++;
            } else if( g1_it < g2_it ) {
                m_het_case( res, *((*(*g1_it++)).second.first) );
            } else {
                m_het_case( res, *((*(*g2_it++)).second.first) );
            }
        }
        while( g1_it != g1_e ) {
            m_het_case(res, *((*(*g1_it++)).second.first));
        }

        while( g2_it != g2_e ) {
            //assert( (*(*g2_it)).second.first != AlleleAlphabet::getInstance()->end_allele());
            //assert( (*g2_it) != AlleleAlphabet::getInstance()->end_db());
            //allele_type at( *((*(*g2_it++)).second.first) );
            m_het_case(res, *((*(*g2_it++)).second.first));
        }

        return res;
    }*/

    double operator()( double f, gamete_pointer g1, gamete_pointer g2 ) {
        double res = f;
        if( g1 == g2 ) return res;

        gamete_type::alphabet_t::pointer alpha = g1->getAlphabet();
        gamete_type::bitset_type g1_copy( *(g1->getBits()) ), g2_copy( *(g2->getBits()));
        if( g1_copy.size() >= g2_copy.size() ) {
            g1_copy.resize( g2_copy.size(), false);
        } else {
            g2_copy.resize( g1_copy.size(), false);
        }

        gamete_type::bitset_type tmp = (g1_copy & g2_copy); // homozygous of gamete 1 and 2
        gamete_type::bitset_type::size_type pos = tmp.find_first();

        while( pos != gamete_type::bitset_type::npos ) {
            m_hom_case( res, *(*alpha)[pos]->second.first);
            pos = tmp.find_next(pos);
        }

        tmp = g1_copy - g2_copy;    // heterozygous of gamete 1
        pos = tmp.find_first();
        while( pos != gamete_type::bitset_type::npos ) {
            m_het_case( res, *(*alpha)[pos]->second.first );
            pos = tmp.find_next(pos);
        }

        tmp = g2_copy - g1_copy;    // heterozygous of gamete 2
        pos = tmp.find_first();
        while( pos != gamete_type::bitset_type::npos ) {
            m_het_case( res, *(*alpha)[pos]->second.first );
            pos = tmp.find_next(pos);
        }

        return res;
    }

    virtual ~fitness_multiplicative() {}
protected:
    het_policy m_het_case;
    hom_policy m_hom_case;

};

int main( int argc, char ** argv ) {

    po::variables_map vm;
    if( !parse_commandline( argc, argv, vm ) ) {
        return 0;
    }

    string engine_config( "" );
    string clotho_config( "" );

    SystemClock::vtime_t tUntil = vm[ SIM_UNTIL_K ].as< SystemClock::vtime_t >();
    unsigned int nGen = vm[ GENERATIONS_K ].as< unsigned int >();

    if( nGen != (unsigned int) -1 ) {
        tUntil =  nGen;
    }

    cout << "Simulate until: " << tUntil << endl;

    gsl_rng_env_setup();
    const gsl_rng_type * T = gsl_rng_default;
    string m_type = T->name;
    unsigned int m_seed = gsl_rng_default_seed;

    gsl_rng * my_rng = gsl_rng_alloc( T );
    gsl_rng_set( my_rng, m_seed );

    shared_ptr< iRNG > rng( new GSL_RNG( my_rng, m_type, m_seed ));
    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    RandomProcess::initialize( rng );

    double mu = vm[ MUTATION_RATE_K ].as<double>();
    double rho = vm[ RECOMBINATION_RATE_K ].as< double >();

    mmodel_type::initialize( mu, false);

    shared_ptr< SimulationStats > stats( new SimulationStats() );

    stats->startPhase( RUNTIME_K );

    environment_type population;
    environment_type buffer;

    system_id blank_id;

    stats->startPhase( "PopInit" );

    fitness_multiplicative< het_fitness, hom_fitness > fmult;

    for( unsigned int i = 0; i < vm[ FOUNDER_SIZE_K ].as< unsigned int >(); ++i) {
        population.push_back( new individual_type() );

        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );
        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY.copy() );

        buffer.push_back( new individual_type() );
    }

    std::cout << gamete_type::EMPTY.copies() << std::endl;
    assert( gamete_type::EMPTY.copies() - 1 == 2 * vm[ FOUNDER_SIZE_K ].as<unsigned int>() );

    stats->stopPhase( "PopInit" );

    environment_type * parent = &population, * child = &buffer;

    unsigned int fitness_size = 0;
    double * fitness = NULL;

    ReproduceWithRecombination repro( mu, rho );

    size_t nSelfing = 0;
    stats->startPhase( "Sim" );
    for( SystemClock::vtime_t i = 0; i < tUntil; ++i ) {

        if( fitness_size < parent->size() ) {
            if( fitness != NULL ) {
                delete [] fitness;
            }
            fitness_size = parent->size();
            fitness = new double[ fitness_size ];
        }

        memset( fitness, 0, sizeof(double) * fitness_size );
//        alphabet_type::getInstance()->resetFreeSymbols();

//        std::cout << "Generation: " << i << std::endl;

//        for( typename locus_bitset::active_iterator it = locus_bitset::active_begin(); it != locus_bitset::active_end(); it++ ) {
//            (*it)->updateSymbols();
//        }

        locus_bitset::updateActiveAlphabet();

        // measure fitness of parent population
        //
        double * tmp = fitness;
//        double e_fitness = 0.0;
        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*tmp) = 1.0;
            (*tmp) = fmult( (*tmp), (*it) );
            //e_fitness += (*tmp++);
            ++tmp;
        }
        //e_fitness /= (double)parent->size();

        //
        // mate
        //
        DiscreteSelector ds( my_rng, fitness, parent->size() );
        unsigned int child_idx = 0;
        while( child_idx < child->size()) {
            std::pair< individual_pointer, individual_pointer > mate_pair = ds( parent );

            nSelfing += (( mate_pair.first == mate_pair.second ) ? 1 : 0);
            gamete_pointer g = repro( mate_pair.first );
            assert( gamete_type::isGamete( g ) );
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g);

            gamete_pointer g1 = repro( mate_pair.second );
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g1);

            (*child)[child_idx++]->getProperties()->setDOB( i );
        }

        assert( parent->size() == vm[ FOUNDER_SIZE_K ].as< unsigned int > () );
        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*it)->reset();
        }

        std::swap( parent, child );
    }

    locus_bitset::updateActiveAlphabet();
    stats->stopPhase( "Sim" );
    stats->startPhase( "Final" );

    double nSymbols = 0;
    double nGametes = 0;
    size_t nMaxSymbols = 0, nMinSymbols = -1;
    double nBlocks = 0;
    unsigned int n = 0;
    for( typename locus_bitset::active_iterator it = locus_bitset::active_begin(); it != locus_bitset::active_end(); it++ ) {
        size_t s = (*it)->size();
        nBlocks += (double)(*it)->block_count();
        nSymbols += (double)s;
        nGametes += 1.0;

        if( nMaxSymbols < s ) {
            nMaxSymbols = s;
        }

        if( nMinSymbols > s ) {
            nMinSymbols = s;
        }

        n += (*it)->copies();
    }

    std::cout << "Final population has " << nGametes  << " (" << n << ") gametes" << std::endl;

    std::cout << "Average number of blocks per Gamete: " << nBlocks/nGametes << std::endl;

    std::cout << "Max Mutations single Gamete: " << nMaxSymbols << std::endl;
    std::cout << "Min Mutations single Gamete: " << nMinSymbols << std::endl;
    std::cout << "Average Mutations per Gamete: " << nSymbols/nGametes << std::endl;

    std::cout << "Population Mutation count: " << alphabet_type::getInstance()->active_count() << " (" << mmodel_type::getVariantMap()->size() << ")" << std::endl;

    std::cout << "Simulation Recombinations: " << repro.getRecombinationCalls() << std::endl;
    std::cout << "Simulation Recombination Events: " << repro.getRecombinationEvents() << std::endl;

    std::cout << "Simulation Mutation Calls: " << repro.getMutationCalls() << std::endl;
    std::cout << "Simulation Mutation Events: " << repro.getMutationEvents() << std::endl;

    std::cout << "Selfing count: " << nSelfing << std::endl;

    std::cout << "Lost or Fixed from previous generation: " << alphabet_type::getInstance()->fixed_lost_count() << std::endl;

    while( !population.empty() ) {
        individual_type * ind = population.back();
        population.pop_back();

        delete ind;
    }

    while( !buffer.empty() ) {
        individual_type * ind = buffer.back();
        buffer.pop_back();
        delete ind;
    }
    stats->stopPhase( "Final" );

    stats->stopPhase( RUNTIME_K );

    cout << *stats;

    delete [] fitness;

    return 0;
}
