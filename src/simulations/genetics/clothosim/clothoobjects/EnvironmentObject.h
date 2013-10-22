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

#ifndef ENVIRONMENTOBJECT_H_
#define ENVIRONMENTOBJECT_H_

#include "warped.h"
#include "common_types.h"
#include "../ClothoObjectCreator.h"

#include <vector>

DECLARE_CLOTHO_OBJECT( Environment ) {
public:
    Environment();
    Environment( const YAML::Node & n);

    virtual ~Environment();

    void initialize();
    void finalize();

    void executeProcess();
    
    State * allocateState();
    const string & getName() const;

    template < class SEL_MODEL >
    OBJECT_ID select_individual( OBJECT_ID & ) const;

    void print( ostream & out ) const;
protected:
    template< class EVT >
    void handleEvent( const EVT * evt );

    vector< OBJECT_ID > m_females, m_males;

private:
    string m_name;
};

template < class SEL_MODEL >
OBJECT_ID Environment::select_individual( OBJECT_ID &  id) const {
    // alone for another night ...
    return id;
}

DECLARE_REGISTERED_CLOTHO_OBJECT( Environment )

#endif  // ENVIRONMENTOBJECT_H_