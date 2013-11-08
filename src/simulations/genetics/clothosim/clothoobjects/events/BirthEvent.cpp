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

#include "BirthEvent.h"
#include "SerializedInstance.h"

#include "../../ClothoModelCoordinator.h"

DEFINE_REGISTERED_CLOTHO_EVENT( BirthEvent )

template<>
void Individual::handleEvent< BirthEvent >( const BirthEvent * e ) {
    // update the date of birth
    m_dob = dynamic_cast< IntVTime *>(e->getBirthTime().clone());

    // send a notification to the environment that an individual has been born
    Event * be = new BirthEvent( *m_dob, *m_dob, this, m_environment, m_sex );

    m_environment->receiveEvent( be );
}

template<>
void Environment::handleEvent< BirthEvent >( const BirthEvent * bEvt ) {
    switch( bEvt->getSex() ) {
    case FEMALE:
        m_females.push_back( OBJECT_ID(bEvt->getSender()) );
        break;
    case MALE:
        m_males.push_back( OBJECT_ID(bEvt->getSender()) );
        break;
    default:
        break;
    }
}

BirthEvent::BirthEvent( const VTime & tSend, const VTime &tRecv,
                        SimulationObject * sender,
                        SimulationObject * receiver,
                        sex_t s ) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_birth( tSend.clone() ),
    m_sex( s ) {}

BirthEvent::BirthEvent( const VTime & tSend, const VTime &tRecv,
                        SimulationObject * sender,
                        SimulationObject * receiver,
                        sex_t sex,
                        const VTime & tBirth) :
    ClothoEvent( tSend, tRecv, sender, receiver ),
    m_birth( tBirth.clone() ),
    m_sex( sex ) {}

BirthEvent::BirthEvent( const VTime & tSend, const VTime & tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const unsigned int evtID,
                        sex_t sex ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_birth( tSend.clone() ),
    m_sex( sex ) {}

BirthEvent::BirthEvent( const VTime & tSend, const VTime &tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const unsigned int evtID,
                        sex_t sex,
                        const VTime & tBirth ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_birth( tBirth.clone() ),
    m_sex( sex ) {}

BirthEvent::BirthEvent( const VTime & tSend, const VTime & tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const EventId & evtID,
                        sex_t sex ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_birth( tSend.clone() ),
    m_sex( sex ) {}

BirthEvent::BirthEvent( const VTime & tSend, const VTime &tRecv,
                        const ObjectID &sender,
                        const ObjectID & receiver,
                        const EventId & evtID,
                        sex_t sex,
                        const VTime & tBirth ) :
    ClothoEvent( tSend, tRecv, sender, receiver, evtID ),
    m_birth( tBirth.clone() ),
    m_sex( sex ) {}

BirthEvent::BirthEvent( const BirthEvent & ce ) :
    ClothoEvent( ce.getSendTime(), ce.getReceiveTime(),
                 ce.getSender(), ce.getReceiver(),
                 ce.getEventId() ),
    m_birth( ce.getBirthTime().clone() ),
    m_sex( ce.m_sex) {}

sex_t BirthEvent::getSex() const {
    return m_sex;
}

const VTime & BirthEvent::getBirthTime() const {
    return  *m_birth;
}

bool BirthEvent::eventCompare( const Event * evt ) {
    const BirthEvent * e = dynamic_cast< const BirthEvent * >(evt);
    return (compareEvents( this, e ) );
}

void BirthEvent::updateModels( Individual * ind ) const {
    ind->handleEvent( this );

    ClothoModelCoordinator< Individual, BirthEvent >::getInstance()->handleEvent( this, ind );
}

void BirthEvent::updateModels( Environment * env ) const {
    env->handleEvent( this );

    ClothoModelCoordinator< Environment, BirthEvent >::getInstance()->handleEvent( this, env );
}

BirthEvent::~BirthEvent() {}

DEFINE_CLOTHO_EVENT_DESERIALIZATION_METHOD( BirthEvent ) {
    shared_ptr< VTime > tSend( dynamic_cast< VTime * >(inst->getSerializable()));
    shared_ptr< VTime > tRecv( dynamic_cast< VTime * >(inst->getSerializable()));

    unsigned int sSimManID = inst->getUnsigned();
    unsigned int sSimObjID = inst->getUnsigned();
    unsigned int rSimManID = inst->getUnsigned();
    unsigned int rSimObjID = inst->getUnsigned();
    unsigned int eventID = inst->getUnsigned();

    sex_t sex = static_cast< sex_t > (inst->getUnsigned());

    ObjectID send( sSimObjID, sSimManID );
    ObjectID recv( rSimObjID, rSimManID );

    BirthEvent * e = new BirthEvent( *tSend, *tRecv, send, recv, eventID, sex );

    return e;
}

