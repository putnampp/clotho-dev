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

#ifndef SHELLMATINGEVENT_H_
#define SHELLMATINGEVENT_H_

#include "ClothoEvent.h"
#include "ClothoEventStub.h"
#include "../IndividualShellObject.h"
#include "../Environment2.h"

class ShellMatingEvent : public ClothoEvent,
    public ModelHandler< Environment2 > {
public:
    ShellMatingEvent( const VTime & tSend, const VTime &tRecv,
                      SimulationObject * sender,
                      SimulationObject * receiver,
                      IndividualShell * is );
    ShellMatingEvent( const VTime & tSend, const VTime & tRecv,
                      const ObjectID &sender,
                      const ObjectID & receiver,
                      const unsigned int evtID,
                      IndividualShell * is);

    ShellMatingEvent( const VTime & tSend, const VTime & tRecv,
                      const ObjectID &sender,
                      const ObjectID & receiver,
                      const EventId & evtID,
                      IndividualShell * is );

    ShellMatingEvent( const ShellMatingEvent & ce );

    virtual const string & getDataType() const;
    virtual unsigned int getEventSize() const;
    virtual bool eventCompare( const Event * e );

    IndividualShell * getFirstPartner() const;

    virtual void updateModels( Environment2 * ) const;

    virtual ~ShellMatingEvent();
protected:
    IndividualShell * m_first_partner;
};

DECLARE_REGISTERED_CLOTHO_EVENT( ShellMatingEvent );

#endif  // SHELLMATINGEVENT_H_

