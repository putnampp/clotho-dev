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

#include "Environment2.h"
//#include "Environment2ObjectState.h"

#include "IntVTime.h"

#include "events/ClothoEvent.h"
#include "events/LogEvent.h"

#include <boost/lexical_cast.hpp>

#include <iostream>

using std::cout;
using std::endl;

const string POOL_SIZE_K = "pool_size";

Environment2::Environment2( const char * n, int max_size, const string & log ) :
    m_name( n ), m_max_pool_size( max_size ), m_pool_size(0), m_logdir(log) {
}
/*
Environment2::Environment2( const YAML::Node & n) : m_name( "ENV" ), m_max_pool_size( -1 ), m_pool_size( 0 ) {
    if( n[ POOL_SIZE_K ] ) {
        m_max_pool_size = n[ POOL_SIZE_K ].as< int >();
    }

    // pre-populate the pool if necessary
    if( m_max_pool_size <= -1 ) assert(false);  // Warped does not support dynamic SimulationObject allocation
}
*/

Environment2::~Environment2() {
    /*
     * Because IndividualShells are SimulationObjects
     * their clean up may be handled by the simulation manager
     * may be sufficient to just clear the vectors and queue
        for( vector< IndividualShell * >::iterator it = m_females.begin(); it != m_females.end(); ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }
        for( vector< IndividualShell * >::iterator it = m_males.begin(); it != m_males.end(); ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }
        for( vector< IndividualShell * >::iterator it = m_unk.begin(); it != m_unk.end(); it ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }

        while(! m_individual_pool.empty()) {
            IndividualShell * shell = m_individual_pool.front();
            m_individual_pool.pop();
            delete shell;
        }
    */

    if( m_logger.is_open() ) m_logger.close();
}

void Environment2::initialize() {
    // schedule first logging event?
    //
//    const IntVTime tmp = static_cast< const IntVTime & >( getSimulationTime() ) + 2;
//    Event * e = new LogEvent(getSimulationTime(), tmp, this, this );
//    this->receiveEvent( e );
//
    if( !m_logdir.empty() ) {
        string log_file = m_logdir + "something.log";
        m_logger.open( log_file );
        if( !m_logger.is_open() )
            abort();

        m_logger << "Environment initialized at " << getSimulationTime() << "\n";
    }
}

void Environment2::finalize() {
    if( m_logger.is_open() ) {
        m_logger << "Environment finalized at " << getSimulationTime() << "\n";

        m_logger.flush();
        m_logger.close();
    }

    if( !m_logdir.empty() ) {
        string path = m_logdir + "final_environment.log";
        m_logger.open( path );

        if( m_logger.is_open() ) {
            print( m_logger );
            m_logger.flush();
            m_logger.close();
        }
    }
}

void Environment2::executeProcess() {
    while( haveMoreEvents() ) {
        const Event * evt = getEvent();

        const ModelHandler< Environment2 > * e = dynamic_cast< const ModelHandler< Environment2 > * >( evt );
        if( e ) {
            e->updateModels( this );
        }
        const ModelHandler< ClothoObject > * e2 = dynamic_cast< const ModelHandler< ClothoObject > * > (evt );
        if( e2 ) {
            e2->updateModels( this );
        }
    }
}

State * Environment2::allocateState() {
    //return new Environment2ObjectState();
    return NULL;
}

const string & Environment2::getName() const {
    return m_name;
}

void Environment2::addIndividual( IndividualShell * s ) {
    if( m_max_pool_size > -1 ) {
        // bounded pool
        if( m_pool_size >= m_max_pool_size ) return;
    }

    switch( s->getSex() ) {
    case FEMALE:
        m_females.push_back(s);
        break;
    case MALE:
        m_males.push_back( s );
        break;
    case UNK_SEX:
        m_unk.push_back( s );
        break;
    default:
        m_individual_pool.push( s );
        break;
    }
    ++m_pool_size;
}

void Environment2::removeIndividual( IndividualShell * s ) {
    list< IndividualShell * > & l = m_unk;

    switch( s->getSex() ) {
    case FEMALE:
        l = m_females;
        break;
    case MALE:
        l = m_males;
        break;
    case UNK_SEX:
        l = m_unk;
        break;
    default:
        return;
    }

    list< IndividualShell * >::iterator to_remove = l.begin();
    while( to_remove != l.end() ) {
        if( (*to_remove) == s ) {
            break;
        }
        to_remove++;
    }
    if( to_remove != l.end() ) {
        //cout << "Removing individual" << endl;
        l.erase( to_remove );
    } else {
        cout << "Individual not found " << endl;
    }

    m_individual_pool.push( s );
}

IndividualShell * Environment2::nextAvailableIndividual() {
    IndividualShell * t = NULL;

    if( !m_individual_pool.empty() ) {
        // there is at least one available Individual so use it
        t = m_individual_pool.front();
        m_individual_pool.pop();
        --m_pool_size;
    } else if( m_max_pool_size == -1 || m_pool_size < m_max_pool_size) {
        // there are no available Individuals but there is no limit
        // on pool size;
        // therefore create a new one
        t = new IndividualShell( this, new IndividualProperties() );
    } // else individual pool has been maxed out; consider terminating

    return t;
}

int Environment2::getMaleCount() const {
    return m_males.size();
}

int Environment2::getFemaleCount() const {
    return m_females.size();
}

IndividualShell * Environment2::getMaleAt( unsigned int idx ) const {
    if( idx < m_males.size() ) {
        list< IndividualShell * >::const_iterator it = m_males.begin();
        unsigned int i = 0;
        while( i++ < idx ) { it++; }

        return *it;
    }
    return NULL;
}

IndividualShell * Environment2::getFemaleAt( unsigned int idx ) const {
    if( idx < m_females.size() ) {
        list< IndividualShell * >::const_iterator it = m_females.begin();
        unsigned int i = 0;
        while( i++ < idx ) { it++; }

        return *it;
    }
    return NULL;
}

void Environment2::print( ostream & out ) const {
    out << m_name << "\n";
    out << m_individual_pool.size() << " pooled individuals\n";
    out << m_females.size() << " females\n";
    for( list< IndividualShell * >::const_iterator it = m_females.begin(); it != m_females.end(); it++ ) {
        (*it)->print( out );
    }

    out << m_males.size() << " males\n";
    for( list< IndividualShell * >::const_iterator it = m_males.begin(); it != m_males.end(); it++ ) {
        (*it)->print( out );
    }

    out << m_unk.size() << " unknown\n";
    for( list< IndividualShell * >::const_iterator it = m_unk.begin(); it != m_unk.end(); it++ ) {
        (*it)->print( out );
    }
}
