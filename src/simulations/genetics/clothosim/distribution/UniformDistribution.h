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

#ifndef UNIFORMDISTRIBUTION_H_
#define UNIFORMDISTRIBUTION_H_

#include "Distribution.h"
#include <queue>

using std::queue;

class UniformDistribution : public iDistribution {
public:
    UniformDistribution( );

    void setRandomNumberGenerator( gsl_rng * rng );
    inline double nextVariate( ) {
        if( !m_rng ) return -1.0;
        return gsl_rng_uniform_pos( m_rng );
//        return (*m_variate)();
    }

    inline unsigned long int nextVariate( unsigned long int max ) {
        if( !m_rng ) return 0;
        return gsl_rng_get( m_rng ) % max;
//        return (*m_variate2)(max);
    }

    bool nextBoolean();

    virtual ~UniformDistribution();
protected:

    typedef double (UniformDistribution::*variate_ptr)();
    typedef unsigned long int (UniformDistribution::*variate2_ptr)( unsigned long int );

    double variateNoRNG() {
        return -1.0;
    }

    double variateRNG() {
        return gsl_rng_uniform_pos( m_rng );
    }

    unsigned long int variate2NoRNG( unsigned long int max ) {
        return 0;
    }

    unsigned long int variate2RNG( unsigned long int max ) {
        return gsl_rng_get( m_rng ) % max;
    }

    void updateBooleanPool();
    void updateBooleanPool2();

    gsl_rng * m_rng;
    queue< bool >   m_bools;

    variate_ptr m_variate;
    variate2_ptr m_variate2;
};

#endif  // UNIFORMDISTRIBUTION_H_
