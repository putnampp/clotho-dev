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

#include <vector>
#include <memory>

#include "utility/simulation_stats.h"
#include "object/individual.hpp"
#include "object/individual_properties.hpp"

#include "rng/rng.hpp"

#include "genetics/variant_base.h"
#include "genetics/basic_variant_map.h"
#include "genetics/gamete.h"
//#include "genetics/fixed_length_gamete.hpp"

//#include "models/default_life_cycle.hpp"
#include "models/selection_models.hpp"
//#include "models/reproduction_models.hpp"
#include "processes/reproduction_models.hpp"

using std::shared_ptr;

#define _PLOIDY 2

//typedef life_cycle::def_life_cycle    life_cycle_type;
typedef variant_base      variant_type;

typedef basic_variant_map variant_map_t;
typedef Gamete gamete_type;
typedef typename gamete_type::pointer gamete_ptr;

typedef reproduction::models::mutation::mutate_site< variant_type, variant_map_t, gamete_type >   mmodel_type;
typedef reproduction::models::recombination::no_recomb< _PLOIDY >     rcmodel_type;
typedef reproduction::IndividualReproduction< mmodel_type, rcmodel_type > rmodel_type;

//typedef TIndividual< life_cycle_type, individual_props< /*variant_type,*/ gamete_type, 2 >, rmodel_type > individual_type;
//typedef std::vector< individual_type * > environment_type;

typedef TIndividual< gamete_type, _PLOIDY, system_id, individual_props< gamete_type, _PLOIDY, system_id > > individual_type;
typedef std::vector< individual_type * > environment_type;

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

    std::pair< individual_type * , individual_type * > operator()( environment_type * env, double f = 1.0 ) {
        size_t i0 = gsl_ran_discrete( m_rng, m_lookup );
        size_t i1 = ((gsl_rng_uniform(m_rng) <= f ) ? i0 : gsl_ran_discrete( m_rng, m_lookup ));

        std::pair< individual_type *, individual_type * > res = make_pair( env->at(i0), env->at(i1));

        return res;
    }

    virtual ~DiscreteSelector() {
        delete m_lookup;
    }
protected:
    gsl_rng * m_rng;
    gsl_ran_discrete_t * m_lookup;
};

class SimpleSelector : public RandomProcess {
public:
    static std::pair< individual_type * , individual_type * > select( environment_type * env ) {
        std::pair< individual_type *, individual_type * > res;

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
    inline void operator()( double & f, variant_type::pointer v ) {
        f *= (1. + v->getDominance() * v->getSelection());
    }
};

struct hom_fitness {
    hom_fitness( double s = 1. ) : m_scaling(1.) {}
    inline void operator()( double & f, variant_type::pointer v ) {
        f *= (1. + v->getSelection() * m_scaling);
    }
    double m_scaling;
};

template < class het_policy, class hom_policy >
class fitness_multiplicative {
public:
//    typedef void (*update_policy)( double & f, variant_type::pointer v );
//
    fitness_multiplicative() {}

    fitness_multiplicative( het_policy & het, hom_policy & hom ) :
        m_het_case(het),
        m_hom_case(hom) {
    }

    double operator()( double f, individual_type * ind ) {
        return (*this)(f, ind->getProperties()->getGamete(0), ind->getProperties()->getGamete(1) );
    }

    double operator()( double f, gamete_ptr g1, gamete_ptr g2 ) {
        double res = f;
        if( g1 == g2 ) return res;

        unsigned int s = g1->size() + g2->size();
        if( s == 0 ) return res;

        gamete_type::var_iterator g1_it = g1->begin(), g1_e = g1->end();
        gamete_type::var_iterator g2_it = g2->begin(), g2_e = g2->end();

        while( g1_it != g1_e && g2_it != g2_e ) {
            if( *g1_it == *g2_it )  {
                m_hom_case( res, *g1_it++);
                g2_it++;
            } else if( (*g1_it)->getKey() < (*g2_it)->getKey() ) {
                m_het_case( res, *g1_it++);
            } else {
                m_het_case( res, *g2_it++);
            }
        }
        while( g1_it != g1_e ) {
            m_het_case(res, *g1_it++);
        }

        while( g2_it != g2_e ) {
            m_het_case(res, *g2_it++);
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

        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY_GAMETE.copy() );
        population.back()->getProperties()->inheritFrom( blank_id,  gamete_type::EMPTY_GAMETE.copy() );

        buffer.push_back( new individual_type() );
    }

    stats->stopPhase( "PopInit" );

    environment_type * parent = &population, * child = &buffer;

    unsigned int fitness_size = 0;
    double * fitness = NULL;

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

//        std::cout << "Generation: " << i << std::endl;
        // measure fitness of parent population
        //
        double * tmp = fitness;
//        double e_fitness = 0.0;
        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*tmp) = fmult( (*tmp), (*it) );
            //e_fitness += (*tmp++);
            ++tmp;
        }
        //e_fitness /= (double)parent->size();
        // mate
        //
        //
        DiscreteSelector ds( my_rng, fitness, parent->size() );
        unsigned int child_idx = 0;
        while( child_idx < child->size()) {
            (*child)[child_idx]->reset();

            //std::pair< individual_type *, individual_type * > mate_pair = selector_t::select( parent );
            std::pair< individual_type *, individual_type * > mate_pair = ds( parent );
            gamete_ptr g = rmodel_type::reproduce( mate_pair.first, (gamete_type *) NULL);
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g);

            gamete_ptr g1 = rmodel_type::reproduce( mate_pair.second, (gamete_type *) NULL);

            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g1);
            (*child)[child_idx++]->getProperties()->setDOB( i );
        }

        for( environment_type::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*it)->getProperties()->died();
        }

        std::swap( parent, child );
    }
    stats->stopPhase( "Sim" );
    stats->startPhase( "Final" );

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

    cout << "Created " << mmodel_type::getVariantMap()->size() << " variants" << std::endl;

    delete [] fitness;

    return 0;
}
