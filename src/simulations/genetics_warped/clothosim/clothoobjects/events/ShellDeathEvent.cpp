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

#include "ShellDeathEvent.h"
#include "SerializedInstance.h"

#include "../../ClothoModelCoordinator.h"

#include <iostream>

using std::cout;
using std::endl;

DEFINE_REGISTERED_CLOTHO_EVENT( ShellDeathEvent )

/*
 *
 * Since the Individual Shell knows what environment it is a member of
 * it is possible to directly add itself to the environment (m_environment->addIndividual)
 * and remove itself from the environment on death (m_environment->removeIndividual)
 *
 * This is instead of spawning events and sending them to the environment.
 *
 * By spawning events, if there are any models which are listening for
 * such events are notified though. Although those models could just as easily
 * be modified to listen for the IndividualShell + ShellBirthEvent, rather than
 * Environment2 + ShellBirthEvent. However, this creates a race condition as
 * an Individual may be dead, but the environment has not be notified of the
 * death. Therefore, a mating event may occur which requests the individual from
 * the environment. But because the individual is dead, the mating fails
 *
 */

template<>
void IndividualShell::handleEvent< ShellDeathEvent >( const ShellDeathEvent * e ) {
    if( m_prop )
        m_prop->m_eol = dynamic_cast< IntVTime * >( e->getReceiveTime().clone() );
//    m_environment->removeIndividual( this );
//
    Event * evt = new ShellDeathEvent( e->getReceiveTime(), e->getReceiveTime(), this, m_environment, this );
    m_environment->receiveEvent( evt );
}

template<>
void Environment2::handleEvent< ShellDeathEvent >( const ShellDeathEvent * e ) {
    this->removeIndividual( e->getIndividual() );
}

ShellDeathEvent::ShellDeathEvent( const VTime & tSend, const VTime &tRecv,
                                  SimulationObject * sender,
                                  SimulationObject * receiver,
                                  IndividualShell * ind
                                ) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_ind( ind ) {
}

ShellDeathEvent::ShellDeathEvent( const VTime & tSend, const VTime & tRecv,
                                  const ObjectID &sender,
                                  const ObjectID & receiver,
                                  const unsigned int evtID,
                                  IndividualShell * ind
                                ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_ind( ind ) {
}

ShellDeathEvent::ShellDeathEvent( const VTime & tSend, const VTime & tRecv,
                                  const ObjectID &sender,
                                  const ObjectID & receiver,
                                  const EventId & evtID,
                                  IndividualShell * ind
                                ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_ind(ind) {
}

ShellDeathEvent::ShellDeathEvent( const ShellDeathEvent & ce ) :
    ClothoEvent( ce.getSendTime(), ce.getReceiveTime(),
                 ce.getSender(), ce.getReceiver(),
                 ce.getEventId() ) {}


bool ShellDeathEvent::eventCompare( const Event * evt ) {
    const ShellDeathEvent * e = dynamic_cast< const ShellDeathEvent * >(evt);
    return (compareEvents( this, e ) );
}

void ShellDeathEvent::updateModels( IndividualShell * ind ) const {
    ind->handleEvent( this );

    ClothoModelCoordinator< IndividualShell, ShellDeathEvent >::getInstance()->handleEvent(this, ind );
}

void ShellDeathEvent::updateModels( Environment2 * env ) const {
    env->handleEvent( this );

    ClothoModelCoordinator< Environment2, ShellDeathEvent >::getInstance()->handleEvent(this, env );
}

IndividualShell * ShellDeathEvent::getIndividual() const {
    return m_ind;
}

ShellDeathEvent::~ShellDeathEvent() {}

DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( ShellDeathEvent ) {
    shared_ptr< VTime > tSend( dynamic_cast< VTime * >(inst->getSerializable()));
    shared_ptr< VTime > tRecv( dynamic_cast< VTime * >(inst->getSerializable()));

    unsigned int sSimManID = inst->getUnsigned();
    unsigned int sSimObjID = inst->getUnsigned();
    unsigned int rSimManID = inst->getUnsigned();
    unsigned int rSimObjID = inst->getUnsigned();
    unsigned int eventID = inst->getUnsigned();

    ObjectID send( sSimObjID, sSimManID );
    ObjectID recv( rSimObjID, rSimManID );

    ShellDeathEvent * e = new ShellDeathEvent( *tSend, *tRecv, send, recv, eventID, NULL );

    return e;
}
