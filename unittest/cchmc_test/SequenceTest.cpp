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
#include "Sequence.h"

BOOST_AUTO_TEST_SUITE( test_sequence )


BOOST_AUTO_TEST_CASE( seq_create ) {
    const size_t size = 35;
    Sequence s(size);

    BOOST_REQUIRE_MESSAGE( s.length() == size, "Failed to create a sequence with " << size << " loci" );
}

BOOST_AUTO_TEST_CASE( seq_create2 ) {
    const size_t size = 35;
    shared_ptr< allele_t [] > alleles( new allele_t[size]);

    for(size_t i = 0; i < size; ++i ) {
        alleles[i] = (i % MAX_ALLELES);
    }

    Sequence s(alleles, size);

    BOOST_REQUIRE_MESSAGE( s.length() == size, "Failed to create a sequence with " << size << " loci" );

    for( size_t i = 0; i < size; ++i ) {
        BOOST_REQUIRE_MESSAGE( s.allele( i ) == (i % MAX_ALLELES), "Unexpected allele value at " << i );
    }

    // modify sequence allele
    s.allele(25) = (allele_t)24;

    BOOST_REQUIRE_MESSAGE( alleles[25] == (allele_t)24, "Change to sequence should be reflected in original vector");
    alleles.reset();
}

BOOST_AUTO_TEST_CASE( seq_set_locus ) {
    const size_t size = 35;

    Sequence s( size );

    for( size_t i = 0; i < size; ++i ) {
        s.allele( i ) = (allele_t) i;
    }

    for( size_t i = 0; i < size; ++i ) {
        BOOST_REQUIRE_MESSAGE( s.allele(i) == (allele_t)i, "Allele " << i << ": Expected = " << (allele_t)i << "; Actual = " << s.allele(i) );
    }
}

BOOST_AUTO_TEST_SUITE_END()
