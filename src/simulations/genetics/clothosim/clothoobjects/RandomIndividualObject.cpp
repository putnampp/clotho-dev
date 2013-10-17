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

#include "RandomIndividualObject.h"
#include "IndividualObjectState.h"

#include "events/clotho_events.h"

#include <gsl/gsl_rng.h>

template<>
class ClothoObjectCreator< RandomIndividual > : public SimObjectCreator {
public:
    ClothoObjectCreator( const char * name ) : m_name( name ), m_T(), m_rng(NULL) {
        ClothoObjectManager::getInstance()->registerObject( this );

        gsl_rng_env_setup();
        m_T = gsl_rng_default;
        m_rng = gsl_rng_alloc( m_T );
    }

    const string & name() {
        return m_name;
    }

    SimulationObject * createObject() {
        return new Individual();
    }

    SimulationObject * createObjectFrom( const YAML::Node & n) {
        try {
            unsigned int max_variants = n[ "max_variants" ].as< unsigned int >();
            unsigned int founder_variants = n[ "founder_variants" ].as< unsigned int > ();
            unsigned int rnd = gsl_rng_get( m_rng );

            sex_t s = (( rnd & 0x00000001 )? MALE : FEMALE );
            rnd >>= 1;
            unsigned int j = 1;
            const unsigned int MAX_J = (sizeof( unsigned int ) << 2);

            vector< allele_t > alleles;
            alleles.reserve( genotype_t::PLOIDY );

            vector< genotype_t > genotypes;
            genotypes.reserve( max_variants );
            for( unsigned int i = 0; i < founder_variants; ++i ) {
                alleles.clear();
                for( ploidy_t p = 0; p < genotype_t::PLOIDY; ++p ) {
                    if( j++ >= MAX_J ) {
                        rnd = gsl_rng_get( m_rng );
                        j = 1;
                    }
                    alleles.push_back( (allele_t) (rnd & 0x00000001));
                    rnd >>= 1;
                }

                genotype_t g( alleles );
                genotypes.push_back( g );
            }

            return new Individual( s, genotypes );
        } catch( ... ) {}

        return NULL;
    }

    virtual ~ClothoObjectCreator() {
        gsl_rng_free( m_rng );
    }
protected:
    const string m_name;
    const gsl_rng_type * m_T;
    gsl_rng * m_rng;
};

DECLARE_REGISTERED_CLOTHO_OBJECT( RandomIndividual )
