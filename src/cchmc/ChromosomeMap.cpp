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

#include "ChromosomeMap.h"

void ChromosomeMap::configure( std::istream & config ) { }

size_t ChromosomeMap::size() const {
    return m_chroms.size();
}

chromid_t   ChromosomeMap::find( const string & name, bool bCreateMissing ) {
    chromid_t id = UNKNOWN_CHROM;
    
    Chromosomes::iterator it = m_chroms.find(name);

    if( it != m_chroms.end() ) {
        id = it->second->id();
    } else if( bCreateMissing ) {
        ChromosomePtr c( new Chromosome( name, 0) );

        m_chroms.insert( std::make_pair(name, c) );

        id = c->id();
    }

    return id;
}

/**
 *  Linearly search for chromosome object by id
 *
 *  TODO: would be more efficient to maintain a map
 *  of chromosomes by ID.  Is it worth maintaining multiple
 *  maps for sorted access? Cost/benefit of find method usage?
 */
ChromosomePtr ChromosomeMap::find( chromid_t id ) {
    assert( 0 <= id && id < (chromid_t) m_chroms.size() );

    ChromosomesIter it = m_chroms.begin();

    ChromosomePtr p;

    while( it != m_chroms.end() ) {
        if( it->second->id() == id ) {
            p = it->second;
            break;
        }
        it++;
    }
    return p;
}

ChromosomeMap::ChromosomesIter ChromosomeMap::begin() const { return m_chroms.begin(); }
ChromosomeMap::ChromosomesIter ChromosomeMap::end() const { return m_chroms.end(); }

ChromosomeMap::~ChromosomeMap() {
    m_chroms.clear();
}
