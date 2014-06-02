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

//#include "clotho_application.hpp"
//#include "engine/engine.hpp"
#include "engine/simulation_stats.h"
#include "object/individual2.hpp"

#include "variant_base.h"
#include "basic_variant_map.h"
#include "gamete.h"
#include "fixed_length_gamete.hpp"

#include "rng/rng.hpp"
#include "models/default_life_cycle.hpp"
#include "models/selection_models.hpp"
#include "models/reproduction_models.hpp"

using std::shared_ptr;

typedef life_cycle::def_life_cycle    LCM_t;
typedef variant_base      VT_t;

typedef basic_variant_map variant_map_t;
typedef Gamete gamete_t;
typedef typename gamete_t::pointer gamete_ptr;

typedef reproduction::models::mutation::mutate_site< VT_t, variant_map_t, gamete_t >   mutation_model_t;
typedef reproduction::models::recombination::no_recomb< 2 >     recombination_model_t;

typedef reproduction::IndividualReproduction< mutation_model_t, recombination_model_t > reproduction_model_t;

typedef TIndividual< LCM_t, individual_props< /*VT_t,*/ gamete_t, 2 >, reproduction_model_t > individual_t;
typedef std::vector< individual_t * > environment_t;


class SimpleSelector : public RandomProcess {
public:
    static std::pair< individual_t * , individual_t * > select( environment_t * env ) {
        std::pair< individual_t *, individual_t * > res;

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

    shared_ptr< iRNG > rng( new GSL_RNG());
    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    mutation_model_t::initialize( 0.0001, false);
    
/*    const double dMaxVariants = 10000.0;
    for( double i = 0.0; i < dMaxVariants; i += 1.0 ) {
        mutation_model_t::getVariantMap()->createVariant( i / dMaxVariants);
    }

    assert(mutation_model_t::getVariantMap()->size() == dMaxVariants);*/

    RandomProcess::initialize( rng );

    //shared_ptr< application > app;
    shared_ptr< SimulationStats > stats( new SimulationStats() );


    stats->startPhase( RUNTIME_K );

    environment_t population;
    environment_t buffer;

    system_id blank_id;

    stats->startPhase( "PopInit" );
    for( unsigned int i = 0; i < vm[ FOUNDER_SIZE_K ].as< unsigned int >(); ++i) {
        population.push_back( new individual_t() );

        population.back()->getProperties()->inheritFrom( blank_id,  gamete_t::EMPTY_GAMETE.copy() );
        population.back()->getProperties()->inheritFrom( blank_id,  gamete_t::EMPTY_GAMETE.copy() );

        buffer.push_back( new individual_t() );
    }

    stats->stopPhase( "PopInit" );

    environment_t * parent = &population, * child = &buffer;

    stats->startPhase( "Sim" );
    for( SystemClock::vtime_t i = 0; i < tUntil; ++i ) {

        std::cout << "Generation: " << i << std::endl;
        // mate
        unsigned int child_idx = 0;
        while( child_idx < child->size()) {
            (*child)[child_idx]->reset();

            std::pair< individual_t *, individual_t * > mate_pair = selector_t::select( parent );
            gamete_ptr g = reproduction_model_t::reproduce( mate_pair.first, (gamete_t *) NULL);
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g);

            gamete_ptr g1 = reproduction_model_t::reproduce( mate_pair.second, (gamete_t *) NULL);
            
            (*child)[child_idx]->getProperties()->inheritFrom(blank_id, g1);
            (*child)[child_idx++]->getProperties()->setDOB( i );
        }

        for( environment_t::iterator it = parent->begin(); it != parent->end(); it++ ) {
            (*it)->getProperties()->died();
        }

        std::swap( parent, child );
    }
    stats->stopPhase( "Sim" );
    stats->startPhase( "Final" );

    while( !population.empty() ) {
        individual_t * ind = population.back();
        population.pop_back();

        delete ind;
    }

    while( !buffer.empty() ) {
        individual_t * ind = buffer.back();
        buffer.pop_back();
        delete ind;
    }
    stats->stopPhase( "Final" );
    
    stats->stopPhase( RUNTIME_K );

    cout << *stats;

    cout << "Created " << mutation_model_t::getVariantMap()->size() << " variants" << std::endl;

    return 0;
}
