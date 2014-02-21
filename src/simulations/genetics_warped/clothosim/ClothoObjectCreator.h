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

#ifndef CLOTHOOBJECTCREATOR_H_
#define CLOTHOOBJECTCREATOR_H_

#include "common.h"
#include "ClothoObject.h"
#include "ClothoObjectManager.h"

template < class OBJ, class PARAMS >
class ClothoObjectCreator : public SimObjectCreator< PARAMS > {
public:
    ClothoObjectCreator( const char * name ) : m_name(name) {
        ClothoObjectManager< PARAMS >::getInstance()->registerObject( this );
    }

    const string & name() {
        return m_name;
    }

    SimulationObject * createObject() {
        return new OBJ();
    }

    void createObjectFrom( const PARAMS & n, shared_ptr< vector< SimulationObject * > > objs ) {
        SimulationObject * so = new OBJ( );
        objs->push_back( so );
    }
    virtual ~ClothoObjectCreator() {}
private:
    const string m_name;
};

#define DECLARE_CLOTHO_OBJECT( name )                            \
    class name : public ClothoObject

#define DECLARE_REGISTERED_CLOTHO_OBJECT( name, params )                 \
    extern ClothoObjectCreator< name, params > objc_##name;

#define DEFINE_REGISTERED_CLOTHO_OBJECT( name, params )                \
    ClothoObjectCreator< name, params > objc_##name( #name );

#endif  // CLOTHOOBJECTCREATOR_H_
