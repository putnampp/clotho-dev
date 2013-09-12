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

#include "Chromosome.h"

Chromosome::Chromosome( const String & n, size_t len ) :
        m_name(n), 
        m_id( nextID() ),
        m_size(len) { }

/**
 *
 * Change the chromosome length. New size cannot be smaller
 * than the largest site. In the event that size is less
 * than the largest site, then length will not be modified
 */
void Chromosome::setLength( size_t size ) {
    // chromosome size must be greater than the largest site
    // site set is ordered, just need to compare size to 
    // last element
    if( m_sites.empty() || size >= (*m_sites.rbegin()))
        m_size = size;
}

/**
 *
 * Add positions of interest (sites).
 *
 * The length of the chromosome will be extended to 
 * include this site should it not be long enough
 *
 */
void Chromosome::add_site( size_t pos ) {
    m_sites.insert( pos );

    if( (*m_sites.rbegin()) >= m_size )
        m_size = (*m_sites.rbegin()) + 1;
}

chromid_t Chromosome::id() const {
    return m_id;
}

String Chromosome::name() const {
    return m_name;
}

size_t Chromosome::length() const {
    return m_size;
}

size_t Chromosome::loci() const {
    return m_sites.size();
}

bool    Chromosome::is_locus( size_t pos ) const {
    set< size_t >::iterator it = m_sites.find( pos );
    return it == m_sites.end();
}

chromid_t Chromosome::nextID() {
    static chromid_t next_id = 0;
    assert( next_id != UNKNOWN_CHROM );
    return next_id++;
}

Chromosome::~Chromosome() {
    m_sites.clear();
}
