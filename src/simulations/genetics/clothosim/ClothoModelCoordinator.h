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

#ifndef CLOTHOMODELCOORDINATOR_H_
#define CLOTHOMODELCOORDINATOR_H_

#include "common.h"
#include "ClothoModel.h"
#include "ClothoObject.h"

#include <map>

using std::multimap;

class ClothoModelCoordinator : public ClothoObject {
public:
    typedef multimap< string, shared_ptr< ClothoModel > > Models;
    static shared_ptr< ClothoModelCoordinator > getInstance();

    void initialize();
    void finalize();

    const string & getName() const;

    void executeProcess();
    State * allocateState();

    void print( ostream & ) const;

//    void registerModel( shared_ptr< ClothoModel > cm );
    void addEventHandler( const string & name, shared_ptr< ClothoModel > cm );
    void handleEvent( const Event * evt ) const;

    void routeEvent( const Event * evt ) const;

    virtual ~ClothoModelCoordinator();
protected:
    ClothoModelCoordinator();

    string  m_name;
    Models  m_models;
};

#endif  // CLOTHOMODELCOORDINATOR_H_
