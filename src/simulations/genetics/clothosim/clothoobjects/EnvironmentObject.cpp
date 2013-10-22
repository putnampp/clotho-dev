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

#include "EnvironmentObject.h"
#include "EnvironmentObjectState.h"

#include "IntVTime.h"

#include "events/BirthEvent.h"
#include "events/DeathEvent.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

using std::cout;
using std::endl;

template<>
void Environment::handleEvent< BirthEvent >( const BirthEvent * evt ) {
    // upon receiving a birth event we should predict a DeathEvent
    // for the sender
    //
    SimulationObject * sender = getObjectHandle( &(evt->getSender()) );
    IntVTime tDeath = dynamic_cast< const IntVTime & >( evt->getSendTime() ) + 90;

    switch( evt->getSex() ) {
    case FEMALE:
        m_females.push_back( OBJECT_ID(evt->getSender()) );
        break;
    case MALE:
        m_males.push_back( OBJECT_ID(evt->getSender()) );
        break;
    default:
        break;
    }

    Event * dEvent = new DeathEvent( getSimulationTime(), tDeath, this, sender );

    sender->receiveEvent( dEvent );
}

Environment::Environment( ) : m_name( "ENV" ) {}

Environment::Environment( const YAML::Node & n) : m_name( "ENV" ) {
}

Environment::~Environment() {}
void Environment::initialize(){}
void Environment::finalize(){}

void Environment::executeProcess(){
    while( haveMoreEvents() ) {
        const Event * evt = getEvent();
        if( evt->getDataType() == "BirthEvent" ) {
            cout << "Processing Birth Event (" << getSimulationTime() << ") ..." << *evt << endl;

            const BirthEvent * eB = dynamic_cast< const BirthEvent *>(evt);
            handleEvent( eB );
        }
    }
}
/*
template<>
void Environment::handleEvent< BirthEvent >( const BirthEvent * evt ) {
    // upon receiving a birth event we should predict a DeathEvent
    // for the sender
    //
    SimulationObject * sender = getObjectHandle( evt->getSender() );
    IntVTime death = dynamic_cast< const IntVTime & >( evt->getSendTime() + 90 );

    Event * dEvent = new DeathEvent( getSimulationTime(), death, this, sender );

    sender->receiveEvent( dEvent );
}
*/
    
State * Environment::allocateState() {
    return new EnvironmentObjectState(NULL, NULL);
}

const string & Environment::getName() const {
    return m_name;
}

void Environment::print( ostream & out ) const {
    out << m_name << "\n";
}

DEFINE_REGISTERED_CLOTHO_OBJECT( Environment )
