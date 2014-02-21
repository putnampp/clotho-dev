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

#ifndef CLOTHOEVENT_H_
#define CLOTHOEVENT_H_

#include "ClothoEventStub.h"
#include "SimulationObject.h"

template < class OBJ >
struct ModelHandler {
    virtual void updateModels( OBJ * ) const = 0;
};

class ClothoEvent : public DefaultEvent,
    public ModelHandler< SimulationObject > {
public:
    virtual const string & getDataType() const = 0;
    virtual unsigned int getEventSize() const = 0;
    virtual bool eventCompare( const Event * e ) = 0;

    virtual void updateModels( SimulationObject * o ) const {}

    virtual ~ClothoEvent() {}
protected:
    ClothoEvent( const VTime & tSend, const VTime &tRecv,
                 SimulationObject * sender,
                 SimulationObject * receiver ) :
        DefaultEvent( tSend, tRecv, sender, receiver ) {}

    ClothoEvent( const VTime & tSend, const VTime & tRecv,
                 const ObjectID &sender,
                 const ObjectID & receiver,
                 const unsigned int evtID ) :
        DefaultEvent( tSend, tRecv, sender, receiver, evtID ) {}

    ClothoEvent( const VTime & tSend, const VTime & tRecv,
                 const ObjectID &sender,
                 const ObjectID & receiver,
                 const EventId & evtID ) :
        DefaultEvent( tSend, tRecv, sender, receiver, evtID ) {}

    ClothoEvent( const ClothoEvent & ce ) :
        DefaultEvent( ce.getSendTime(), ce.getReceiveTime(),
                      ce.getSender(), ce.getReceiver(), ce.getEventId() ) {}
};

#endif  // CLOTHOEVENT_H_
