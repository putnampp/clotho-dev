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

#ifndef MATINGMODELCREATOR_H_
#define MATINGMODELCREATOR_H_

#include "common.h"
#include "iMatingModelCreator.h"
#include "MatingModelFactory.h"

template < class MODEL >
class MatingModelCreator : public iMatingModelCreator {
public:
    MatingModelCreator( const char * name, const char * desc ) :
        m_name(name), m_desc( desc ) {
        MatingModelFactory::getInstance()->add( this );
    }

    virtual String & name() const {
        return m_name;
    }

    virtual String & description() const {
        return m_desc;
    }

    virtual void    print( std::ostream & out ) const;

    virtual boost::shared_ptr< MatingModel > create() {
        return boost::shared_ptr< MatingModel >( new MODEL() );
    }

    virtual ~MatingModelCreator() {
        MatingModelFactory::getInstance()->remove( this );
    }
private:
    String  m_name;
    String  m_desc;
};

#define REGISTERED_MATINGMODEL( name, desc )                \
    class name;                                             \
    MatingModelCreator< name >  mm_##name( #name, #desc );  \
    class name : public MatingModel

#endif  // MATINGMODELCREATOR_H_
