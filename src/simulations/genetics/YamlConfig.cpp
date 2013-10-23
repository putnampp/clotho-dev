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

#include "clothosim/ClothoModelCoordinator.h"

using std::ifstream;
using std::vector;

using std::cout;
using std::endl;

const string COUNT_K = "count";

YamlConfig::YamlConfig( const string & file ) :
    m_config( file )
{}

shared_ptr< vector< SimulationObject * > > YamlConfig::getSimulationObjects() {
    shared_ptr< vector< SimulationObject * > > objs( new vector< SimulationObject * >() );

    vector< YAML::Node > docs = YAML::LoadAllFromFile( m_config );

    cout << "\nFound " << docs.size() << " documents." << endl;
    for( vector< YAML::Node >::iterator it = docs.begin(); it != docs.end(); it++ ) {
        if( !it->IsMap() ) continue;

        if( (*it)[ OBJECT_K ] ) {
            cout << (*it) << "\n" << endl;
            unsigned int count = 1;
            if( (*it)[ COUNT_K ] ) {
                count = (*it)[ COUNT_K ].as< unsigned int >();
                if( count > 1 ) {
                    objs->reserve( objs->size() + count );
                }
            }

            for( unsigned int i = 0; i < count; ++i ) {
                SimulationObject * so = ClothoObjectManager::getInstance()->createObjectFrom( (*it) );

                if( so ) {
                    cout << (ClothoObject *)so << endl;
                    objs->push_back( so );
                }
            }

            cout << "SimulationObjects created: " << objs->size() << "( " << objs->capacity() << " )" << endl;
        } else if( (*it)[ MODEL_K ] ) {
            ClothoModel * cm = ClothoModelManager::getInstance()->createModelFrom( (*it) );
        }
    }

    objs->push_back( &*ClothoModelCoordinator::getInstance() );
    
    return objs;
}

YamlConfig::~YamlConfig() {}
