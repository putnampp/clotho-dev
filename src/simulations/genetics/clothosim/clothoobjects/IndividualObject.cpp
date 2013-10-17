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

#include "IndividualObject.h"

#include "events/BirthEvent.h"
#include "events/DeathEvent.h"

#include "IntVTime.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

using std::cout;
using std::endl;

DECLARE_REGISTERED_CLOTHO_OBJECT( Individual )

Individual::Individual() : 
    m_name( "IND"  +  boost::lexical_cast<string>( m_id ) ) {
}

Individual::Individual( const YAML::Node & n ) {
    try {
        m_name = n[ "name" ].as< string >();
    } catch ( ... ) {
        m_name = "IND";
        m_name.append( boost::lexical_cast<string>( m_id ) );
    }
}

Individual::Individual( sex_t s, const vector< genotype_t > & genos ) :
    m_name( "IND"  +  boost::lexical_cast<string>(m_id)) {

}

Individual::~Individual() {}

void Individual::initialize() {
    m_environment = dynamic_cast< ClothoObject * >( getObjectHandle( "ENV" ));

    if( !m_environment ) {
        abort();
    }

    born();
}

void Individual::finalize() {

}

void Individual::executeProcess() {
    IndividualObjectState * iso = static_cast< IndividualObjectState * >(getState());
    ASSERT( iso != NULL );

    while( haveMoreEvents() ) {
        const Event * evt = getEvent();
        if( evt->getDataType() == "DeathEvent" ) {
            cout << "Processing DeathEvent (" << getSimulationTime() << ") ... " << *evt << endl;
        }
    }
}

State * Individual::allocateState()  {
    return new IndividualObjectState( getSimulationTime() );
}

const string & Individual::getName() const {
    return m_name;
}

void Individual::born() {
    IntVTime recvTime = dynamic_cast< const IntVTime &>(getSimulationTime());
    Event * eBorn = new BirthEvent( recvTime, recvTime, this, m_environment );

    m_environment->receiveEvent( eBorn );
}

void Individual::died() {
}

void Individual::print( ostream & out ) const {
    out << m_name << "\n";
}
