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

#ifndef BIRTHEVENT_H_
#define BIRTHEVENT_H_

#include "../common_types.h"
#include "ClothoEventStub.h"
#include "ClothoEvent.h"
#include "../IndividualObject.h"
#include "../EnvironmentObject.h"

class BirthEvent :
    public ClothoEvent,
    virtual public ModelHandler< Environment >,
        virtual public ModelHandler< Individual > {
public:
    BirthEvent( const VTime & tSend, const VTime &tRecv,
                SimulationObject * sender,
                SimulationObject * receiver,
                sex_t s );
    BirthEvent( const VTime & tSend, const VTime &tRecv,
                SimulationObject * sender,
                SimulationObject * receiver,
                sex_t s,
                const VTime & tBirth);
    BirthEvent( const VTime & tSend, const VTime & tRecv,
                const ObjectID &sender,
                const ObjectID & receiver,
                const unsigned int evtID,
                sex_t s );
    BirthEvent( const VTime & tSend, const VTime & tRecv,
                const ObjectID &sender,
                const ObjectID & receiver,
                const unsigned int evtID,
                sex_t s,
                const VTime & tBirth);

    BirthEvent( const VTime & tSend, const VTime & tRecv,
                const ObjectID &sender,
                const ObjectID & receiver,
                const EventId & evtID,
                sex_t s );
    BirthEvent( const VTime & tSend, const VTime & tRecv,
                const ObjectID &sender,
                const ObjectID & receiver,
                const EventId & evtID,
                sex_t s,
                const VTime & tBirth);
    BirthEvent( const BirthEvent & ce );
    virtual ~BirthEvent();

    virtual const string & getDataType() const;
    virtual unsigned int getEventSize() const;
    virtual bool eventCompare( const Event * e );

    virtual void updateModels( Individual * ) const;
    virtual void updateModels( Environment * ) const;

    sex_t getSex() const;
    const VTime & getBirthTime() const;

protected:
    const VTime * m_birth;
    sex_t m_sex;
};

DECLARE_REGISTERED_CLOTHO_EVENT( BirthEvent );

#endif  // BIRTHEVENT_H_

