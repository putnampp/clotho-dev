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

#include "IndividualShellObject.h"
#include "common_types.h"

#include "events/ShellBirthEvent.h"
#include "events/DeathEvent.h"

#include <boost/lexical_cast.hpp>

#include "Environment2.h"

#include <iostream>

using std::cout;
using std::endl;

const string IND_K = "IND";
const string NAME_K = "name";

/*
IndividualShell::IndividualShell() : ClothoObject(),
    m_name( IND_K  ),
    m_prop( NULL ),
    m_environment( NULL ) {
    m_name.append( boost::lexical_cast< string >(m_id ));
}
*/

IndividualShell::IndividualShell( Environment2 * env /*, IndividualProperties * p*/ ) :
    ClothoObject(),
    m_name( IND_K ),
    m_environment( env ),
    m_prop( new IndividualProperties( m_environment->getGeneticMap()->createLociAlleles() ) ) {
    m_name.append( boost::lexical_cast< string >( m_id ) );
}

IndividualShell::~IndividualShell() {
//    setProperties( NULL );
    delete m_prop;
}

void IndividualShell::initialize() {
    if( !m_environment ) {
        abort();    // Environment should be specified, before object is initialized
    }

    // notify self that has been born?
    if( m_prop->m_sex != UNASSIGNED ) {
        Event * eBorn = new ShellBirthEvent( getSimulationTime(), getSimulationTime(), this, this );
        this->receiveEvent( eBorn );
    }
}

void IndividualShell::finalize() {

}

void IndividualShell::executeProcess() {
//    IndividualShellObjectState * iso = static_cast< IndividualShellObjectState * >(getState());
//    ASSERT( iso != NULL );

    while( haveMoreEvents() ) {
        const Event * e = getEvent();
        if( isAlive() ) {
            const ModelHandler< IndividualShell > * evt = dynamic_cast< const ModelHandler< IndividualShell > * >(e);
            if( evt ) {
                evt->updateModels( this );
            }
            const ModelHandler< ClothoObject > * e2 = dynamic_cast< const ModelHandler< ClothoObject > * >( e );
            if( e2 ) {
                e2->updateModels( this );
            }
        }
    }
}

State * IndividualShell::allocateState()  {
    //return new IndividualObjectState( getSimulationTime(), m_sex );
    return NULL;
}

const string & IndividualShell::getName() const {
    return m_name;
}

bool IndividualShell::isAlive() const {
    return m_prop && !m_prop->m_eol;
}

sex_t IndividualShell::getSex() const {
    return m_prop->m_sex;
}

IntVTime * IndividualShell::getBirthTime() const {
    return m_prop->m_dob;
}

void IndividualShell::setEnvironment( Environment2 * env ) {
    m_environment = env;
}

Environment2 * IndividualShell::getEnvironment() const {
    return m_environment;
}

/*
void IndividualShell::setProperties( IndividualProperties * prop ) {
    //cout << "Setting Properties: " << *prop << endl;
    if( m_prop ) delete m_prop;
    m_prop = prop;
}
*/

IndividualProperties * IndividualShell::getProperties() {
    return m_prop;
}

void IndividualShell::addOffspring() {
    m_prop->m_offspring++;
}

unsigned int IndividualShell::getOffspringCount() const {
    return m_prop->m_offspring;
}

unsigned int IndividualShell::getEnvironmentLociCount() const {
    return m_environment->getLociCount();
}

unsigned int IndividualShell::getVariantCount() const {
    return m_prop->m_genos->size();
}

allele_t IndividualShell::alleleAt( unsigned int var_idx, ploidy_t copy ) const {
    if( var_idx < m_prop->m_genos->size() )
        return m_prop->m_genos->at( var_idx )[ copy ];
    return (allele_t)ANCESTRAL_ALLELE;
}

void IndividualShell::print( ostream & out ) const {
    out << m_name;

    if( m_prop ) {
        out << ", " << *m_prop;
    }
    out << "\n";
}
