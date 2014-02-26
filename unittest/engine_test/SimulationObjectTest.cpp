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
#include "test_simulation_object.h"

#include "SequentialSimulationManager.h"
#include "test_application.h"

BOOST_AUTO_TEST_SUITE( test_simulation_object )

BOOST_AUTO_TEST_CASE( create_test_so ) {
    MyTestSO so;

    system_id expected_id(0);

    BOOST_REQUIRE_MESSAGE( so.getSystemID() == expected_id, "Unexpected object id: " << so.getSystemID() << " != " << expected_id );
}

BOOST_AUTO_TEST_CASE( create_test_so2 ) {
    application * app = new TestApplication();

    system_id::manager_id_t man_id = 0x00000020;
    SequentialSimulationManager ssm( app, man_id );

    object * so = new MyTestSO( &ssm );

    system_id expected_id( man_id, 1 );

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 1, "Simulation Object was not registered" );
    BOOST_REQUIRE_MESSAGE( so->getSystemID() == expected_id, "Unexpected object id: " << so->getSystemID() << " != " << expected_id );

    object * tmp_obj = ssm.getObject( expected_id );

    BOOST_REQUIRE_MESSAGE( so == tmp_obj, "Unexpected object returned by simulation manager" );

    so->finalize();
    delete so;

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 0, "Simulation Object was not unregistered" );
    delete app;
}

BOOST_AUTO_TEST_CASE( process_test ) {
    application * app = new TestApplication();

    system_id::manager_id_t man_id = 0x00000030;
    SequentialSimulationManager ssm( app, man_id );

    object * so = new MyTestSO( &ssm );

    system_id expected_id( man_id, 1 );

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 1, "Simulation Object was not registered" );
    BOOST_REQUIRE_MESSAGE( so->getSystemID() == expected_id, "Unexpected object id: " << so->getSystemID() << " != " << expected_id );

    object * tmp_obj = ssm.getObject( expected_id );
    tmp_obj->process();

    BOOST_REQUIRE_MESSAGE( so == tmp_obj, "Unexpected object returned by simulation manager" );

    so->finalize();
    delete so;

    BOOST_REQUIRE_MESSAGE( ssm.getObjectCount() == 0, "Simulation Object was not unregistered" );
    delete app;
}

BOOST_AUTO_TEST_SUITE_END()
