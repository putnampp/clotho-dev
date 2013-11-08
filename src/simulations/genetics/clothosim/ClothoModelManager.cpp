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

#include "ClothoModelManager.h"

const string MODEL_K = "model";

ClothoModelManager::ClothoModelManager() {}

shared_ptr< ClothoModelManager > ClothoModelManager::getInstance() {
    static shared_ptr< ClothoModelManager > inst( new ClothoModelManager() );
    return inst;
}

void ClothoModelManager::registerModel( SimModelCreator * smc ) {
    m_creators[ smc->name() ] = smc;
}

void ClothoModelManager::createModel( const string & name ) {
    iterator it = m_creators.find( name );

    if( it != m_creators.end() )
        it->second->createModel();
}

void ClothoModelManager::createModelFrom( const YAML::Node & n ) {
    if( n[ MODEL_K ] ) {
        string name = n[ MODEL_K ].as<string>();

        iterator it = m_creators.find( name );

        if( it != m_creators.end() )
            it->second->createModelFrom(n);
    }
}

ClothoModelManager::~ClothoModelManager() {
    m_creators.clear();
}
