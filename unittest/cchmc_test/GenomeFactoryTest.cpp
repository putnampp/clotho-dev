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
#include "GenomeFactory.hpp"

BOOST_AUTO_TEST_SUITE( test_genome_factory )

const chromid_t chrom = 23;
const ploidy_t  ploid = 2;

typedef GenomeFactory< chrom > GF;

BOOST_AUTO_TEST_CASE( genome_factory_create ) {
    BOOST_REQUIRE_MESSAGE( (GF::getInstance() != NULL), "Unable to create GenomeFactory<" << chrom << ">" );
}

BOOST_AUTO_TEST_CASE( genome_factory_default_size ) {
    BOOST_REQUIRE_MESSAGE( (GF::getInstance()->total_size() == (chrom * DEFAULT_CHROMOSOME_LEN)), "Unexpected length of the default genome: " << (GF::getInstance()->total_size()));
    BOOST_REQUIRE_MESSAGE( (GF::getInstance()->loci() == 0), "Unexpected number of loci in default genome");
}

BOOST_AUTO_TEST_CASE( genome_factory_reset ) {
    // add a site of interest to each chromosome
    // site pos is unimportant, just needs to be smaller
    // than the chrom_len
    for( chromid_t c = 0; c < chrom; ++c ) {
        size_t before = GF::getInstance()->getChromosome(c)->loci();

        GF::getInstance()->addChromosomeSite( c, (size_t) 33 );

        BOOST_REQUIRE_MESSAGE( (GF::getInstance()->getChromosome(c)->loci() == before + 1), "Site was not added");
    }

    // reset the GenomeFactory to default
    GF::getInstance()->reset();

    for( chromid_t c = 0; c < chrom; ++c ) {
        for( ploidy_t p = 0; p < ploid; ++p ) {
            ChromosomePtr chr = GF::getInstance()->getChromosome(c);

            BOOST_REQUIRE_MESSAGE( (chr->length() == DEFAULT_CHROMOSOME_LEN), "Unexpected size of chromosome after reset" );
            BOOST_REQUIRE_MESSAGE( (chr->loci() == 0), "Unexpected number of loci after reset");
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
