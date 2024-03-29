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

#include "YamlConfig.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "clothosim/ClothoObjectManager.h"
#include "clothosim/ClothoObject.h"
#include "clothosim/ClothoModelManager.h"
#include "clothosim/ClothoModel.h"

#include "clothosim/ClothoInitializerManager.h"

//#include "clothosim/ClothoModelCoordinator.h"

using std::ifstream;
using std::vector;

using std::cout;
using std::endl;

const string COUNT_K = "count";

YamlConfig::YamlConfig( const string & file ) :
    m_config( file ),
    m_objs( new vector< SimulationObject * >() ) {
}

shared_ptr< vector< SimulationObject * > > YamlConfig::getSimulationObjects() {
    //shared_ptr< vector< SimulationObject * > > objs( new vector< SimulationObject * >() );

    vector< YAML::Node > docs = YAML::LoadAllFromFile( m_config );

    cout << "\nFound " << docs.size() << " documents." << endl;
    for( vector< YAML::Node >::iterator it = docs.begin(); it != docs.end(); it++ ) {
        if( !it->IsMap() ) continue;

        if( (*it)[ OBJECT_K ] ) {
            cout << (*it) << "\n" << endl;
            string name = (*it)[ OBJECT_K ].as< string >();

            ClothoObjectManager< YAML::Node >::getInstance()->createObjectFrom(name, (*it), m_objs );
            cout << "SimulationObjects created: " << m_objs->size() << "( " << m_objs->capacity() << " )" << endl;
        } else if( (*it)[ MODEL_K ] ) {
            string name = (*it)[ MODEL_K ].as< string >();
            ClothoModelManager< YAML::Node >::getInstance()->createModelFrom( name, (*it) );
        } else if( (*it)[ INITIALIZER_K ] ) {
            string name = (*it)[ INITIALIZER_K ].as< string >();
            ClothoInitializerManager< YAML::Node >::getInstance()->initialize( name, (*it), m_objs );
        }
    }

    return m_objs;
}

void YamlConfig::cleanUp() {
    while(! m_objs->empty() ) {
        SimulationObject * to_delete = m_objs->back();
        m_objs->pop_back();

        delete to_delete;
    }
}

YamlConfig::~YamlConfig() {
    cleanUp();

    m_objs.reset();
}
