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
#include "common_types.h"
//#include "../ClothoModelCoordinator.h"

#include "events/BirthEvent.h"
#include "events/DeathEvent.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

using std::cout;
using std::endl;

const string IND_K = "IND";
const string NAME_K = "name";

//DEFINE_REGISTERED_CLOTHO_OBJECT( Individual )

Individual::Individual() :
    m_name( IND_K  +  boost::lexical_cast<string>( m_id ) ),
    m_sex( UNK_SEX ),
    m_offspring(0),
    m_dob( NULL ),
    m_eol( NULL ) {
}

/*
Individual::Individual( const YAML::Node & n ) {
    if( n[ NAME_K ] ) {
        m_name = n[ NAME_K ].as< string >();
    } else {
        m_name = IND_K +  boost::lexical_cast<string>( m_id );
    }

    if( n[ SEX_K ] ) {
        switch( n[ SEX_K ].as<unsigned int>() ) {
        case 0:
            m_sex = FEMALE;
            break;
        case 1:
            m_sex = MALE;
            break;
        default:
            m_sex = UNK_SEX;
            break;
        }
    } else {
        m_sex = UNK_SEX;
    }

    m_offspring = 0;
    m_dob = NULL;
    m_eol = NULL;
}
*/
Individual::Individual( sex_t s, const AlleleGroupPtr genos ) :
    m_name( "IND"  +  boost::lexical_cast<string>(m_id)),
    m_sex( s ),
    m_offspring(0),
    m_dob( NULL ),
    m_eol( NULL ) {

}

Individual::~Individual() {
    if( m_dob ) delete m_dob;
    if( m_eol ) delete m_eol;
}

void Individual::initialize() {
    m_environment = dynamic_cast< ClothoObject * >( getObjectHandle( "ENV" ));

    if( !m_environment ) {
        abort();
    }

    // notify self that has been born?
    Event * eBorn = new BirthEvent( getSimulationTime(), getSimulationTime(), this, this, m_sex );

    this->receiveEvent( eBorn );
}

void Individual::finalize() {

}

void Individual::executeProcess() {
    IndividualObjectState * iso = static_cast< IndividualObjectState * >(getState());
    ASSERT( iso != NULL );

    while( haveMoreEvents() ) {
        const Event * e = getEvent();
        if( !m_eol ) {
            const ModelHandler< Individual > * evt = dynamic_cast< const ModelHandler< Individual > * >(e);
            if( evt ) {
                evt->updateModels( this );
            } else {
                const ModelHandler< ClothoObject > * e2 = dynamic_cast< const ModelHandler< ClothoObject > * >( e );
                e2->updateModels( this );
            }
        }
    }
}

State * Individual::allocateState()  {
    return new IndividualObjectState( getSimulationTime(), m_sex );
}

const string & Individual::getName() const {
    return m_name;
}

sex_t Individual::getSex() const {
    return m_sex;
}

void Individual::print( ostream & out ) const {
    out << m_name
        << ", " << m_sex
        << ", " << m_offspring
        ;
    if( m_dob ) {
        if( m_eol ) {
            out << ", " << (*m_eol - *m_dob);
        } else {
            const IntVTime & tmp = static_cast< const IntVTime &>( getSimulationTime() );

            out << ", " << (tmp - *m_dob );
        }
    }
    out << "\n";
}
