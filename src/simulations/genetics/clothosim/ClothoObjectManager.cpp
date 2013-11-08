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

#include "ClothoObjectManager.h"
#include "yaml-cpp/yaml.h"


ClothoObjectManager::ClothoObjectManager() {}

shared_ptr< ClothoObjectManager > ClothoObjectManager::getInstance() {
    static shared_ptr< ClothoObjectManager > inst( new ClothoObjectManager() );
    return inst;
}

void ClothoObjectManager::registerObject( SimObjectCreator * soc ) {
    m_creators[ soc->name() ] = soc;
}

SimulationObject * ClothoObjectManager::createObject( const string & name ) {
    iterator it = m_creators.find( name );

    if( it == m_creators.end() )
        return NULL;

    return it->second->createObject();
}

void ClothoObjectManager::createObjectFrom< YAML::Node >( const YAML::Node & n, shared_ptr< vector< SimulationObject * > > objs ) {
    if( n[ OBJECT_K ] ) {
        string name = n[ OBJECT_K ].as<string>();

        iterator it = m_creators.find( name );

        if( it != m_creators.end() ) {
            creatable< YAML::Node > * c = dynamic_cast< createable< YAML::Node > * >( (it->second));
            if( c )
                c->createObjectFrom(n, objs);
        }
    }
}

ClothoObjectManager::~ClothoObjectManager() {
    m_creators.clear();
}
