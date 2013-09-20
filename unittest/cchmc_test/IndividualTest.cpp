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
#include "Individual.hpp"
#include "GenomeFactory.hpp"

BOOST_AUTO_TEST_SUITE( test_individual )

const chromid_t chroms = 23;
const ploidy_t  ploid = 2;

static GenomeFactory g(chroms);

BOOST_AUTO_TEST_CASE( ind_create ) {
    IndividualPtr ind = g.createIndividual( ploid );

    BOOST_REQUIRE_MESSAGE( ind->chromosomes() == chroms, "Unexpected number of chromosomes" );
}

BOOST_AUTO_TEST_CASE( ind_chrom_size ) {
    IndividualPtr ind = g.createIndividual( ploid );

    for( chromid_t c = 0; c < chroms; ++c ) {
        for( ploidy_t p = 0; p < ploid; ++p ) {
            // A default genome has no loci set, therefore all sequences should have 0 length
            SequencePtr sq = ind->getSequenceByIndex(c, p);
            BOOST_REQUIRE_MESSAGE( (sq != NULL), "Unexpected NULL_SEQUENCE at " << c << ", " << p);
            BOOST_REQUIRE_MESSAGE( (sq->length() == 0), "Unexpected length of sequence for chromosome " << c << " copy " << p << ", " << sq->length());
        }
    }
}

BOOST_AUTO_TEST_CASE( ind_chrom_size2 ) {
    // add site to all chromosomes
    //
    g.reset();
    for( chromid_t c = 0; c < chroms; ++c ) {
        BOOST_REQUIRE_MESSAGE( g.addChromosomeSite(c, (size_t) 42 ), "Site not added" );
    }

    BOOST_REQUIRE_MESSAGE( g.loci() == chroms, "Unexpected number of loci: " << g.loci() << " v " << chroms );

    // Individual object assumes underlying genome has already been constructed
    IndividualPtr h = g.createIndividual(ploid);

    for( chromid_t c = 0; c < chroms; ++c ) {
        for( ploidy_t p = 0; p < ploid; ++p ) {
            SequencePtr sq = h->getSequenceByIndex( c, p );
            BOOST_REQUIRE_MESSAGE( (sq != NULL), "Unexpected NULL_SEQUENCE at " << c << ", " << p);
            BOOST_REQUIRE_MESSAGE( (sq->length() == 1), "Unexpected sequence length");
        }
    }

    g.reset();
}

BOOST_AUTO_TEST_SUITE_END()
