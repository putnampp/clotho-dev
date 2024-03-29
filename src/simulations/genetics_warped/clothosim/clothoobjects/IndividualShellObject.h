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

#ifndef INDIVIDUALSHELLOBJECT_H_
#define INDIVIDUALSHELLOBJECT_H_

#include "warped.h"
#include "SimulationObject.h"
#include "SerializedInstance.h"

#include "common.h"
#include <vector>

#include "events/ClothoEvent.h"
#include "../ClothoObject.h"
#include "IntVTime.h"
#include "IndividualObjectState.h"
#include "IndividualProperties.h"
#include "reproduction.h"

class Environment2;

using std::vector;

class IndividualShell : public ClothoObject {
public:
    IndividualShell( Environment2 * env );
    IndividualShell( const ObjectID & env, reproduction * repro);

    virtual ~IndividualShell();

    void initialize();
    void finalize();

    void executeProcess();

    State * allocateState();
    const string & getName() const;

    void print( ostream & out ) const;

    template < class EVT >
    void handleEvent( const EVT * e ) {}

    void setEnvironment( Environment2 * env );
    IndividualProperties * getProperties();

    bool    isAlive() const;
    sex_t   getSex() const;
    IntVTime * getBirthTime() const;

    Environment2 * getEnvironment() const;

    void addOffspring();
    unsigned int getOffspringCount() const;

    allele_t alleleAt( unsigned int var_idx, ploidy_t strand ) const;
    unsigned int getVariantCount() const;

    unsigned int getEnvironmentLociCount() const;

private:
    string  m_name;
    Environment2 *  m_environment;
    IndividualProperties * m_prop;
};

#endif  // INDIVIDUALSHELLOBJECT_H_
