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

#include "clotho_application.h"
#include "engine/SequentialSimulationManager.hpp"
#include "engine/CentralizedSimulationManager.hpp"
#include "engine/ThreadedCentralizedSimulationManager.hpp"
#include "engine/simulation_stats.h"

#include "rng/rng.hpp"

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

const string SEQSM_K = "sequential";
const string CENSM_K = "centralized";
const string TCENSM_K = "threaded-centralized";

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

int main( int argc, char ** argv ) {

    po::variables_map vm;
    if( !parse_commandline( argc, argv, vm ) ) {
        return 0;
    }

    string engine_config( "" );
    string clotho_config( "" );

    SystemClock::vtime_t tUntil = vm[ SIM_UNTIL_K ].as< event::vtime_t >();

    cout << "Simulate until: " << tUntil << endl;

    shared_ptr< iRNG > rng( new GSL_RNG());

    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    shared_ptr< application > app( new ClothoApplication( clotho_config, rng ) );

    shared_ptr< SimulationStats > stats( new SimulationStats() );

    simulation_manager * sim = NULL;

    if( vm.count( CENSM_K ) ) {
        cout << "Using a Centralized Simulation Manager" << endl;
        sim = new CentralizedSimulationManager< ClothoEventSet >( app, stats );
    } else if( vm.count( TCENSM_K ) ) {
        unsigned int tc = vm[ THREAD_COUNT_K ].as< unsigned int >();

        assert( 0 < tc && tc <= 6 );
        cout << "Using a Threaded Centralized Simulation Manager with " << tc << " threads" << endl;
        sim = new ThreadedCentralizedSimulationManager< ClothoEventSet >( app, stats, tc );
    } else {
        cout << "Using a Sequential Simulation Manager" << endl;
        sim = new SequentialSimulationManager< ClothoEventSet >( app, stats );
    }

    assert( sim != NULL );

    (std::static_pointer_cast<ClothoApplication>(app))->setSimulationManager( sim );

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
    ( SIM_UNTIL_K.c_str(), po::value<event::vtime_t>()->default_value( SystemClock::POSITIVE_INFINITY ), "Simulate until time. Default value is positive infinity")
    ( THREAD_COUNT_K.c_str(), po::value< unsigned int >()->default_value( 4 ), "Thread count for thread aware simulation managers are used. Does not apply when --sequential, or --centralized flags are used")
    ( SEQSM_K.c_str(), "Run the simulation with the sequential simulation manager" )
    ( CENSM_K.c_str(), "Run the simulation with the centralized simulation manager" )
    ( TCENSM_K.c_str(), "Run the simulation with the centralized simulation manager (thread aware)")
    ;

//    po::options_description app( "Application Parameters" );

    po::options_description cmdline;

    cmdline.add(gen).add(simulation);
    po::store( po::command_line_parser( argc, argv ).options( cmdline ).run(), vm );

    bool res = true;
    if( vm.count( HELP_K ) ) {
        cout << cmdline << endl;
        res = false;
    }

    return res;
}
