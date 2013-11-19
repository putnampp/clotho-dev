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

#ifndef ENVIRONMENT2OBJECT_H_
#define ENVIRONMENT2OBJECT_H_

#include "warped.h"
#include "common_types.h"

#include <fstream>

#include "IndividualShellObject.h"
#include "GeneticMap.h"

#include <vector>
#include <list>
#include <queue>
#include <unordered_map>

using std::unordered_map;
using std::queue;
using std::ofstream;

class Environment2 : public ClothoObject {
public:
    Environment2( const char * n, int max_size, const string & l);

    virtual ~Environment2();

    void initialize();
    void finalize();

    void executeProcess();

    State * allocateState();
    const string & getName() const;

    void addIndividual( IndividualShell * s );
    void removeIndividual( IndividualShell * s );
    IndividualShell * nextAvailableIndividual();

    template < class EVT >
    void handleEvent( const EVT * e ) {}

    int getMaleCount() const;
    int getFemaleCount() const;

    size_t getLociCount() const;

    shared_ptr< GeneticMap > getGeneticMap();

    IndividualShell * getMaleAt( unsigned int idx ) const;
    IndividualShell * getFemaleAt( unsigned int idx ) const;

    void print( ostream & out ) const;
protected:
    vector< IndividualShell * > m_females, m_males, m_unk;
    queue< IndividualShell * > m_individual_pool;
    shared_ptr< GeneticMap >    m_genetic_map;

private:
    string m_name;
    int m_max_pool_size;
    int m_pool_size;
    string m_logdir;

    ofstream m_logger;
};

#endif  // ENVIRONMENT2OBJECT_H_
