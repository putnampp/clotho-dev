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

#include "ShellMaturityEvent.h"
#include "SerializedInstance.h"

#include "../../ClothoModelCoordinator.h"

#include <iostream>

using std::cout;
using std::endl;

DEFINE_REGISTERED_CLOTHO_EVENT( ShellMaturityEvent )

template<>
void IndividualShell::handleEvent< ShellMaturityEvent >( const ShellMaturityEvent * e ) {
//    cout << "Individual reached maturity at " << e->getReceiveTime() << " (" << getSimulationTime() << ")" << endl;
    if( m_prop )
        m_prop->m_isMature = true;
}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime &tRecv,
                                        SimulationObject * sender,
                                        SimulationObject * receiver ) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_age_of_maturity( tRecv.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime &tRecv,
                                        SimulationObject * sender,
                                        SimulationObject * receiver,
                                        const VTime & tMature ) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_age_of_maturity( tMature.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                                        const ObjectID &sender,
                                        const ObjectID & receiver,
                                        const unsigned int evtID ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_age_of_maturity( tRecv.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                                        const ObjectID &sender,
                                        const ObjectID & receiver,
                                        const unsigned int evtID,
                                        const VTime & tMature ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_age_of_maturity( tMature.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                                        const ObjectID &sender,
                                        const ObjectID & receiver,
                                        const EventId & evtID ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_age_of_maturity( tRecv.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                                        const ObjectID &sender,
                                        const ObjectID & receiver,
                                        const EventId & evtID,
                                        const VTime & tMature ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_age_of_maturity( tMature.clone() ) {}

ShellMaturityEvent::ShellMaturityEvent( const ShellMaturityEvent & ce ) :
    ClothoEvent( ce.getSendTime(), ce.getReceiveTime(),
                 ce.getSender(), ce.getReceiver(),
                 ce.getEventId() ),
    m_age_of_maturity( ce.getAgeOfMaturity().clone() ) {}


bool ShellMaturityEvent::eventCompare( const Event * evt ) {
    const ShellMaturityEvent * e = dynamic_cast< const ShellMaturityEvent * >(evt);
    return (compareEvents( this, e ) );
}

const VTime & ShellMaturityEvent::getAgeOfMaturity() const {
    return * m_age_of_maturity;
}

void ShellMaturityEvent::updateModels( IndividualShell * ind ) const {
    ind->handleEvent(this);

    ClothoModelCoordinator< IndividualShell, ShellMaturityEvent >::getInstance()->handleEvent( this, ind );
}

ShellMaturityEvent::~ShellMaturityEvent() {
    delete m_age_of_maturity;
}

DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( ShellMaturityEvent ) {
    shared_ptr< VTime > tSend( dynamic_cast< VTime * >(inst->getSerializable()));
    shared_ptr< VTime > tRecv( dynamic_cast< VTime * >(inst->getSerializable()));

    unsigned int sSimManID = inst->getUnsigned();
    unsigned int sSimObjID = inst->getUnsigned();
    unsigned int rSimManID = inst->getUnsigned();
    unsigned int rSimObjID = inst->getUnsigned();
    unsigned int eventID = inst->getUnsigned();

    ObjectID send( sSimObjID, sSimManID );
    ObjectID recv( rSimObjID, rSimManID );

    ShellMaturityEvent * e = new ShellMaturityEvent( *tSend, *tRecv, send, recv, eventID );

    return e;
}
