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

#ifndef INDIVIDUALPROPERTIES_H_
#define INDIVIDUALPROPERTIES_H_

#include "common_types.h"
#include "IntVTime.h"

#include <ostream>
#include <vector>

using std::ostream;
using std::vector;

class IndividualProperties {
public:
    long    m_id;
    long    m_parent0, m_parent1;
    IntVTime * m_dob, * m_eol;
    bool m_isMature;
    unsigned int m_offspring;
    sex_t   m_sex;
    AlleleGroupPtr m_genos;

    IndividualProperties( AlleleGroupPtr g) : 
        m_id(-1),
        m_parent0(-1),
        m_parent1(-1),
        m_dob(NULL), 
        m_eol( NULL ), 
        m_isMature(false), 
        m_offspring(0), 
        m_sex( UNASSIGNED ),
        m_genos(g) {}

    IndividualProperties( sex_t s, AlleleGroupPtr genos ) :
        m_id( next_id++),
        m_parent0(-1),
        m_parent1(-1),
        m_dob(NULL), 
        m_eol(NULL), 
        m_isMature(false), 
        m_offspring(0), 
        m_sex(s), 
        m_genos(genos) {}

    IndividualProperties( IndividualProperties * p0, IndividualProperties * p1,
                         sex_t s, AlleleGroupPtr genos ) :
        m_id( next_id++),
        m_parent0( p0->m_id ),
        m_parent1( p1->m_id ),
        m_dob(NULL), 
        m_eol(NULL), 
        m_isMature(false), 
        m_offspring(0), 
        m_sex(s), 
        m_genos(genos) {}

    virtual ~IndividualProperties()  {
        if( m_dob ) delete m_dob;
        if( m_eol ) delete m_eol;

        m_genos.reset();
    }
private:
    static long next_id;
};

ostream & operator<<( ostream &, const IndividualProperties & ip );

#endif  // INDIVIDUALPROPERTIES_H_