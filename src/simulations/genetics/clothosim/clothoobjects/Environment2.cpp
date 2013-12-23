/*******************************************************************************
 * Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    vector of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this vector of conditions and the following disclaimer in the documentation
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

#include "IntVTime.h"

#include "events/ShellBirthEvent.h"
#include "events/ShellDeathEvent.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <utility>

using std::swap;
using std::cout;
using std::endl;

const string POOL_SIZE_K = "pool_size";

Environment2::Environment2( const char * n, int max_size, const string & log ) :
    m_genetic_map( new GeneticMap() ),
    m_name( n ),
    m_max_pool_size( max_size ), 
    m_pool_size(0), 
    m_logdir(log) {
}

Environment2::~Environment2() {
    /*
     * SequentialSimulationManager does not perform cleanup of 
     * SimulationObjects.
     * Task is assumed to be taken care of by mechanism which
     * created the object. In this case YamlConfig object.
        for( vector< IndividualShell * >::iterator it = m_females.begin(); it != m_females.end(); ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }
        for( vector< IndividualShell * >::iterator it = m_males.begin(); it != m_males.end(); ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }
        for( vector< IndividualShell * >::iterator it = m_unk.begin(); it != m_unk.end(); ) {
            IndividualShell * shell = (*it++);
            delete shell;
        }

        while(! m_individual_pool.empty()) {
            IndividualShell * shell = m_individual_pool.front();
            m_individual_pool.pop();
            delete shell;
        }
    */

    m_females.clear();
    m_males.clear();
    m_unk.clear();

    while( !m_individual_pool.empty() ) { m_individual_pool.pop(); }

    if( m_logger.is_open() ) m_logger.close();
}

void Environment2::initialize() {
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

        //delete evt;
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
    if( !s ) return;

    if( m_max_pool_size > -1 ) {
        // bounded pool
        if( m_pool_size >= m_max_pool_size ) return;
    }
    ++m_pool_size;

    switch( s->getSex() ) {
    case FEMALE:
        m_mapped_females.insert( make_pair( s, m_females.size()) );
        m_females.push_back(s);
        break;
    case MALE:
        m_mapped_males.insert( make_pair(s, m_males.size() ) );
        m_males.push_back( s );
        break;
    case UNK_SEX:
        m_mapped_unk.insert( make_pair(s, m_unk.size()) );
        m_unk.push_back( s );
        break;
    default:
        m_individual_pool.push( s );
        return;
    }

    Event * evt = new ShellBirthEvent( getSimulationTime(), getSimulationTime(), s, this );
    this->receiveEvent( evt );
}

void Environment2::removeIndividual( IndividualShell * s ) {
    if( !s ) return;

//    vector< IndividualShell * > * l =  &m_unk;
//    MappedIndividuals * m = &m_mapped_unk;

    switch( s->getSex() ) {
    case FEMALE: {
//        l = &m_females;
//        m = &m_mapped_females;
        MappedIndividuals::iterator i = m_mapped_females.find( s ), b = m_mapped_females.find( m_females.back() );

        swap( m_females[ i->second ], m_females.back() );
        b->second = i->second;

        m_mapped_females.erase( i );
        m_females.pop_back();
    }
        break;
    case MALE: {
//        l = &m_males;
//        m = &m_mapped_males;
        MappedIndividuals::iterator i = m_mapped_males.find( s ), b = m_mapped_males.find( m_males.back() );

        swap( m_males[ i->second ], m_males.back() );
        b->second = i->second;

        m_mapped_males.erase( i );
        m_males.pop_back();
    }
        break;
    case UNK_SEX: {
//        l = &m_unk;
        MappedIndividuals::iterator i = m_mapped_unk.find( s ), b = m_mapped_unk.find( m_unk.back() );

        swap( m_unk[ i->second ], m_unk.back() );
        b->second = i->second;

        m_mapped_unk.erase( i );
        m_unk.pop_back();
    }
        break;
    default:
        cout << "ERRRORORORROR\n";
        return;
    }
/*
    bool bFound = false;
    while( it != it_end ) {
        if( *it == s ) {
            bFound = true;
            swap( *it, l->back() );

            l->pop_back();
            break;             
        }
        it++;
    }
    if(! bFound ) {
        cout << "Individual not found" << endl;
    }
*/
/*
    MappedIndividuals::iterator i = m->find( s ), b = m->find( l->back() );

    swap( (*l)[ i->second ], l->back() );
    b->second = i->second;

    m->erase( i );
    l->pop_back();
*/

    m_individual_pool.push( s );

    const VTime & t = getSimulationTime();

    Event * e = new ShellDeathEvent( t, t, s, this );
    this->receiveEvent(e);
}

IndividualShell * Environment2::nextAvailableIndividual() {
    IndividualShell * t = NULL;

    if( !m_individual_pool.empty() ) {
        // there is at least one available Individual so use it
        t = m_individual_pool.front();
        m_individual_pool.pop();
        --m_pool_size;
/*    } else if( m_max_pool_size == -1 || m_pool_size < m_max_pool_size) {
        // there are no available Individuals but there is no limit
        // on pool size;
        // therefore create a new one
        cout << "Generating a new individual" << endl;
        t = new IndividualShell( this, new IndividualProperties() );*/
    } // else individual pool has been maxed out; consider terminating

    return t;
}

bool Environment2::hasAvailableIndividuals() const {
    return !m_individual_pool.empty();
}

int Environment2::getMaleCount() const {
    return m_males.size();
}

int Environment2::getFemaleCount() const {
    return m_females.size();
}

size_t Environment2::getLociCount() const {
    return m_genetic_map->getLociCount();
}

shared_ptr< GeneticMap > Environment2::getGeneticMap() {
    return m_genetic_map;
}

IndividualShell * Environment2::getMaleAt( unsigned int idx ) const {
    if( idx < m_males.size() ) {
        return m_males[idx];
    }
    return NULL;
}

IndividualShell * Environment2::getFemaleAt( unsigned int idx ) const {
    if( idx < m_females.size() ) {
        return m_females[idx];
    }
    return NULL;
}

void Environment2::print( ostream & out ) const {
    out << m_name << "\n";
    out << m_individual_pool.size() << " pooled individuals\n";
    out << m_females.size() << " females\n";
    for( vector< IndividualShell * >::const_iterator it = m_females.begin(); it != m_females.end(); it++ ) {
        (*it)->print( out );
    }

    out << m_males.size() << " males\n";
    for( vector< IndividualShell * >::const_iterator it = m_males.begin(); it != m_males.end(); it++ ) {
        (*it)->print( out );
    }

    out << m_unk.size() << " unknown\n";
    for( vector< IndividualShell * >::const_iterator it = m_unk.begin(); it != m_unk.end(); it++ ) {
        (*it)->print( out );
    }
}
