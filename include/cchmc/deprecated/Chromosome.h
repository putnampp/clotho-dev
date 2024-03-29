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

#ifndef CHROMOSOME_H_
#define CHROMOSOME_H_

#include "common.h"
#include <cassert>
#include "Cloneable.h"
#include "Configurable.h"

#include <map>

using std::map;
using std::make_pair;

#define DEFAULT_CHROMOSOME_LEN 100

//typedef unsigned int chromid_t;
#define UNKNOWN_CHROM -1

class ChromosomeMap;

/**
 *
 * A Chromosome is a named sequence of positions.
 *
 * The chromosome length is at least the largest position.
 *
 */
class Chromosome : public Cloneable, public Configurable {
public:
    friend class ChromosomeMap;

    Chromosome( const string & n, size_t s = DEFAULT_CHROMOSOME_LEN);
    Chromosome( const Chromosome & c );

    virtual void setLength( size_t len );
    virtual Cloneable * clone();
    virtual void    configure( istream & config );

    virtual bool    add_site( pos_t pos );
    virtual void    clear_sites();

    chromid_t id() const;
    string  name() const;
    size_t  length() const;

    size_t  loci() const;
    bool    is_locus( pos_t pos, pos_t & offset ) const;

    virtual ~Chromosome();
protected:
    string      m_name;
    chromid_t   m_id;
    pos_t       m_size;

    typedef map<size_t, size_t> Sites;
    Sites m_sites;

private:
    static chromid_t nextID();
};

typedef shared_ptr< Chromosome > ChromosomePtr;

static const ChromosomePtr NULL_CHROMOSOME = ChromosomePtr();

#endif  // CHROMOSOME_H_
