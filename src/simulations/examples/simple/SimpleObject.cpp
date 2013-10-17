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

#include "SimpleObject.h"
#include "SimpleState.h"

#include <boost/lexical_cast.hpp>

SimpleObject::SimpleObject(  ) : m_id(nextID()), m_name( "name" + boost::lexical_cast<string>(m_id) ) {}

SimpleObject::~SimpleObject() {}

void SimpleObject::initialize() { }

void SimpleObject::finalize() { 
    SEVERITY sev = NOTE;

    SimpleState * s = static_cast< SimpleState * >( getState() );
    ASSERT( s != 0 );

    string msg = "Getting here\n";
    reportError( msg, sev );
}

void SimpleObject::executeProcess() {
    SimpleState * myState = static_cast< SimpleState * >( getState() );
    ASSERT( myState != 0 );
}

State * SimpleObject::allocateState() {
    return new SimpleState();
}

void SimpleObject::deallocateState( const State * state ) {
    delete state;
}

void SimpleObject::reclaimEvent( const Event * e ) {
    delete e;
}

int SimpleObject::nextID() {
    static int id = 0;
    return id++;
}

const string &SimpleObject::getName() const {
    return m_name;
}

void SimpleObject::sendEvent( const string & owner ) {
}

