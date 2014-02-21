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

#include "ClothoModelCoordinator.h"

#include <iostream>
using std::cout;
using std::endl;

using std::pair;
using std::make_pair;

ClothoModelCoordinator::ClothoModelCoordinator() : ClothoObject(), m_name("coordinator") { }

ClothoModelCoordinator::~ClothoModelCoordinator() {
    m_models.clear();
}

shared_ptr< ClothoModelCoordinator > ClothoModelCoordinator::getInstance() {
    static shared_ptr< ClothoModelCoordinator > inst( new ClothoModelCoordinator() );
    return inst;
}

void ClothoModelCoordinator::initialize() { }

void ClothoModelCoordinator::finalize() { }

const string & ClothoModelCoordinator::getName() const {
    return m_name;
}

void ClothoModelCoordinator::executeProcess() { }

State * ClothoModelCoordinator::allocateState() {
    return NULL;
}

void ClothoModelCoordinator::print( ostream & out ) const { }

void ClothoModelCoordinator::addEventHandler( const string & name, shared_ptr< ClothoModel > handle) {
    m_models.insert( make_pair( name, handle ) );
}

void ClothoModelCoordinator::handleEvent( const Event * evt ) const {
    typedef pair< Models::const_iterator, Models::const_iterator > ModelRange;

    ModelRange mr = m_models.equal_range( evt->getDataType() );

    for( Models::const_iterator it = mr.first; it != mr.second; it++ ) {
        it->second->handle( evt );
    }

    mr = m_models.equal_range( ANY_EVENTS );

    for( Models::const_iterator it = mr.first; it != mr.second; it++ ) {
        it->second->handle( evt );
    }
}

void ClothoModelCoordinator::routeEvent( const Event * evt ) const {
    SimulationObject * so = getObjectHandle( &evt->getReceiver() );
    so->receiveEvent( evt );
}
