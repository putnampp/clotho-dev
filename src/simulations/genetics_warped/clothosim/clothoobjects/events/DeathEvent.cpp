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

#include "DeathEvent.h"
#include "SerializedInstance.h"

#include <iostream>

using std::cout;
using std::endl;

DEFINE_REGISTERED_CLOTHO_EVENT( DeathEvent )

template<>
void Individual::handleEvent< DeathEvent >( const DeathEvent * e ) {
    m_eol = dynamic_cast< IntVTime * >( e->getReceiveTime().clone() );
    //this->print( cout );
    //
    Event * de = new DeathEvent( e->getSendTime(), e->getReceiveTime(), this, m_environment );

    m_environment->receiveEvent( de );
}

template<>
void Environment::handleEvent< DeathEvent >( const DeathEvent * e ) {

}

DeathEvent::DeathEvent( const VTime & tSend, const VTime &tRecv,
                        SimulationObject * sender,
                        SimulationObject * receiver ) :
    ClothoEvent( tSend, tRecv, sender, receiver ) {}

DeathEvent::DeathEvent( const VTime & tSend, const VTime & tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const unsigned int evtID ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ) {}

DeathEvent::DeathEvent( const VTime & tSend, const VTime & tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const EventId & evtID ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ) {}

DeathEvent::DeathEvent( const DeathEvent & ce ) :
    ClothoEvent( ce.getSendTime(), ce.getReceiveTime(),
                 ce.getSender(), ce.getReceiver(),
                 ce.getEventId() ) {}


bool DeathEvent::eventCompare( const Event * evt ) {
    const DeathEvent * e = dynamic_cast< const DeathEvent * >(evt);
    return (compareEvents( this, e ) );
}

void DeathEvent::updateModels( Individual * ind ) const {
    ind->handleEvent( this );
}

void DeathEvent::updateModels( Environment * env ) const {
    env->handleEvent( this );
}

DeathEvent::~DeathEvent() {}

DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( DeathEvent ) {
    shared_ptr< VTime > tSend( dynamic_cast< VTime * >(inst->getSerializable()));
    shared_ptr< VTime > tRecv( dynamic_cast< VTime * >(inst->getSerializable()));

    unsigned int sSimManID = inst->getUnsigned();
    unsigned int sSimObjID = inst->getUnsigned();
    unsigned int rSimManID = inst->getUnsigned();
    unsigned int rSimObjID = inst->getUnsigned();
    unsigned int eventID = inst->getUnsigned();

    ObjectID send( sSimObjID, sSimManID );
    ObjectID recv( rSimObjID, rSimManID );

    DeathEvent * e = new DeathEvent( *tSend, *tRecv, send, recv, eventID );

    return e;
}
