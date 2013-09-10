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

#ifndef CHROMOSOMEMAP_H_
#define CHROMOSOMEMAP_H_

#include "common.h"
#include "Chromosome.h"
#include "Configurable.h"
#include "Sequence.h"

#include <map>

class ChromosomeMap : public Configurable {
public:
    typedef std::map< const String, ChromosomePtr > Chromosomes;
    typedef Chromosomes::const_iterator ChromosomesIter;

    static ChromosomeMap * getInstance() {
        static ChromosomeMap * inst = new ChromosomeMap();
        return inst;
    }

    virtual void    configure( std::istream & config );

/**
 *  The number of chromosomes currently being tracked
 */
    virtual size_t    size() const;

/**
 *  Find a chromosome id by the chromosomes name.
 *
 *  It is possible to create missing chromosomes
 *
 *  String matching is done case-sensitively, by default
 */
    virtual chromid_t find( const String & name, bool bCreateMissing = false );

    virtual ChromosomePtr find( chromid_t id );

/**
 * Iterators that traverse the chromosomes in name lexicographical order
 * 
 * NOTE: ID order may not reflected sorted index
 */
    virtual ChromosomesIter begin() const;
    virtual ChromosomesIter end() const;

    virtual ~ChromosomeMap();
protected:
    ChromosomeMap(): m_chroms( new Chromosomes() ) {}

    shared_ptr< Chromosomes >   m_chroms;
};

#endif  // CHROMOSOMEMAP_H_
