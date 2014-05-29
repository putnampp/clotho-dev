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

#include "system_id.h"


BOOST_AUTO_TEST_SUITE( test_system_id )

BOOST_AUTO_TEST_CASE( create_id_test ) {
    system_id::manager_id_t man_id = 0x00000001;
    system_id::object_id_t obj_id =  0x00000020;
    system_id::system_key_t test_key = 0x0000000100000020;

    system_id sid( man_id, obj_id );

    BOOST_REQUIRE_MESSAGE( sid.getKey() == test_key, "Unexpected key mismatch of system_id::system_key: " << std::hex << sid.getKey() << " != " << test_key );

    BOOST_REQUIRE_MESSAGE( sid.getManagerID() == man_id, "ID1: Unexpected ManagerID - " << std::hex << sid.getManagerID() << " != " << man_id  );
    BOOST_REQUIRE_MESSAGE( sid.getObjectID() ==  obj_id, "ID1: Unexpected ObjectID - " << std::hex << sid.getObjectID() << " != " << obj_id  );

    system_id::manager_id_t test_manid = 0x00003000;
    system_id::object_id_t  test_objid = 0x00000001;
    system_id::system_key_t test_key2 = 0x0000300000000001;

    system_id sid2( test_manid, test_objid );

    BOOST_REQUIRE_MESSAGE( sid2.getManagerID() == test_manid, "ID2: Unexpected ManagerID" );
    BOOST_REQUIRE_MESSAGE( sid2.getObjectID() == test_objid, "ID2: Unexpected ObjectID" );
    BOOST_REQUIRE_MESSAGE( sid2.getKey() == test_key2, "ID2: Unexpected key");
}

BOOST_AUTO_TEST_CASE( system_id_comp_test ) {
    system_id::manager_id_t man_id = 0x00000010;
    system_id::object_id_t obj_id = 0, obj_id2 = 1;

    system_id sid( man_id, obj_id);
    system_id sid2( man_id, obj_id2);

    BOOST_REQUIRE_MESSAGE( sid < sid2, "FAILED: " << sid << " < " << sid2 );
}

BOOST_AUTO_TEST_CASE( system_id_equal_test ) {
    system_id::manager_id_t man_id = 0x00000010;
    system_id::object_id_t obj_id = 0, obj_id2 = 0;

    system_id sid( man_id, obj_id);
    system_id sid2( man_id, obj_id2);

    BOOST_REQUIRE_MESSAGE( sid == sid2, "FAILED: " << sid << " == " << sid2 );
}

BOOST_AUTO_TEST_CASE( system_id_assignment_test ) {
    system_id::manager_id_t man_id = 0x00000010;
    system_id::object_id_t obj_id = 10;

    system_id sid( man_id, obj_id);

    system_id sid2;
    sid2 = sid;

    BOOST_REQUIRE_MESSAGE( sid == sid2, "FAILED: " << sid << " == " << sid2 );
}

BOOST_AUTO_TEST_SUITE_END()
