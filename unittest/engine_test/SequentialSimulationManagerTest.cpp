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

#include <boost/test/unit_test.hpp>
#include "SequentialSimulationManager.h"

#include "test_application.h"
#include "test_object.h"

#include "test_event.h"

BOOST_AUTO_TEST_SUITE( test_sequential_simulation_manager )

BOOST_AUTO_TEST_CASE( create_seq_sim_man ) {

    application * app = new TestApplication();

    system_id::manager_id_t man_id = 0x00000010;

    SequentialSimulationManager ssm( app, man_id );

    system_id expected_id(man_id, 0);

    BOOST_REQUIRE_MESSAGE( ssm.getSystemID() == expected_id, "Unexpected Sequential Simulation Manager system id: " << ssm.getSystemID() << " != " << expected_id );

    delete app;
}

BOOST_AUTO_TEST_CASE( register_object ) {
    application * app = new TestApplication();
    system_id::manager_id_t man_id = 0x00000010;

    SequentialSimulationManager ssm( app, man_id );

    system_id expected_ssm_id( man_id, 0 );

    BOOST_REQUIRE_MESSAGE( true, "" );

    TestObject obj( &ssm );

    system_id expected_obj_id( man_id, 1 );

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 1, "Unexpected number of registered objects: " << ssm.getObjectCount() << " != 1");

    BOOST_REQUIRE_MESSAGE( obj.getSystemID() == expected_obj_id, "Unexpected Test Object system id: " << obj.getSystemID() << " != " << expected_obj_id );

    delete app;
}

BOOST_AUTO_TEST_CASE( get_and_unregister_object ) {
    application * app = new TestApplication();
    system_id::manager_id_t man_id = 0x00000010;

    SequentialSimulationManager ssm( app, man_id );

    system_id expected_ssm_id( man_id, 0 );

    object * obj = new TestObject( &ssm );

    system_id expected_obj_id( man_id, 1 );

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 1, "Unexpected number of registered objects: " << ssm.getObjectCount() << " != 1");

    BOOST_REQUIRE_MESSAGE( obj->getSystemID() == expected_obj_id, "Unexpected Test Object system id: " << obj->getSystemID() << " != " << expected_obj_id );

    object * tmp_obj = ssm.getObject( expected_obj_id );

    if( tmp_obj == NULL ) {
        BOOST_REQUIRE_MESSAGE( false, "No matching object found by system id" );
    } else {
        BOOST_REQUIRE_MESSAGE( tmp_obj == obj, "Unexpected object returned by getObject: " << tmp_obj->getSystemID() << " != " << obj->getSystemID() );
    }

    delete obj; // deleting object should unregister it from simulation manager

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 0, "Unexpected number of registered objects: " << ssm.getObjectCount() << " != 0");

    delete app;
}

BOOST_AUTO_TEST_CASE( route_event_test ) {
    application * app = new TestApplication();
    system_id::manager_id_t man_id = 0x00000010;

    SequentialSimulationManager ssm( app, man_id );
    system_id expected_ssm_id( man_id, 0 );

    object * obj = new TestObject( &ssm );

    default_event::vtime_t sr_time(0);
    default_event::event_id_t eid(0);

    default_event * evt = new default_event( sr_time, sr_time, obj, obj, eid );

    ssm.routeEvent( evt );

    BOOST_REQUIRE_MESSAGE( obj->pendingEventCount() == 1, "Unexpected number of pending events: " << obj->pendingEventCount() << " != 1" );

    const event * tmp_evt = obj->peekEvent( );

    if( tmp_evt == NULL ) {
        BOOST_REQUIRE_MESSAGE( false, "No event returned by peekEvent" );
    } else {
        BOOST_REQUIRE_MESSAGE( tmp_evt == evt, "Unexpected event returned by peekEvent");
    }

    tmp_evt = obj->getEvent();

    if( tmp_evt == NULL ) {
        BOOST_REQUIRE_MESSAGE( false, "No event returned by getEvent" );
    } else {
        BOOST_REQUIRE_MESSAGE( tmp_evt == evt, "Unexpected event returned by getEvent");
    }

    delete evt;
    delete obj;
    delete app;
}

BOOST_AUTO_TEST_SUITE_END()
