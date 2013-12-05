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

#ifndef DISTRIBUTIONMANAGER_H_
#define DISTRIBTUIONMANAGER_H_

#include "common.h"
#include <unordered_map>
#include <time.h>

#include "Distribution.h"

#include "gsl/gsl_rng.h"

template < class PARAMS >
struct dist_creator {
    virtual const string & getName() const = 0;
    virtual shared_ptr< iDistribution > createDistribution( const PARAMS & n ) = 0;
};

template < class PARAMS >
class DistributionManager {
public:
    typedef dist_creator< PARAMS > dist_creator_t;
    typedef dist_creator_t * dist_creator_ptr;
    typedef std::unordered_map< string, dist_creator_ptr > Distributions;
    typedef typename Distributions::iterator iterator;

    static shared_ptr< DistributionManager< PARAMS > > getInstance() {
        static shared_ptr< DistributionManager< PARAMS > > inst( new DistributionManager< PARAMS >() );
        return inst;
    }

    void registerDistribution( dist_creator_ptr dist ) {
        m_distributions[ dist->getName() ] = dist;
    }

    shared_ptr< iDistribution > createDistribution( const string & name, const PARAMS & p ) {
        shared_ptr< iDistribution > dist;
        iterator it = m_distributions.find( name );

        if( it != m_distributions.end() ) {
            dist = it->second->createDistribution( p );
        }

        if( dist ) {
            dist->setRandomNumberGenerator( m_rng );
        }

        return dist;
    }

    virtual ~DistributionManager() {
        m_distributions.clear();

        gsl_rng_free( m_rng );
    }
protected:
    DistributionManager() :
        m_rng( gsl_rng_alloc( gsl_rng_mt19937 ) ) {
        long seed = time(NULL);
        gsl_rng_set( m_rng, seed );
    }

    Distributions   m_distributions;
    gsl_rng * m_rng;
};

#endif  // DISTRIBUTIONMANAGER_H_
