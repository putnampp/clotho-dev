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
#include "engine/engine.hpp"

#include "common_initializers.hpp"

//#include "selection.hpp"
//#include "reproduction.hpp"

#include "variant_base.h"

#include "rng/rng.hpp"
//#include "models/default_life_cycle.h"
#include "models/default_life_cycle.hpp"
#include "models/selection_models.hpp"
#include "models/reproduction_models.hpp"

#include <boost/program_options.hpp>

namespace po=boost::program_options;

bool parse_commandline( int argc, char ** argv, po::variables_map & vm );

const string RUNTIME_K = "total_runtime";

const string HELP_K = "help";
const string VERSION_K = "version";

const string ENGINE_CONFIG_K    = "engine_config";
const string APP_CONFIG_K       = "app_config";

const string SIM_UNTIL_K        = "sim-until";
const string THREAD_COUNT_K     = "thread-count";
const string GENERATIONS_K       = "generations";

const string SEQSM_K = "sequential";
const string CENSM_K = "centralized";
const string TCENSM_K = "threaded-centralized";

const string DISTRIBUTED_ENV_K = "dist-env";
const string FOUNDER_SIZE_K = "founder-size";

static const unsigned int MAX_THREADS = 24;

template <>
void SequentialSimulationManager< pooled_event_set >::routeEvent( const event * evt ) {
    if( insertEvent( evt ) ) {
        notifyNextEvent( evt->getReceiver(), evt->getReceived() );
    }
}

template <>
void CentralizedSimulationManager< pooled_event_set >::routeEvent( const event * evt ) {
    if( insertEvent( evt ) ) {
        notifyNextEvent( evt->getReceiver(), evt->getReceived() );
    }
}

template <>
void ThreadedCentralizedSimulationManager< pooled_event_set >::clearBufferedEvents() {
    for( unsigned int i = 0; i < m_nThreads; ++i ) {
        while( !m_buffers[i]->empty() ) {
            const event * t = m_buffers[i]->back();
            m_buffers[i]->pop_back();

            if( insertEvent( t ) ) {
                notifyNextEvent( t->getReceiver(), t->getReceived() );
            }
        }
    }
}

typedef life_cycle::def_life_cycle    LCM_t;
typedef variant_base      VT_t;

typedef reproduction::models::mutation::infinite_site< VT_t > mutation_model_t;

typedef typename mutation_model_t::variant_map_t variant_map_t;
typedef typename mutation_model_t::gamete_t gamete_t;

typedef reproduction::models::model< int > reproduction_model_t;
typedef selection::models::random_selection selection_model_t;

typedef TIndividual< LCM_t, individual_props< /*VT_t,*/ gamete_t, 2 >, reproduction_model_t > IND_t;
typedef TEnvironment< LCM_t, /*VT_t,*/ IND_t, selection_model_t > ENV_t;

typedef SequentialSimulationManager< ClothoEventSet >   SEQUENTIAL_MGR_t;
typedef ClothoApplication< SEQUENTIAL_MGR_t, ENV_t >    SEQUENTIAL_APP_t;

typedef CentralizedSimulationManager< ClothoEventSet >  CENTRAL_MGR_t;
typedef ClothoApplication< CENTRAL_MGR_t, ENV_t > SIMPLE_CENTRAL_APP_t;

