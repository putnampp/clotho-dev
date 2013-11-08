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

#ifndef SIMPLEEVENT_H_
#define SIMPLEEVENT_H_

#include "DefaultEvent.h"
#include "IntVTime.h"
#include "SerializedInstance.h"


class SimpleEvent : public DefaultEvent {
public:
    SimpleEvent( const VTime & initSend, const VTime & initRecv,
                 SimulationObject * initSender, SimulationObject * initReceiver ) :
        DefaultEvent( initSend, initRecv, initSender, initReceiver ) {

    }

    virtual ~SimpleEvent() {}

    static Serializable * deserialize( SerializedInstance * inst ) {
        VTime * tSent = dynamic_cast< VTime * >(inst->getSerializable());
        VTime * tRecv = dynamic_cast< VTime * >(inst->getSerializable());

        unsigned int sSimManID = inst->getUnsigned();
        unsigned int sSimObjID = inst->getUnsigned();
        unsigned int rSimManID = inst->getUnsigned();
        unsigned int rSimObjID = inst->getUnsigned();
        unsigned int eventID = inst->getUnsigned();

        ObjectID    sender(sSimObjID, sSimManID );
        ObjectID    receiver(rSimObjID, rSimManID );

        SimpleEvent * e = new SimpleEvent( *tSent, *tRecv, sender, receiver, eventID );

        delete tSent;
        delete tRecv;

        return e;
    }

    bool eventCompare( const Event * event ) {
        // SimpleEvent *e = (SimpleEvent *) event;

        return ( compareEvents( this, event ) );
    }

    void serialize( SerializedInstance * addTo ) const {
        Event::serialize(addTo);
    }

    unsigned int getEventSize() const {
        return sizeof(SimpleEvent);
    }

    static const string & getSimpleEventDataType() {
        static string simpleDataType = "SimpleEvent";
        return simpleDataType;
    }

    const string & getDataType() const {
        return getSimpleEventDataType();
    }

private:
    SimpleEvent( const VTime &tSend, const VTime &tRecv,
                 const ObjectID &sendID, const ObjectID &recvID,
                 const unsigned int eventID ) :
        DefaultEvent( tSend, tRecv, sendID, recvID, eventID ) {}
};

#endif  // SIMPLEEVENT_H_
