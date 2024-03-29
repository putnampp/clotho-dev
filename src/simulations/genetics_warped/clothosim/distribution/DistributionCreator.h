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

#ifndef DISTRIBUTIONCREATOR_H_
#define DISTRIBUTIONCREATOR_H_

#include "common.h"
#include "DistributionManager.h"

template < class OBJ, class PARAMS >
class DistributionCreator : public dist_creator< PARAMS > {
public:
    DistributionCreator( const char * name ) : m_name(name) {
        DistributionManager< PARAMS >::getInstance()->registerDistribution( this );
    }

    const string & getName() const {
        return m_name;
    }

    shared_ptr< iDistribution > createDistribution( const PARAMS & n ) {
        shared_ptr< iDistribution > dist;

        return dist;
    }

    virtual ~DistributionCreator() {}
private:
    const string m_name;
};

#define DECLARE_REGISTERED_DISTRIBUTION( name, params ) \
    extern DistributionCreator< name, params > dc_##name;

#define DEFINE_REGISTERED_DISTRIBUTION( name, params ) \
    DistributionCreator< name, params> dc_##name( #name );

#endif  // DISTRIBUTIONCREATOR_H_
