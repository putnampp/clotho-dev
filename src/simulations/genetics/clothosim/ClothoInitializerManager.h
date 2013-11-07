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

#ifndef CLOTHOINITIALIZERMANAGER_H_
#define CLOTHOINITIALIZERMANAGER_H_

#include "common.h"

#include "SimulationObject.h"

#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

static const string INITIALIZER_K = "initializer";

struct initializer_creator {
    virtual const string & name() = 0;
};

template < class P >
struct InitializerCreator : virtual public initializer_creator {
    virtual void initialize( const P &, shared_ptr< vector< SimulationObject * > > ) = 0;
};

template < class PARAMS >
class ClothoInitializerManager {
public:
    typedef InitializerCreator< PARAMS > object_creator_t;
    typedef std::unordered_map< string, object_creator_t * > ManagedInitializers;
    typedef typename ManagedInitializers::iterator  miterator;

    static shared_ptr< ClothoInitializerManager< PARAMS > > getInstance() {
        static shared_ptr< ClothoInitializerManager< PARAMS > > inst( new ClothoInitializerManager<PARAMS>() );
        return inst;
    }

    void registerInitializer( object_creator_t * soc ) {
        m_creators[ soc->name() ] = soc;
    }

    void initialize( const string & name, const PARAMS & p, shared_ptr< vector< SimulationObject * > > objs ) {
        miterator it = m_creators.find( name );
        if( it == m_creators.end() )
            return;

        it->second->initialize( p, objs );
    }

    virtual ~ClothoInitializerManager() {
        m_creators.clear();
    }
protected:
    ClothoInitializerManager() {}

    ManagedInitializers m_creators;
};

#endif  // CLOTHOINITIALIZERMANAGER_H_
