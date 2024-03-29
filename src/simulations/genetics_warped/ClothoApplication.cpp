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

#include "ClothoApplication.h"
#include "ClothoPartitioner.h"
#include "YamlConfig.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <boost/lexical_cast.hpp>

ClothoApplication::ClothoApplication( const string & config ) : m_config( config ),
    m_time(NULL),
    m_part( new ClothoPartitioner() ),
    m_yaml( new YamlConfig( m_config ) ) {}

int ClothoApplication::finalize( ) {
    return 0;
}

const PartitionInfo * ClothoApplication::getPartitionInfo( unsigned int nPE ) {
    if( m_config.empty() ) {
        cerr << "Unspecified configuration file " << endl;
        abort();
    }

    shared_ptr< vector< SimulationObject * > > objs = m_yaml->getSimulationObjects();

    const PartitionInfo * ret = m_part->partition( &*objs, nPE );

    return ret;
}

int     ClothoApplication::getNumberOfSimulationObjects( int mgrID ) const {
    return 0;
}

string ClothoApplication::getCommandLineParameters() const {
    return string("");
}

void ClothoApplication::registerDeserializers() { }

const VTime & ClothoApplication::getPositiveInfinity() {
    return IntVTime::getIntVTimePositiveInfinity();
}

const VTime & ClothoApplication::getZero() {
    return IntVTime::getIntVTimeZero();
}

const VTime & ClothoApplication::getTime( string & t) {
    if( !m_time ) {
        int time = boost::lexical_cast<int>( t );
        m_time = new IntVTime( time );
    }
    return *m_time;
}

ClothoApplication::~ClothoApplication() {
    if( m_time ) delete m_time;

    delete m_part;
    delete m_yaml;
}
