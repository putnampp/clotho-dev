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
#include "engine/simulation_stats.h"

#include "rng/rng.hpp"

const string RUNTIME_K = "total_runtime";

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

int main( int argc, char ** argv ) {

    if( argc < 3 || argc > 5 ) {
        cerr << "Expected Usage: clotho <engine_config_file> <app_config_file> (simulateUntil) ([C]entralized)" << endl;
        return EXIT_FAILURE;
    }

    string engine_config( argv[1] );
    string clotho_config( argv[2] );

    SystemClock::vtime_t tUntil = SystemClock::POSITIVE_INFINITY;

    if( argc >= 4 ) {
        string tmp = argv[3];
        tUntil = SystemClock::toVTime( tmp );
    }

    cout << "Simulate until: " << tUntil << endl;

    shared_ptr< iRNG > rng( new GSL_RNG());

    cout << "RNG: " <<  rng->getType() << "; seed: " << rng->getSeed() << endl;

    shared_ptr< application > app( new ClothoApplication( clotho_config, rng ) );

    shared_ptr< SimulationStats > stats( new SimulationStats() );

    simulation_manager * sim = NULL;

    if( argc == 5 && argv[4][0] == 'C' ) {
        cout << "Using a Centralized Simulation Manager" << endl;
        sim = new CentralizedSimulationManager< ClothoEventSet >( app, stats );
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