namespace initializer {

template < > 
void ClothoAppInitializer::createEnvironment< SIMPLE_CENTRAL_APP_t, SIMPLE_CENTRAL_APP_t::environment_t >( SIMPLE_CENTRAL_APP_t * a, SIMPLE_CENTRAL_APP_t::environment_t * ) {
    cout << "Blocking default protocol" << endl;
    typename SIMPLE_CENTRAL_APP_t::environment_ptr_t env = new typename SIMPLE_CENTRAL_APP_t::environment_t( a->m_sim_manager, a->m_rng );

    env->setFounderSize( a->m_nFounder );
    env->initialize();
    a->m_objects.push_back( env->getSystemID());
}
}


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
        tUntil = LCM_t::convertGenerationsToVTime( nGen );
    }

    cout << "Simulate until: " << tUntil << endl;

    shared_ptr< iRNG > rng( new GSL_RNG());

    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    shared_ptr< application > app;
    shared_ptr< SimulationStats > stats( new SimulationStats() );

    simulation_manager * sim = NULL;

    if( vm.count( CENSM_K ) ) {
        cout << "Using a Centralized Simulation Manager" << endl;

        SIMPLE_CENTRAL_APP_t * tmp = new SIMPLE_CENTRAL_APP_t( rng );

        app.reset( tmp );
        CENTRAL_MGR_t * tmgr = new CENTRAL_MGR_t( app, stats );

        tmp->setSimulationManager( tmgr );
        tmp->setFounderSize( vm[ FOUNDER_SIZE_K ].as<unsigned int>() );
        sim = tmgr;
    } /*else if( vm.count( TCENSM_K ) ) {
        unsigned int tc = vm[ THREAD_COUNT_K ].as< unsigned int >();

        assert( 0 < tc && tc <= MAX_THREADS );
        cout << "Using a Threaded Centralized Simulation Manager with " << tc << " threads" << endl;

        if( tc > vm[ DISTRIBUTED_ENV_K ].as< unsigned int >() ) {
            app.reset( new ClothoApplication( clotho_config, rng, tc ) );
        } else {
            app.reset( new ClothoApplication( clotho_config, rng, vm[ DISTRIBUTED_ENV_K ].as< unsigned int >() ) );
        }
        sim = new ThreadedCentralizedSimulationManager< ClothoEventSet >( app, stats, tc );

    } else {
        cout << "Using a Sequential Simulation Manager" << endl;
        app.reset( new ClothoApplication( clotho_config, rng, vm[ DISTRIBUTED_ENV_K ].as< unsigned int >() ) );
        sim = new SequentialSimulationManager< ClothoEventSet >( app, stats );
    }*/

    assert( sim != NULL );

//    (std::static_pointer_cast<ClothoApplication>(app))->setSimulationManager( sim );
//    (std::static_pointer_cast<ClothoApplication>(app))->setFounderSize( vm[ FOUNDER_SIZE_K ].as<unsigned int>() );

    stats->startPhase( RUNTIME_K );

    sim->initialize();

    sim->simulate( tUntil );

    sim->finalize();
    
    stats->stopPhase( RUNTIME_K );

    cout << *stats;

    delete sim;

    return 0;
}

bool parse_commandline( int argc, char ** argv, po::variables_map & vm ) {
    po::options_description gen( "General" );
    gen.add_options()
    ( (HELP_K + ",h").c_str(), "Print this" )
    ( (VERSION_K + ",v").c_str(), "Version" )
    ;

    po::options_description simulation( "Simulation Parameters" );
    simulation.add_options()
    ( SIM_UNTIL_K.c_str(), po::value<SystemClock::vtime_t>()->default_value( SystemClock::POSITIVE_INFINITY ), "Simulate until time. Default value is positive infinity.")
    ( THREAD_COUNT_K.c_str(), po::value< unsigned int >()->default_value( 4 ), "Thread count for thread aware simulation managers are used. Does not apply when --sequential, or --centralized flags are used")
    ( SEQSM_K.c_str(), "Run the simulation with the sequential simulation manager" )
    ( CENSM_K.c_str(), "Run the simulation with the centralized simulation manager" )
    ( TCENSM_K.c_str(), "Run the simulation with the centralized simulation manager (thread aware)")
    ;

    po::options_description clotho_app( "Clotho Application Parameters" );
    clotho_app.add_options()
    ( GENERATIONS_K.c_str(), po::value<unsigned int>()->default_value(-1), "Simulate a number of generations.")
    ( DISTRIBUTED_ENV_K.c_str(), po::value< unsigned int >()->default_value( 1 ), "Number of environment partitions; Thread aware simulation managers will partition the environment into the max of the number of partitions and thread count" )
    ( FOUNDER_SIZE_K.c_str(), po::value< unsigned int >()->default_value(10000), "Founding population size" )
    ;

    po::options_description cmdline;

    cmdline.add(gen).add(simulation).add( clotho_app );
    po::store( po::command_line_parser( argc, argv ).options( cmdline ).run(), vm );

    bool res = true;
    if( vm.count( HELP_K ) ) {
        cout << cmdline << endl;
        res = false;
    }

    return res;
}
