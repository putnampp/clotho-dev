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

#include "engine/ltsf_pool.hpp"
#include <vector>

struct pool_test_obj : public poolable {
    size_t getPoolIndex() const {
        return id;
    }

    void setPoolIndex( size_t i ) {
        id = i;
    }

    pool_test_obj( ) : id(-1) {}

    size_t id;

    virtual ~pool_test_obj() {}
};

typedef unsigned int my_time;
typedef ltsf_pool< my_time, pool_test_obj > ltsf_pool_t;

BOOST_AUTO_TEST_SUITE( test_ltsf_pool )

BOOST_AUTO_TEST_CASE( create_test_pool ) {

    ltsf_pool_t my_pool;

    std::vector< pool_test_obj * > test_objs;

    for( int i = 0; i < 10; ++i ) {
        test_objs.push_back( new pool_test_obj( ) );
        my_pool.setPoolObject( test_objs.back() );
    }

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == (my_time) -1, "unexpected timestamp for empty queue"  );

    BOOST_REQUIRE_MESSAGE( my_pool.getPoolObject( 0 ) == test_objs[0], "Unexpected object at index 0");

    my_time t = 0;
    pool_test_obj * obj = NULL;
    for( int i = 0; i < 2; ++i ) {
        my_pool.updateObject( *test_objs.begin(), t );

        BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == t, "Unexpected timestamp for updated queue - " << my_pool.peekNextTimestamp() );

        obj = my_pool.getNextObject( t + 1);
        BOOST_REQUIRE_MESSAGE( obj == NULL, "Unexpected object returned for invalid timestamp");

        obj = my_pool.getNextObject(t);

        BOOST_REQUIRE_MESSAGE( obj == test_objs[0], "Unexpected object returned for valid timestamp");
        BOOST_REQUIRE_MESSAGE( my_pool.getLTSFNodeCount() == 1, "Unexpected number of LTSF Nodes instantiated" );
    }

    while( !test_objs.empty()) {
        obj = test_objs.back();
        test_objs.pop_back();
        delete obj;
    }
}

BOOST_AUTO_TEST_CASE( test_moving_queue ) {
    ltsf_pool_t my_pool;

    std::vector< pool_test_obj * > test_objs;

    for( int i = 0; i < 10; ++i ) {
        test_objs.push_back( new pool_test_obj( ) );
        my_pool.setPoolObject( test_objs.back() );
    }

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == (my_time) -1, "unexpected timestamp for empty queue"  );

    BOOST_REQUIRE_MESSAGE( my_pool.getPoolObject( 0 ) == test_objs[0], "Unexpected object at index 0");

    my_time t = 3;
    pool_test_obj * obj = NULL;
    my_pool.updateObject( *test_objs.begin(), t );

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == t, "Unexpected timestamp for updated queue - " << my_pool.peekNextTimestamp() );

    obj = my_pool.getNextObject( t + 1);
    BOOST_REQUIRE_MESSAGE( obj == NULL, "Unexpected object returned for invalid timestamp");

    my_pool.updateObject( *test_objs.begin(), --t );

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == t, "Unexpected timestamp for updated queue - " << my_pool.peekNextTimestamp() );

    obj = my_pool.getNextObject( t + 1 );
    BOOST_REQUIRE_MESSAGE( obj == NULL, "Unexpected object returned for invalid timestamp");

    obj = my_pool.getNextObject( t );
    BOOST_REQUIRE_MESSAGE( obj == test_objs[0], "Unexpected object returned" );

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() == (my_time) -1, "Unexpected timestamp for expected empty queue " << my_pool.peekNextTimestamp() );

    while( !test_objs.empty()) {
        obj = test_objs.back();
        test_objs.pop_back();
        delete obj;
    }
}

BOOST_AUTO_TEST_CASE( popping_queue ) {
    ltsf_pool_t my_pool;

    std::vector< pool_test_obj * > test_objs;

    const int obj_count = 10;
    for( int i = 0; i < obj_count; ++i ) {
        test_objs.push_back( new pool_test_obj( ) );
        my_pool.setPoolObject( test_objs.back() );
        my_pool.updateObject( test_objs.back(), obj_count - i );
    }

    BOOST_REQUIRE_MESSAGE( my_pool.peekNextTimestamp() != (my_time) -1, "unexpected timestamp for empty queue"  );

    pool_test_obj * obj = NULL;

    int loop_count = obj_count;
    my_time t;
    while( (t = my_pool.peekNextTimestamp()) != (my_time) -1 ) {
        obj = my_pool.getNextObject( t );

        BOOST_REQUIRE_MESSAGE( obj == test_objs[--loop_count], "Unexpected object [" << obj->getPoolIndex() << "] returned at loop " << loop_count);
        BOOST_REQUIRE_MESSAGE( ((my_pool.peekNextTimestamp() == (my_time) -1) || t <= my_pool.peekNextTimestamp()), "Unexpected timestamp order: " << t << " before " << my_pool.peekNextTimestamp());
    }
}
BOOST_AUTO_TEST_SUITE_END()
