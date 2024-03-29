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

#ifndef CLOTHOAPPLICATION_H_
#define CLOTHOAPPLICATION_H_

#include "common.h"
#include "Application.h"
#include "SimulationConfiguration.h"
#include "IntVTime.h"
#include "ClothoPartitioner.h"
#include "YamlConfig.h"

#include "GeneticMap.h"

class ClothoApplication : public Application {
public:
    ClothoApplication( const string & config );
    virtual int finalize( );

    virtual const   PartitionInfo * getPartitionInfo( unsigned int nPE );
    virtual int     getNumberOfSimulationObjects( int mgrID ) const;

    virtual string getCommandLineParameters() const;

    virtual void registerDeserializers();

    virtual const VTime & getPositiveInfinity();
    virtual const VTime & getZero();
    virtual const VTime & getTime( string & );

    virtual ~ClothoApplication();
protected:
    string  m_config;

    IntVTime * m_time;

    ClothoPartitioner * m_part;
    YamlConfig        * m_yaml;

    GeneticMap::Ptr     m_genetic_map;
};

#endif  // CLOTHOAPPLICATION_H_
