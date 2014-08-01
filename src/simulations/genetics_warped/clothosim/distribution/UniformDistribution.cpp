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

#include "UniformDistribution.h"

#include <cmath>

UniformDistribution::UniformDistribution( ) :
    m_rng( NULL ),
    m_bools() {
    setRandomNumberGenerator( NULL );
}

UniformDistribution::~UniformDistribution() {
}

void UniformDistribution::setRandomNumberGenerator( gsl_rng * rng ) {
    m_rng = rng;
    /*
        if( m_rng ) {
           m_variate = &UniformDistribution::variateRNG;
           m_variate2 = &UniformDistribution::variate2RNG;
        } else {
            m_variate = &UniformDistribution::variateNoRNG;
            m_variate2 = &UniformDistribution::variate2NoRNG;
        }
    */
}

bool UniformDistribution::nextBoolean() {
    if( !m_rng ) return false;

    if( m_bools.empty() )
        updateBooleanPool2();

    bool ret = m_bools.front();
    m_bools.pop();

    return ret;
}

/*
 *
 * Iterated Von Neumann extractor for uniform
 * boolean values from random bit stream
 *
 */
void UniformDistribution::updateBooleanPool() {
    while( m_bools.empty() ) {
        unsigned long rnd = gsl_rng_get(m_rng);
        unsigned int offset = (sizeof(unsigned long) * 8);
        while( offset >= 2 ) {
            unsigned long input = rnd & 3;
            rnd /= 4;
            offset -= 2;
            switch( input ) {
            case 1:
                m_bools.push( false );
                // append 1 to bools
                ++offset;
                rnd |= (1 << offset);
                break;
            case 2:
                m_bools.push( true );
                ++offset;
                break;
            default:
                break;
            }
        }
    }

}

/*
 *
 * Von Neumann extractor for uniform
 * boolean values from random bit stream
 *
 */
void UniformDistribution::updateBooleanPool2() {
    while( m_bools.empty() ) {
        unsigned long rnd = gsl_rng_get(m_rng);
        unsigned long rnd2 = gsl_rng_get(m_rng);
        unsigned int offset = (sizeof(unsigned long) * 8);
        rnd ^= rnd2;

        do {
            if( rnd & (1 << offset) ) {
                m_bools.push( rnd2 & (1 << offset) );
            }
        } while( --offset );
    }
}
