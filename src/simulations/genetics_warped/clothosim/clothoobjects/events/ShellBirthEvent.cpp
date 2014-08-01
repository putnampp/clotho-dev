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

#include "ShellBirthEvent.h"

#include "../../ClothoModelCoordinator.h"

#include <iostream>

using std::cout;
using std::endl;

DEFINE_REGISTERED_CLOTHO_EVENT( ShellBirthEvent )

template<>
void IndividualShell::handleEvent< ShellBirthEvent >( const ShellBirthEvent * e ) {
    //cout << getSimulationTime() << ":" << e->getSendTime() << " -> " << e->getReceiveTime() << " (" << *e->getBirthTime() << ")" << endl;
    if( m_prop )
        m_prop->m_dob = dynamic_cast< IntVTime * >(e->getBirthTime()->clone());

//    m_environment->addIndividual( this );
    Event * evt = new ShellBirthEvent( e->getReceiveTime(), e->getReceiveTime(), this, m_environment, this );
    m_environment->receiveEvent( evt );
}

template<>
void Environment2::handleEvent< ShellBirthEvent >( const ShellBirthEvent * e ) {
    this->addIndividual( e->getIndividual() );
}

ShellBirthEvent::ShellBirthEvent( const VTime & tSend, const VTime &tRecv,
                                  SimulationObject * sender,
                                  SimulationObject * receiver,
                                  IndividualShell * ind ) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_birth( dynamic_cast< IntVTime * >( tRecv.clone() )),
    m_ind( ind ) {
}

ShellBirthEvent::ShellBirthEvent( const VTime & tSend,
                                  const VTime &tRecv,
                                  ObjectID & sender,
                                  ObjectID & receiver,
                                  unsigned int evtID,
                                  IndividualShell * ind ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_birth( dynamic_cast< IntVTime * >( tRecv.clone() )),
    m_ind( ind ) {
}

bool ShellBirthEvent::eventCompare( const Event * evt ) {
    const ShellBirthEvent * e = dynamic_cast< const ShellBirthEvent * >(evt);
    return (compareEvents( this, e ) );
}

void ShellBirthEvent::updateModels( IndividualShell * ishell ) const {
    ishell->handleEvent( this );

    ClothoModelCoordinator< IndividualShell, ShellBirthEvent >::getInstance()->handleEvent( this, ishell );
}

void ShellBirthEvent::updateModels( Environment2 * env ) const {
    env->handleEvent( this );

    ClothoModelCoordinator< Environment2, ShellBirthEvent >::getInstance()->handleEvent( this, env );
}

IntVTime * ShellBirthEvent::getBirthTime() const {
    return m_birth;
}

IndividualShell *  ShellBirthEvent::getIndividual() const {
    return m_ind;
}

ShellBirthEvent::~ShellBirthEvent() {}

DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( ShellBirthEvent ) {
    shared_ptr< VTime > tSend( dynamic_cast< VTime * >(inst->getSerializable()));
    shared_ptr< VTime > tRecv( dynamic_cast< VTime * >(inst->getSerializable()));

    unsigned int sSimManID = inst->getUnsigned();
    unsigned int sSimObjID = inst->getUnsigned();
    unsigned int rSimManID = inst->getUnsigned();
    unsigned int rSimObjID = inst->getUnsigned();
    unsigned int eventID = inst->getUnsigned();

    ObjectID send( sSimObjID, sSimManID );
    ObjectID recv( rSimObjID, rSimManID );

    ShellBirthEvent * e = new ShellBirthEvent( *tSend, *tRecv, send, recv, eventID, NULL );

    return e;
}
