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

#ifndef INDIVIDUALOBJECT_H_
#define INDIVIDUALOBJECT_H_

#include "warped.h"
#include "SimulationObject.h"
#include "SerializedInstance.h"

#include "common.h"
#include <vector>

#include "events/ClothoEvent.h"

#include "../ClothoObject.h"
#include "IndividualObjectState.h"
#include "IntVTime.h"

using std::vector;

class Individual : public ClothoObject {
public:
    Individual( );

    Individual( sex_t s, const AlleleGroupPtr genos);

    virtual ~Individual();

    void initialize();
    void finalize();

    void executeProcess();

    State * allocateState();
    const string & getName() const;

    void print( ostream & out ) const;

    template < class EVT >
    void handleEvent( const EVT * e ) {}

    sex_t   getSex() const;

protected:
//    void born();
//    void died( const DeathEvent * evt );
//    void died( );

private:
    string  m_name;

    sex_t   m_sex;
    unsigned int    m_offspring;
    IntVTime    * m_dob;
    IntVTime    * m_eol;

    ClothoObject *  m_environment;
};

//DECLARE_REGISTERED_CLOTHO_OBJECT( Individual )

#endif  // INDIVIDUALOBJECT_H_
