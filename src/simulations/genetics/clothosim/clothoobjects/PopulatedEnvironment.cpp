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

#include "PopulatedEnvironment.h"
#include "Environment2.h"
#include "IndividualShellObject.h"

#include "events/clotho_events.h"

#include <gsl/gsl_rng.h>

#include <iostream>
#include <time.h>

using std::cout;
using std::endl;

/*
const string MAX_VARIANTS_K = "max_variants";
const string VARIANTS_K = "variants";
const string POP_SIZE_K = "pop_size";
const string PERCENT_FEMALE_K = "percent_female";

#define DEFAULT_MAX_VARIANTS 100
#define DEFAULT_ANC_VARIANTS 10
#define DEFAULT_POP_SIZE    10
*/
const string NAME_K = "name";
const string POOL_SIZE_K = "pool_size";
//const string INIT_POP_K = "initial_population";

const string DEFAULT_NAME = "ENV";

template<>
class ClothoObjectCreator< PopulatedEnvironment, YAML::Node > : public SimObjectCreator< YAML::Node > {
public:
    ClothoObjectCreator( const char * name ) : m_name( name ) {
        ClothoObjectManager< YAML::Node >::getInstance()->registerObject( this );
    }

    const string & name() {
        return m_name;
    }

    SimulationObject * createObject() {
        return new Environment2(DEFAULT_NAME.c_str());
    }

    void createObjectFrom( const YAML::Node & n, shared_ptr< vector< SimulationObject * > > objs ) {
        int pool_size = -1;
        if( n[ POOL_SIZE_K ] ) {
            pool_size = n[ POOL_SIZE_K ].as< int > ();
        }

        string name = DEFAULT_NAME;
        if( n[ NAME_K ] ) {
            name = n[ NAME_K ].as< string >();
        }
        Environment2 * env = new Environment2( name.c_str(), pool_size );

        int allocated = 0;
        while( ++allocated <= pool_size ) {
            IndividualShell * ishell = new IndividualShell( env, new IndividualProperties() );

            env->addIndividual( ishell );
            objs->push_back( ishell );

        }
        objs->push_back( env );
    }

    virtual ~ClothoObjectCreator() {}
protected:
/*
    int initializePopulation( const YAML::Node & n, Environment2 * env, shared_ptr< vector< SimulationObject * > > objs ) {
        shared_ptr< vector< SimulationObject * > > pop_objs( new vector< SimulationObject *>() );

        int allocated = 0;
        if( n[ OBJECT_K ] ) {
            string name = n[ OBJECT_K ].as<string>();
            ClothoObjectManager< YAML::Node >::getInstance()->createObjectFrom( name, n[INIT_POP_K], pop_objs );

            for( vector< SimulationObject * >::iterator it = pop_objs->begin(); it != pop_objs->end(); it++ ) {
                IndividualShell * ishell = dynamic_cast< IndividualShell * >( *it );
                if( ishell ) {
                    env->addIndividual( ishell );
                    objs->push_back( ishell );
                    ++allocated;
                }
            }
        }
        return allocated;
    }
*/
    
    const string m_name;
};

DEFINE_REGISTERED_CLOTHO_OBJECT( PopulatedEnvironment, YAML::Node )
