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

#include "IndividualProperties.h"

long IndividualProperties::next_id = 1;

void IndividualProperties::updateGenotypes( AlleleGroupPtr agp ) {
    size_t idx = 0;
    for( AlleleGroup::iterator it = agp->begin(); it != agp->end(); it++, ++idx ) {
        if( idx < m_genos->size() ) {
            (*m_genos)[idx] = *it;
        } else {
            m_genos->push_back( *it );
        }
    }
}

void IndividualProperties::setMother( const IndividualProperties * mother ) {
    m_mother = mother->m_id;
}

void IndividualProperties::setFather( const IndividualProperties * father ) {
    m_father = father->m_id;
}

void IndividualProperties::reset() {
    if( m_dob ) {
        delete m_dob;
        m_dob = NULL;
    }

    if( m_eol ) {
        delete m_eol;
        m_eol = NULL;
    }

    m_mother = 0;
    m_father = 0;
    m_isMature = false;
    m_offspring = 0;
}

ostream & operator<<( ostream & out, const IndividualProperties & ip ) {
    out << ip.m_id 
        << ", " << ip.m_sex
        << ", " << ip.m_mother
        << ", " << ip.m_father;
    if( ip.m_dob ) {
        out << ", " << *ip.m_dob;
    } else {
        out << ", -1";
    }

    if( ip.m_eol ) {
        out << ", " << (*ip.m_eol - *ip.m_dob);
    } else {
        out << ", ?";
    }

    out << ", " << ip.m_offspring;

    if( !ip.m_genos->empty() ) {
        out << ", [";

        AlleleGroup::iterator it = ip.m_genos->begin();
        out << "{" << (*it++) << "}";
        while( it != ip.m_genos->end() ) {
            out << ",{" << (*it++) << "}";
        }
        out << "]";
    }
    return out;
}
