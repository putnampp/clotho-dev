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

#include "PopulationInitializer.h"
#include "Environment2.h"
#include "IndividualShellObject.h"

#include <gsl/gsl_rng.h>

#include <iostream>
#include <time.h>

using std::cout;
using std::endl;

const string ENVIRONMENT_K = "environment";
const string MAX_VARIANTS_K = "max_variants";
const string VARIANTS_K = "variants";
const string POP_SIZE_K = "pop_size";
const string PERCENT_FEMALE_K = "percent_female";

#define DEFAULT_MAX_VARIANTS 100
#define DEFAULT_ANC_VARIANTS 10
#define DEFAULT_POP_SIZE 10

template<>
class ClothoInitializerCreator< PopulationInitializer, YAML::Node > : public InitializerCreator< YAML::Node > {
public:
    ClothoInitializerCreator( const char * name ) : m_name( name ), m_T(), m_rng(NULL) {
        ClothoInitializerManager< YAML::Node >::getInstance()->registerInitializer( this );

        gsl_rng_env_setup();
        m_T = gsl_rng_default;
        m_rng = gsl_rng_alloc( m_T );

        long seed = time(NULL);
        gsl_rng_set( m_rng, seed );
    }

    const string & name() {
        return m_name;
    }

    void initialize( const YAML::Node & n, shared_ptr< vector< SimulationObject * > > objs ) {
        if( !objs ) return;

        string env_name = "";
        if( n[ ENVIRONMENT_K ] ) {
            env_name = n[ ENVIRONMENT_K ].as<string>();
        } else {
            abort();
        }
/*
        unsigned int max_variants = DEFAULT_MAX_VARIANTS;
        if( n[ MAX_VARIANTS_K ] ) {
            max_variants = n[ MAX_VARIANTS_K ].as< unsigned int >();
        }

        unsigned int variants = DEFAULT_ANC_VARIANTS;
        if( n[ VARIANTS_K ] ) {
            variants = n[ VARIANTS_K ].as< unsigned int > ();
        }
*/
        unsigned int pop_size = DEFAULT_POP_SIZE;
        if( n[ POP_SIZE_K ] ) {
            pop_size = n[ POP_SIZE_K ].as< unsigned int >();
        }

        unsigned int female_count = (pop_size >> 1);
        if( n[ PERCENT_FEMALE_K ] ) {
            unsigned int percent = n[ PERCENT_FEMALE_K ].as< unsigned int >();
            if( percent > 100 ) {
                percent = 100;
            }

            female_count = (pop_size * percent) / 100;
        }

        // find the Environment from which the individuals are being initialized
        Environment2 * env = NULL;
        for( vector< SimulationObject * >::reverse_iterator it = objs->rbegin(); it != objs->rend(); it++ ) {
            env = dynamic_cast< Environment2 * >( (*it) );
            if( env ) {
                if( env->getName() == env_name ) {
                    break;
                } else {
                    env = NULL;
                }
            }
        }

        if( !env )  abort();    // could not find the environment

        for( unsigned int i = 0; i < female_count; i++ ) {
            IndividualShell * ishell = env->nextAvailableIndividual();
            if( ishell ) {
                generateGenotypes( ishell->getProperties()->m_genos, env->getGeneticMap()->getLociCount() );
                ishell->getProperties()->m_sex = FEMALE;
            }
        }

        for( unsigned int i = female_count; i < pop_size; i++ ) {
            IndividualShell * ishell = env->nextAvailableIndividual();
            if( ishell ) {
                generateGenotypes( ishell->getProperties()->m_genos, env->getGeneticMap()->getLociCount() );
                ishell->getProperties()->m_sex = MALE;
            }
        }
    }

    virtual ~ClothoInitializerCreator() {
        gsl_rng_free( m_rng );
    }
protected:

    void generateGenotypes( AlleleGroupPtr genotypes, unsigned int count ) {
        const unsigned int MAX_J = (sizeof( unsigned int ) << 2);
        unsigned int rnd = gsl_rng_get( m_rng );
        unsigned int j = 1;
        for( unsigned int i = 0; i < count; ++i ) {
            genotype_t g;
            for( ploidy_t p = 0; p < g.max_size(); ++p ) {
                if( j++ >= MAX_J ) {
                    rnd = gsl_rng_get( m_rng );
                    j = 1;
                }
                g[p] = (allele_t) (rnd & 0x00000001);
                rnd >>= 1;
            }

            genotypes->push_back( g );
        }
    }

    const string m_name;
    const gsl_rng_type * m_T;
    gsl_rng * m_rng;
};

DEFINE_REGISTERED_INITIALIZER( PopulationInitializer, YAML::Node )

