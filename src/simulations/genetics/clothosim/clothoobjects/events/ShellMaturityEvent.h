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

#ifndef SHELLMATURITYEVENT_H_
#define SHELLMATURITYEVENT_H_

#include "ClothoEvent.h"
#include "ClothoEventStub.h"
#include "../IndividualShellObject.h"

//DECLARE_CLOTHO_EVENT( ShellMaturityEvent )
class ShellMaturityEvent : public ClothoEvent,
        public ModelHandler< IndividualShell > {
public:
    ShellMaturityEvent( const VTime & tSend, const VTime &tRecv,
                    SimulationObject * sender, 
                    SimulationObject * receiver );
    ShellMaturityEvent( const VTime & tSend, const VTime &tRecv,
                    SimulationObject * sender, 
                    SimulationObject * receiver,
                    const VTime & tMature );
    ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                    const ObjectID &sender, 
                    const ObjectID & receiver,
                    const unsigned int evtID );
    ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                    const ObjectID &sender, 
                    const ObjectID & receiver,
                    const unsigned int evtID,
                    const VTime & tMature );

    ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                    const ObjectID &sender, 
                    const ObjectID & receiver,
                    const EventId & evtID );
    ShellMaturityEvent( const VTime & tSend, const VTime & tRecv,
                    const ObjectID &sender, 
                    const ObjectID & receiver,
                    const EventId & evtID,
                    const VTime & tMature );

    ShellMaturityEvent( const ShellMaturityEvent & ce );

    virtual const string & getDataType() const;
    virtual unsigned int getEventSize() const;
    virtual bool eventCompare( const Event * e );

    virtual void updateModels( IndividualShell * ) const;

    const VTime & getAgeOfMaturity() const;

    virtual ~ShellMaturityEvent();
protected:
    const VTime * m_age_of_maturity;
};

DECLARE_REGISTERED_CLOTHO_EVENT( ShellMaturityEvent );

#endif  // SHELLMATURITYEVENT_H_
