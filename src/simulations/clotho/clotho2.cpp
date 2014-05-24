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
#include <cstdlib>

#include "clotho_application.hpp"
#include "clotho_commandline.h"
#include "engine/engine.hpp"

//#include "selection.hpp"
//#include "reproduction.hpp"

#include "variant_base.h"

#include "rng/rng.hpp"
//#include "models/default_life_cycle.h"
//#include "models/default_life_cycle.hpp"
#include "models/opt_default_life_cycle.hpp"
#include "models/selection_models.hpp"
#include "models/reproduction_models.hpp"

const unsigned int MAX_THREADS = 24;

//typedef life_cycle::def_life_cycle    LCM_t;
typedef life_cycle::opt_default_life_cycle    LCM_t;
typedef variant_base      VT_t;

typedef reproduction::models::mutation::mutate_site< VT_t >   mutation_model_t;
typedef reproduction::models::recombination::no_recomb< 2 >     recombination_model_t;

typedef typename mutation_model_t::variant_map_t variant_map_t;
typedef typename mutation_model_t::gamete_t gamete_t;

typedef reproduction::IndividualReproduction< mutation_model_t, recombination_model_t > reproduction_model_t;
typedef selection::models::random_selection selection_model_t;

typedef TIndividual< LCM_t, individual_props< /*VT_t,*/ gamete_t, 2 >, reproduction_model_t > IND_t;
typedef TEnvironment< LCM_t, /*VT_t,*/ IND_t, selection_model_t > ENV_t;

typedef CentralizedSimulationManager< ClothoEvent, ClothoObject >  CENTRAL_MGR_t;
typedef ClothoApplication< CENTRAL_MGR_t, ENV_t > SIMPLE_CENTRAL_APP_t;

typedef ThreadedCentralizedSimulationManager< ClothoEvent, ClothoObject >  THREADED_CENTRAL_MGR_t;
typedef ClothoApplication< THREADED_CENTRAL_MGR_t, ENV_t > THREADED_CENTRAL_APP_t;

typedef SimulationManager< ClothoEvent, ClothoObject > simulation_manager_t;

namespace initializer {

template < > 
void ClothoAppInitializer::createEnvironment< SIMPLE_CENTRAL_APP_t, SIMPLE_CENTRAL_APP_t::environment_t >( SIMPLE_CENTRAL_APP_t * a, SIMPLE_CENTRAL_APP_t::environment_t * ) {
    cout << "Blocking default protocol" << endl;
    //typename SIMPLE_CENTRAL_APP_t::environment_ptr_t env = new typename SIMPLE_CENTRAL_APP_t::environment_t( a->m_sim_manager, a->m_rng );
    //
    typedef typename SIMPLE_CENTRAL_APP_t::environment_t environment_t;

    ClothoObject * co = a->m_sim_manager->getSimulationObject();

    a->m_env = new environment_t( co, a->m_rng );
    a->m_env->setFounderSize( a->m_nFounder );
    a->m_env->initialize();
    //a->m_objects.push_back( env->getSystemID());
}

template <>
void ClothoAppInitializer::createEnvironment< THREADED_CENTRAL_APP_t, THREADED_CENTRAL_APP_t::environment_t >( THREADED_CENTRAL_APP_t * a, THREADED_CENTRAL_APP_t::environment_t * ) {
    cout << "Blocking default protocol" << endl;
    //
    typedef typename THREADED_CENTRAL_APP_t::environment_t environment_t;

    ClothoObject * co = a->m_sim_manager->getSimulationObject();

    a->m_env = new environment_t( co, a->m_rng );
    a->m_env->setFounderSize( a->m_nFounder );
    a->m_env->initialize();
}

}   // namespace initializer


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
        tUntil = LCM_t::convertGenerationsToVTime( nGen ) /* + 1*/;
    }

    cout << "Simulate until: " << tUntil << endl;

    shared_ptr< iRNG > rng( new GSL_RNG());
    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    double mu = vm[ MUTATION_RATE_K].as<double>();
    cout << "Mutation rate: " << mu << endl;

    double rho = vm[ RECOMBINATION_RATE_K].as<double>();
    cout << "Recombination rate: " << rho << endl;

    unsigned int max_variants = vm[ VARIANT_POOL_SIZE_K ].as< unsigned int >();

    bool bFixed =  max_variants != (unsigned int) -1;

    shared_ptr< application > app;
    shared_ptr< SimulationStats > stats( new SimulationStats() );

    simulation_manager_t * sim = NULL;

    if( vm.count( CENSM_K ) ) {
        RandomProcess::initialize( rng );
        mutation_model_t::initialize(mu, bFixed);

        cout << "Using a Centralized Simulation Manager" << endl;

        SIMPLE_CENTRAL_APP_t * tmp = new SIMPLE_CENTRAL_APP_t( rng );

        app.reset( tmp );
        CENTRAL_MGR_t * tmgr = new CENTRAL_MGR_t( app, stats );

        tmp->setSimulationManager( tmgr );
        tmp->setFounderSize( vm[ FOUNDER_SIZE_K ].as<unsigned int>() );
        sim = tmgr;
    } else if( vm.count( TCENSM_K ) ) {
        shared_ptr< iRNG > trng( new ThreadedRNG( rng ) );

        RandomProcess::initialize( trng );
        mutation_model_t::initialize(mu, bFixed);
        unsigned int tc = vm[ THREAD_COUNT_K ].as< unsigned int >();

        assert( 0 < tc && tc <= MAX_THREADS );
        cout << "Using a Threaded Centralized Simulation Manager with " << tc << " threads" << endl;

        THREADED_CENTRAL_APP_t * tmp = new THREADED_CENTRAL_APP_t( rng );
        app.reset(tmp);

        THREADED_CENTRAL_MGR_t * tmgr = new THREADED_CENTRAL_MGR_t(app, stats, tc );
        tmp->setSimulationManager( tmgr );
        tmp->setFounderSize( vm[ FOUNDER_SIZE_K ].as<unsigned int>() );
        sim = tmgr;
    } /*else {
        cout << "Using a Sequential Simulation Manager" << endl;
        app.reset( new ClothoApplication( clotho_config, rng, vm[ DISTRIBUTED_ENV_K ].as< unsigned int >() ) );
        sim = new SequentialSimulationManager< ClothoEventSet >( app, stats );
    }*/

    if( bFixed ) {
        cout << "Creating static variant pool of size " << max_variants << endl;
        const double dMaxVariants = (double) max_variants;
        for( double i = 0.0; i < dMaxVariants; i += 1.0 ) {
            mutation_model_t::getVariantMap()->createVariant( i / dMaxVariants);
        }
    }

    assert( sim != NULL );

    stats->startPhase( RUNTIME_K );

    sim->initialize();

    sim->simulate( tUntil );

    //unsigned int prevalent_mutations = 0;
    //for( variant_map_t::const_iterator it = mutation_model_t::getVariantMap()->begin(); it != mutation_model_t::getVariantMap()->end(); it++ ) {
    //    (*it)->print( std::cout );
    //    if( (*it)->getPenetrance() > 0 ) {
    //        ++prevalent_mutations;
    //    }
    //}

    sim->finalize();
    
    stats->stopPhase( RUNTIME_K );

    cout << *stats;

    cout << "Created " << mutation_model_t::getVariantMap()->size() << " variants" << std::endl;

    //std::cout << "Prevalent Mutations within population: " << prevalent_mutations << std::endl;

    delete sim;

    return 0;
}
