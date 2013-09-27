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
#include "StatisticFactory.h"

#include "StatCreator.h"

BOOST_AUTO_TEST_SUITE( test_statistic_factory )

BOOST_AUTO_TEST_CASE( sfSingleton ) {
    BOOST_REQUIRE_MESSAGE( StatisticFactory::getInstance() != NULL, "Failed to get Statistic Factory" );
}

size_t c = StatisticFactory::getInstance()->count();

REGISTERED_STATISTIC( testA, "this is just a test") {
public:
        testA() : Statistic("this is just a test") {}
        void configure( std::istream & in ) {}

        void operator()(const Population & p ) {}
};

BOOST_AUTO_TEST_CASE( sfAutoRegistration ) {

    BOOST_REQUIRE_MESSAGE( StatisticFactory::getInstance()->count() == (c + 1), "Failed to auto register new statistic" );
}

BOOST_AUTO_TEST_CASE( sfCreation ) {
    string  name("testA");
    string  dne_name("dne_test");

    boost::shared_ptr< Statistic > stat = StatisticFactory::getInstance()->create( name );

    BOOST_REQUIRE_MESSAGE( stat, name << " does not exist when it should!!" );

    boost::shared_ptr< Statistic > stat2 = StatisticFactory::getInstance()->create( dne_name );

    BOOST_REQUIRE_MESSAGE( !stat2, dne_name << " exists when it shouldn't!!" );
}

BOOST_AUTO_TEST_SUITE_END()
