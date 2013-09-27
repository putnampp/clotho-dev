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

#include <boost/lexical_cast.hpp>

BOOST_AUTO_TEST_SUITE( test_genome_factory )

const chromid_t chrom = 23;
const ploidy_t  ploid = 2;

void build_basic_gf( GenomeFactory & g ) {
    for( chromid_t i = 0; i < chrom; ++i ) {
        ChromosomePtr c( new Chromosome( "chr" + boost::lexical_cast<string>(i) ) );
        g.addChromosome( c );
    }
}

BOOST_AUTO_TEST_CASE( genome_factory_create ) {
    GenomeFactory g;

    BOOST_REQUIRE_MESSAGE( g.size() == 0, "Unexpected size of genome");
}

BOOST_AUTO_TEST_CASE( genome_factory_create2 ) {
    GenomeFactory g;

    ChromosomePtr c( new Chromosome( "chr0") );

    g.addChromosome( c );

    BOOST_REQUIRE_MESSAGE( g.chromosomes() == 1, "Individual chromosome not added" );

    vector< ChromosomePtr > chrs;
    for( chromid_t i = 1; i < chrom; ++i ) {
        chrs.push_back( ChromosomePtr( new Chromosome( "chr" + boost::lexical_cast<string>(i) ) ) );
    }

    g.addChromosome( chrs );

    BOOST_REQUIRE_MESSAGE( g.chromosomes() == chrom, "Unexpected number of chromosomes");

    BOOST_REQUIRE_MESSAGE( (g.size() == (chrom * DEFAULT_CHROMOSOME_LEN)), "Unexpected length of the default genome: " << (g.size()));
    BOOST_REQUIRE_MESSAGE( (g.loci() == 0), "Unexpected number of loci in default genome");
    chrs.clear();
}

BOOST_AUTO_TEST_CASE( genome_factory_reset ) {
    // add a site of interest to each chromosome
    // site pos is unimportant, just needs to be smaller
    // than the chrom_len
    GenomeFactory g;

    build_basic_gf( g );
    
    for( chromid_t c = 0; c < chrom; ++c ) {
        size_t before = g.getChromosomeByIndex(c)->loci();

        if( g.addChromosomeSite( c, (size_t) 33 ) ) {
            BOOST_REQUIRE_MESSAGE( (g.getChromosomeByIndex(c)->loci() == before + 1), "Site was not added");
        } else {
            BOOST_REQUIRE_MESSAGE( false, "Failed to add site on chromosome (" << (int)c << ")");
        }
    }

    // reset the GenomeFactory to default
    g.reset();

    for( chromid_t c = 0; c < chrom; ++c ) {
        ChromosomePtr chr = g.getChromosomeByIndex(c);

        BOOST_REQUIRE_MESSAGE( (chr->length() == DEFAULT_CHROMOSOME_LEN), "Unexpected size of chromosome after reset" );
        BOOST_REQUIRE_MESSAGE( (chr->loci() == 0), "Unexpected number of loci after reset");
    }
}
/*
BOOST_AUTO_TEST_CASE( genome_factory_chrom_sizes ) {
    GenomeFactory g(chrom);

    const size_t new_size = 3000;
    for( chromid_t c = 0; c < chrom; ++c ) {
        g.setChromosomeSize( c, new_size );
    }

    BOOST_REQUIRE_MESSAGE( g.size() == new_size * chrom, "Unexpected genome size after resizing");
}
*/
BOOST_AUTO_TEST_CASE( genome_factory_chrom_site ) {
    GenomeFactory g;

    build_basic_gf( g );

    size_t genome_size = g.size();
    const size_t locus_pos = DEFAULT_CHROMOSOME_LEN + 10;

    g.addChromosomeSite( 1, locus_pos );

    if( locus_pos >= DEFAULT_CHROMOSOME_LEN ) {
        BOOST_REQUIRE_MESSAGE( (g.size() - 1 - locus_pos + DEFAULT_CHROMOSOME_LEN == genome_size),
        "Unexpected genome size: " << g.size() << " was " << genome_size );
    } else {
        BOOST_REQUIRE_MESSAGE( (g.size() == genome_size), "Genome Size should not have changed when site " << locus_pos << " was added");
    }
}

BOOST_AUTO_TEST_SUITE_END()
